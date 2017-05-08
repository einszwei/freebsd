/*-
 * Copyright (c) 2010 Isilon Systems, Inc.
 * Copyright (c) 2010 iX Systems, Inc.
 * Copyright (c) 2010 Panasas, Inc.
 * Copyright (c) 2013-2016 Mellanox Technologies, Ltd.
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
#ifndef	_LINUX_NOTIFIER_H_
#define	_LINUX_NOTIFIER_H_

#include <sys/types.h>
#include <sys/eventhandler.h>

#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/rwsem.h>
#include <linux/srcu.h>


#define	NOTIFY_DONE	0
#define NOTIFY_OK	0x0001
#define NOTIFY_STOP_MASK	0x8000
#define NOTIFY_BAD	(NOTIFY_STOP_MASK|0x0002)

enum {
	NETDEV_CHANGE,
	NETDEV_UP,
	NETDEV_DOWN,
	NETDEV_REGISTER,
	NETDEV_UNREGISTER,
	NETDEV_CHANGEADDR,
	NETDEV_CHANGEIFADDR,
	LINUX_NOTIFY_TAGS		/* must be last */
};

struct notifier_block {
	int     (*notifier_call) (struct notifier_block *, unsigned long, void *);
	struct notifier_block *next;
	int	priority;
	eventhandler_tag tags[LINUX_NOTIFY_TAGS];
};

struct atomic_notifier_head {
	spinlock_t lock;
	struct notifier_block __rcu *head;
};

struct blocking_notifier_head {
	struct rw_semaphore rwsem;
	struct notifier_block __rcu *head;
};

#define ATOMIC_INIT_NOTIFIER_HEAD(name) do {	\
		spin_lock_init(&(name)->lock);	\
		(name)->head = NULL;		\
	} while (0)
#define BLOCKING_INIT_NOTIFIER_HEAD(name) do {	\
		init_rwsem(&(name)->rwsem);	\
		(name)->head = NULL;		\
	} while (0)


extern int atomic_notifier_call_chain(struct atomic_notifier_head *nh,
		unsigned long val, void *v);
extern int __atomic_notifier_call_chain(struct atomic_notifier_head *nh,
	unsigned long val, void *v, int nr_to_call, int *nr_calls);

extern int blocking_notifier_chain_register(struct blocking_notifier_head *nh,
		struct notifier_block *nb);
extern int blocking_notifier_call_chain(struct blocking_notifier_head *nh,
		unsigned long val, void *v);
extern int blocking_notifier_chain_unregister(struct blocking_notifier_head *nh,
		struct notifier_block *nb);


#endif					/* _LINUX_NOTIFIER_H_ */
