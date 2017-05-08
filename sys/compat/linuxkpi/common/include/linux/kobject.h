/*-
 * Copyright (c) 2010 Isilon Systems, Inc.
 * Copyright (c) 2010 iX Systems, Inc.
 * Copyright (c) 2010 Panasas, Inc.
 * Copyright (c) 2013, 2014 Mellanox Technologies, Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD$
 */
#ifndef	_LINUX_KOBJECT_H_
#define	_LINUX_KOBJECT_H_

#include <machine/stdarg.h>

#include <linux/types.h>
#include <linux/list.h>
#include <linux/sysfs.h>
#include <linux/compiler.h>
#include <linux/spinlock.h>
#include <linux/kref.h>
#include <linux/kobject_ns.h>
#include <linux/kernel.h>
#include <linux/wait.h>
#include <linux/atomic.h>
#include <linux/workqueue.h>

#define UEVENT_HELPER_PATH_LEN		256
#define UEVENT_NUM_ENVP			32	/* number of env pointers */
#define UEVENT_BUFFER_SIZE		2048	/* buffer for the variables */

struct kobject;
struct sysctl_oid;
struct pfs_node;

enum kobject_action {
	KOBJ_ADD,
	KOBJ_REMOVE,
	KOBJ_CHANGE,
	KOBJ_MOVE,
	KOBJ_ONLINE,
	KOBJ_OFFLINE,
	KOBJ_MAX
};

enum kobj_ns_type {
	KOBJ_NS_TYPE_NONE = 0,
	KOBJ_NS_TYPE_NET,
	KOBJ_NS_TYPES
};

struct kobj_ns_type_operations {
	enum kobj_ns_type type;
	bool (*current_may_mount)(void);
	void *(*grab_current_ns)(void);
	const void *(*initial_ns)(void);
	void (*drop_ns)(void *);
};

struct kobj_type {
	void (*release)(struct kobject *kobj);
	const struct sysfs_ops *sysfs_ops;
	struct attribute **default_attrs;
	const struct kobj_ns_type_operations *(*child_ns_type)(struct kobject *kobj);
	const void *(*namespace)(struct kobject *kobj);	
};

struct kobj_uevent_env {
	char *argv[3];
	char *envp[UEVENT_NUM_ENVP];
	int envp_idx;
	char buf[UEVENT_BUFFER_SIZE];
	int buflen;
};

extern const struct kobj_type linux_kfree_type;


struct kobject {
	struct kobject		*parent;
	char			*name;
	struct kref		kref;
	const struct kobj_type	*ktype;
	struct list_head	entry;
	struct sysctl_oid	*oidp;
	struct pfs_node		*sd;
	unsigned int state_initialized:1;
	unsigned int state_in_sysfs:1;
	unsigned int state_add_uevent_sent:1;
	unsigned int state_remove_uevent_sent:1;
	unsigned int uevent_suppress:1;	
};

extern struct kobject *mm_kobj;


struct kobj_attribute {
        struct attribute attr;
        ssize_t (*show)(struct kobject *kobj, struct kobj_attribute *attr,
                        char *buf);
        ssize_t (*store)(struct kobject *kobj, struct kobj_attribute *attr,
                         const char *buf, size_t count);
};

static inline const struct kobj_ns_type_operations *
kobj_child_ns_ops(struct kobject *parent)
{
	const struct kobj_ns_type_operations *ops = NULL;

	if (parent && parent->ktype && parent->ktype->child_ns_type)
		ops = parent->ktype->child_ns_type(parent);

	return ops;
}

static inline const struct kobj_ns_type_operations *
kobj_ns_ops(struct kobject *kobj)
{
	return kobj_child_ns_ops(kobj->parent);
}

static inline void
kobject_init(struct kobject *kobj, const struct kobj_type *ktype)
{

	kref_init(&kobj->kref);
	INIT_LIST_HEAD(&kobj->entry);
	kobj->ktype = ktype;
	kobj->oidp = NULL;
	kobj->state_in_sysfs = 0;
}

void linux_kobject_release(struct kref *kref);

static inline void
kobject_put(struct kobject *kobj)
{

	if (kobj)
		kref_put(&kobj->kref, linux_kobject_release);
}

static inline struct kobject *
kobject_get(struct kobject *kobj)
{

	if (kobj)
		kref_get(&kobj->kref);
	return kobj;
}

int	kobject_set_name_vargs(struct kobject *kobj, const char *fmt, va_list);
int	kobject_add(struct kobject *kobj, struct kobject *parent,
	    const char *fmt, ...);

static inline struct kobject *
kobject_create(void)
{
	struct kobject *kobj;

	kobj = kzalloc(sizeof(*kobj), GFP_KERNEL);
	if (kobj == NULL)
		return (NULL);
	kobject_init(kobj, &linux_kfree_type);

	return (kobj);
}

static inline struct kobject *
kobject_create_and_add(const char *name, struct kobject *parent)
{
	struct kobject *kobj;

	kobj = kobject_create();
	if (kobj == NULL)
		return (NULL);
	if (kobject_add(kobj, parent, "%s", name) == 0)
		return (kobj);
	kobject_put(kobj);

	return (NULL);
}

static inline char *
kobject_name(const struct kobject *kobj)
{

	return kobj->name;
}

int	kobject_set_name(struct kobject *kobj, const char *fmt, ...);
int	kobject_init_and_add(struct kobject *kobj, const struct kobj_type *ktype,
	    struct kobject *parent, const char *fmt, ...);

static inline void
kobject_del(struct kobject *kobj)
{
	if (!kobj)
		return;
	sysfs_remove_dir(kobj);
	kobj->state_in_sysfs = 0;
	/* list removal? */
	kobject_put(kobj->parent);
	kobj->parent = NULL;
}

static inline void
kobject_uevent_env(struct kobject *kobj, enum kobject_action action, char *envp_ext[]) {}


static inline int
add_uevent_var(struct kobj_uevent_env *env, const char *format, ...)
{
	va_list args;
	int len;

	if (env->envp_idx >= ARRAY_SIZE(env->envp)) {
		WARN(1, KERN_ERR "add_uevent_var: too many keys\n");
		return -ENOMEM;
	}

	va_start(args, format);
	len = vsnprintf(&env->buf[env->buflen],
			sizeof(env->buf) - env->buflen,
			format, args);
	va_end(args);

	if (len >= (sizeof(env->buf) - env->buflen)) {
		WARN(1, KERN_ERR "add_uevent_var: buffer size too small\n");
		return -ENOMEM;
	}

	env->envp[env->envp_idx++] = &env->buf[env->buflen];
	env->buflen += len + 1;
	return 0;
}


#endif /* _LINUX_KOBJECT_H_ */
