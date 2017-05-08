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
#ifndef	_LINUX_PCI_H_
#define	_LINUX_PCI_H_

#define	CONFIG_PCI_MSI

#include <linux/types.h>

#include <sys/param.h>
#include <sys/bus.h>
#include <sys/pciio.h>
#include <sys/rman.h>
#include <dev/pci/pcivar.h>
#include <dev/pci/pcireg.h>
#include <dev/pci/pci_private.h>
#include <sys/resourcevar.h>

#include <machine/resource.h>

#include <linux/mod_devicetable.h>

#include <linux/list.h>
#include <linux/dmapool.h>
#include <linux/dma-mapping.h>
#include <linux/compiler.h>
#include <linux/errno.h>
#include <linux/kobject.h>

#include <asm/atomic.h>
#include <linux/device.h>
#include <linux/ioport.h>
#include <linux/pci_ids.h>



struct bus_attribute {
	struct attribute	attr;
	ssize_t (*show)(struct bus_type *bus, char *buf);
	ssize_t (*store)(struct bus_type *bus, const char *buf, size_t count);
};

#define BUS_ATTR(_name, _mode, _show, _store)	\
	struct bus_attribute bus_attr_##_name = __ATTR(_name, _mode, _show, _store)
#define BUS_ATTR_RW(_name) \
	struct bus_attribute bus_attr_##_name = __ATTR_RW(_name)
#define BUS_ATTR_RO(_name) \
	struct bus_attribute bus_attr_##_name = __ATTR_RO(_name)

extern int __must_check bus_create_file(struct bus_type *,
					struct bus_attribute *);
extern void bus_remove_file(struct bus_type *, struct bus_attribute *);


#define PCI_BASE_CLASS_DISPLAY		0x03
#define PCI_CLASS_DISPLAY_VGA		0x0300
#define PCI_CLASS_DISPLAY_XGA		0x0301
#define PCI_CLASS_DISPLAY_3D		0x0302
#define PCI_CLASS_DISPLAY_OTHER		0x0380

#define PCI_BASE_CLASS_BRIDGE		0x06
#define PCI_CLASS_BRIDGE_HOST		0x0600
#define PCI_CLASS_BRIDGE_ISA		0x0601
#define PCI_CLASS_BRIDGE_EISA		0x0602
#define PCI_CLASS_BRIDGE_MC		0x0603
#define PCI_CLASS_BRIDGE_PCI		0x0604
#define PCI_CLASS_BRIDGE_PCMCIA		0x0605
#define PCI_CLASS_BRIDGE_NUBUS		0x0606
#define PCI_CLASS_BRIDGE_CARDBUS	0x0607
#define PCI_CLASS_BRIDGE_RACEWAY	0x0608
#define PCI_CLASS_BRIDGE_OTHER		0x0680

#define PCI_HEADER_TYPE		0x0e	/* 8 bits */
#define  PCI_HEADER_TYPE_NORMAL		0
#define  PCI_HEADER_TYPE_BRIDGE		1
#define  PCI_HEADER_TYPE_CARDBUS	2

#define PCI_CFG_SPACE_SIZE	256
#define PCI_CFG_SPACE_EXP_SIZE	4096


int __must_check pci_create_sysfs_dev_files(struct pci_dev *pdev);
void pci_remove_sysfs_dev_files(struct pci_dev *pdev);
static inline void pci_create_firmware_label_files(struct pci_dev *pdev)
{ return; }
static inline void pci_remove_firmware_label_files(struct pci_dev *pdev)
{ return; }


struct pci_device_id {
	uint32_t	vendor;
	uint32_t	device;
        uint32_t	subvendor;
	uint32_t	subdevice;
	uint32_t	class;
	uint32_t	class_mask;
	uintptr_t	driver_data;
};

#define	MODULE_DEVICE_TABLE(bus, table)
#define	PCI_ANY_ID		(-1)
#define	PCI_VENDOR_ID_APPLE		0x106b
#define	PCI_VENDOR_ID_ASUSTEK		0x1043
#define	PCI_VENDOR_ID_ATI		0x1002
#define	PCI_VENDOR_ID_DELL		0x1028
#define	PCI_VENDOR_ID_HP		0x103c
#define	PCI_VENDOR_ID_IBM		0x1014
#define	PCI_VENDOR_ID_INTEL		0x8086
#define	PCI_VENDOR_ID_MELLANOX			0x15b3
#define	PCI_VENDOR_ID_SERVERWORKS	0x1166
#define	PCI_VENDOR_ID_SONY		0x104d
#define	PCI_VENDOR_ID_TOPSPIN			0x1867
#define	PCI_VENDOR_ID_VIA		0x1106
#define	PCI_DEVICE_ID_MELLANOX_TAVOR		0x5a44
#define	PCI_DEVICE_ID_MELLANOX_TAVOR_BRIDGE	0x5a46
#define	PCI_DEVICE_ID_MELLANOX_ARBEL_COMPAT	0x6278
#define	PCI_DEVICE_ID_MELLANOX_ARBEL		0x6282
#define	PCI_DEVICE_ID_MELLANOX_SINAI_OLD	0x5e8c
#define	PCI_DEVICE_ID_MELLANOX_SINAI		0x6274

#define PCI_DEVFN(slot, func)   ((((slot) & 0x1f) << 3) | ((func) & 0x07))
#define PCI_SLOT(devfn)         (((devfn) >> 3) & 0x1f)
#define PCI_FUNC(devfn)         ((devfn) & 0x07)

#define PCI_VDEVICE(_vendor, _device)					\
	    .vendor = PCI_VENDOR_ID_##_vendor, .device = (_device),	\
	    .subvendor = PCI_ANY_ID, .subdevice = PCI_ANY_ID
#define	PCI_DEVICE(_vendor, _device)					\
	    .vendor = (_vendor), .device = (_device),			\
	    .subvendor = PCI_ANY_ID, .subdevice = PCI_ANY_ID

#define	to_pci_dev(n)	container_of(n, struct pci_dev, dev)

struct pci_dev *linux_pci_get_device(unsigned int vendor, unsigned int device,
				struct pci_dev *from);


#define for_each_pci_dev(d) while ((d = linux_pci_get_device(PCI_ANY_ID, PCI_ANY_ID, d)) != NULL)


#define	PCI_VENDOR_ID		PCIR_DEVVENDOR
#define	PCI_COMMAND		PCIR_COMMAND
#define	PCI_EXP_DEVCTL		PCIER_DEVICE_CTL		/* Device Control */
#define	PCI_EXP_LNKCTL		PCIER_LINK_CTL			/* Link Control */
#define	PCI_EXP_FLAGS_TYPE	PCIEM_FLAGS_TYPE		/* Device/Port type */
#define	PCI_EXP_DEVCAP		PCIER_DEVICE_CAP		/* Device capabilities */
#define	PCI_EXP_DEVSTA		PCIER_DEVICE_STA		/* Device Status */
#define	PCI_EXP_LNKCAP		PCIER_LINK_CAP			/* Link Capabilities */
#define	PCI_EXP_LNKSTA		PCIER_LINK_STA			/* Link Status */
#define	PCI_EXP_SLTCAP		PCIER_SLOT_CAP			/* Slot Capabilities */
#define	PCI_EXP_SLTCTL		PCIER_SLOT_CTL			/* Slot Control */
#define	PCI_EXP_SLTSTA		PCIER_SLOT_STA			/* Slot Status */
#define	PCI_EXP_RTCTL		PCIER_ROOT_CTL			/* Root Control */
#define	PCI_EXP_RTCAP		PCIER_ROOT_CAP			/* Root Capabilities */
#define	PCI_EXP_RTSTA		PCIER_ROOT_STA			/* Root Status */
#define	PCI_EXP_DEVCAP2		PCIER_DEVICE_CAP2		/* Device Capabilities 2 */
#define	PCI_EXP_DEVCTL2		PCIER_DEVICE_CTL2		/* Device Control 2 */
#define	PCI_EXP_LNKCAP2		PCIER_LINK_CAP2			/* Link Capabilities 2 */
#define	PCI_EXP_LNKCTL2		PCIER_LINK_CTL2			/* Link Control 2 */
#define	PCI_EXP_LNKSTA2		PCIER_LINK_STA2			/* Link Status 2 */
#define	PCI_EXP_FLAGS		PCIER_FLAGS			/* Capabilities register */
#define	PCI_EXP_FLAGS_VERS	PCIEM_FLAGS_VERSION		/* Capability version */
#define	PCI_EXP_TYPE_ROOT_PORT	PCIEM_TYPE_ROOT_PORT		/* Root Port */
#define	PCI_EXP_TYPE_ENDPOINT	PCIEM_TYPE_ENDPOINT		/* Express Endpoint */
#define	PCI_EXP_TYPE_LEG_END	PCIEM_TYPE_LEGACY_ENDPOINT	/* Legacy Endpoint */
#define	PCI_EXP_TYPE_DOWNSTREAM PCIEM_TYPE_DOWNSTREAM_PORT	/* Downstream Port */
#define	PCI_EXP_FLAGS_SLOT	PCIEM_FLAGS_SLOT		/* Slot implemented */
#define	PCI_EXP_TYPE_RC_EC	PCIEM_TYPE_ROOT_EC		/* Root Complex Event Collector */
#define	PCI_EXP_LNKCAP_SLS_2_5GB 0x01	/* Supported Link Speed 2.5GT/s */
#define	PCI_EXP_LNKCAP_SLS_5_0GB 0x02	/* Supported Link Speed 5.0GT/s */
#define	PCI_EXP_LNKCAP_MLW	0x03f0	/* Maximum Link Width */
#define	PCI_EXP_LNKCAP2_SLS_2_5GB 0x02	/* Supported Link Speed 2.5GT/s */
#define	PCI_EXP_LNKCAP2_SLS_5_0GB 0x04	/* Supported Link Speed 5.0GT/s */
#define	PCI_EXP_LNKCAP2_SLS_8_0GB 0x08	/* Supported Link Speed 8.0GT/s */

#define PCI_EXP_LNKCTL_HAWD	PCIEM_LINK_CTL_HAWD
#define PCI_EXP_LNKCAP_CLKPM	0x00040000
#define PCI_EXP_DEVSTA_TRPND	0x0020

enum pci_bus_speed {
	PCI_SPEED_UNKNOWN = -1,
	PCIE_SPEED_2_5GT,
	PCIE_SPEED_5_0GT,
	PCIE_SPEED_8_0GT,
};

typedef unsigned short __bitwise pci_bus_flags_t;
enum pci_bus_flags {
	PCI_BUS_FLAGS_NO_MSI   = (__force pci_bus_flags_t) 1,
	PCI_BUS_FLAGS_NO_MMRBC = (__force pci_bus_flags_t) 2,
};

enum pcie_link_width {
	PCIE_LNK_WIDTH_UNKNOWN = 0xFF,
};


/*
 *  For PCI devices, the region numbers are assigned this way:
 */
enum {
	/* #0-5: standard PCI resources */
	PCI_STD_RESOURCES,
	PCI_STD_RESOURCE_END = 5,

	/* #6: expansion ROM resource */
	PCI_ROM_RESOURCE,

	/* device specific resources */
#ifdef CONFIG_PCI_IOV
	PCI_IOV_RESOURCES,
	PCI_IOV_RESOURCE_END = PCI_IOV_RESOURCES + PCI_SRIOV_NUM_BARS - 1,
#endif

	/* resources assigned to buses behind the bridge */
#define PCI_BRIDGE_RESOURCE_NUM 4

	PCI_BRIDGE_RESOURCES,
	PCI_BRIDGE_RESOURCE_END = PCI_BRIDGE_RESOURCES +
				  PCI_BRIDGE_RESOURCE_NUM - 1,

	/* total resources associated with a PCI device */
	PCI_NUM_RESOURCES,

	/* preserve this for compatibility */
	DEVICE_COUNT_RESOURCE = PCI_NUM_RESOURCES,
};

typedef int pci_power_t;

#define PCI_D0	PCI_POWERSTATE_D0
#define PCI_D1	PCI_POWERSTATE_D1
#define PCI_D2	PCI_POWERSTATE_D2
#define PCI_D3hot	PCI_POWERSTATE_D3
#define PCI_D3cold	4

#define PCI_POWER_ERROR	PCI_POWERSTATE_UNKNOWN

/* Remember to update this when the list above changes! */
extern const char *pci_power_names[];

static inline const char *pci_power_name(pci_power_t state)
{
	return pci_power_names[1 + (int) state];
}

struct pci_dev;

struct pci_driver {
	struct list_head		links;
	char				*name;
	const struct pci_device_id		*id_table;
	int  (*probe)(struct pci_dev *dev, const struct pci_device_id *id);
	void (*remove)(struct pci_dev *dev);
        int  (*suspend) (struct pci_dev *dev, pm_message_t state);      /* Device suspended */
        int  (*suspend_late) (struct pci_dev *dev, pm_message_t state);
        int  (*resume_early) (struct pci_dev *dev);
        int  (*resume) (struct pci_dev *dev);                   /* Device woken up */
	void (*shutdown) (struct pci_dev *dev);		/* Device shutdown */
	int (*sriov_configure) (struct pci_dev *dev, int num_vfs); /* PF pdev */
	driver_t			bsd_driver;
	devclass_t			*bsdclass;
	char				*busname;
	struct device_driver	driver;
        const struct pci_error_handlers       *err_handler;
};

extern struct list_head pci_drivers;
extern struct list_head pci_devices;
extern spinlock_t pci_lock;

#define	__devexit_p(x)	x

#define PCI_BRIDGE_RESOURCE_NUM 4
#if defined(__i386__) || defined(__amd64__)
extern unsigned long pci_mem_start;
#define PCIBIOS_MIN_IO		0x1000
#define PCIBIOS_MIN_MEM		(pci_mem_start)
#endif

struct pci_bus {
	struct list_head node;		/* node in list of buses */
	struct pci_bus	*parent;	/* parent bus this bridge is on */
	struct list_head children;	/* list of child buses */
	struct list_head devices;	/* list of devices on this bus */
	struct pci_dev	*self;		/* bridge device as seen by parent */
	struct list_head slots;		/* list of slots on this bus */
	struct linux_resource *linux_resource[PCI_BRIDGE_RESOURCE_NUM];
	struct list_head resources;	/* address space routed to this bus */
	struct resource busn_res;	/* bus numbers routed to this bus */

	struct pci_ops	*ops;		/* configuration access functions */
	void		*sysdata;	/* hook for sys-specific extension */

	unsigned char	number;		/* bus number */
	unsigned char   max_bus_speed;  /* enum pci_bus_speed */
	pci_bus_flags_t bus_flags;	/* inherited by child buses */
	struct device		dev;
};

#define to_pci_bus(n)	container_of(n, struct pci_bus, dev)


int __must_check pci_bus_alloc_resource(struct pci_bus *bus,
			struct linux_resource *res, resource_size_t size,
			resource_size_t align, resource_size_t min,
			unsigned int type_mask,
			resource_size_t (*alignf)(void *,
						  const struct linux_resource *,
						  resource_size_t,
						  resource_size_t),
			void *alignf_data);


extern resource_size_t pcibios_align_resource(void *data, const struct linux_resource *res,
		       resource_size_t size, resource_size_t align);

extern int release_resource(struct linux_resource *old);

#define LINUXKPI_BIOS 6
#define LINUXKPI_MAX_PCI_RESOURCE 7

struct pci_resources {
	struct resource *r[LINUXKPI_MAX_PCI_RESOURCE];
	int rid[LINUXKPI_MAX_PCI_RESOURCE];
	void *map[LINUXKPI_MAX_PCI_RESOURCE];
	int type[LINUXKPI_MAX_PCI_RESOURCE];
};


struct pci_vpd_ops {
	ssize_t (*read)(struct pci_dev *dev, loff_t pos, size_t count, void *buf);
	ssize_t (*write)(struct pci_dev *dev, loff_t pos, size_t count, const void *buf);
	int (*set_size)(struct pci_dev *dev, size_t len);
};

struct pci_vpd {
	const struct pci_vpd_ops *ops;
	struct bin_attribute *attr; /* descriptor for sysfs VPD entry */
	struct mutex	lock;
	unsigned int	len;
	u16		flag;
	u8		cap;
	u8		busy:1;
	u8		valid:1;
};


struct pci_dev {
	struct list_head bus_list;	/* node in per-bus list */
	struct pci_bus	*bus;		/* bus this device is on */
	struct pci_bus	*subordinate;	/* bus this device bridges to */

	struct device		dev;
	int			cfg_size;	/* Size of configuration space */

	struct list_head	links;
	struct pci_driver	*pdrv;
	struct pci_resources	pcir;
	uint64_t		dma_mask;
	unsigned int		devfn;
	uint16_t		vendor;
	uint16_t		device;
	uint16_t		subsystem_vendor;
	uint16_t		subsystem_device;
	unsigned int		class;
	u8			revision;
	u8			hdr_type;	/* PCI header type (`multi' flag masked out) */

	pci_power_t     current_state;  /* Current operating state. In ACPI-speak,
					   this is D0-D3, D0 being fully functional,
					   and D3 being off. */
	unsigned int	d1_support:1;	/* Low power state D1 is supported */
	unsigned int	d2_support:1;	/* Low power state D2 is supported */
	unsigned int	no_d1d2:1;	/* D1 and D2 are forbidden */
	unsigned int	no_d3cold:1;	/* D3cold is forbidden */
	unsigned int	bridge_d3:1;	/* Allow D3 for bridge */
	unsigned int	d3cold_allowed:1;	/* D3cold is allowed by user */
	unsigned int	mmio_always_on:1;	/* disallow turning off io/mem
						   decoding during bar sizing */
	unsigned int	wakeup_prepared:1;
	unsigned int	runtime_d3cold:1;	/* whether go through runtime
						   D3cold, not set for devices
						   powered on/off by the
						   corresponding bridge */
	unsigned int	ignore_hotplug:1;	/* Ignore hotplug events */
	unsigned int	d3_delay;	/* D3->D0 transition time in ms */
	unsigned int	d3cold_delay;	/* D3cold->D0 transition time in ms */
	
	unsigned int		msi_enabled:1;
	unsigned int		msix_enabled:1;
	unsigned int		no_msi:1;	/* device may not use msi */
	unsigned int		no_64bit_msi:1;
	unsigned int		broken_parity_status:1;	/* Device generates false positive parity */
	unsigned int		is_physfn:1;
	unsigned int		is_virtfn:1;
	unsigned int		reset_fn:1;
	unsigned int		irq;
	atomic_t		enable_cnt;	/* pci_enable_device has been called */
	struct bin_attribute	*rom_attr; /* attribute descriptor for sysfs ROM entry */
	int			rom_attr_enabled; /* has display of the rom attribute been enabled? */
	struct linux_resource linux_resource[DEVICE_COUNT_RESOURCE]; /* I/O and memory regions + expansion ROMs */
	char *driver_override; /* Driver name to force a match */
	struct pci_vpd *vpd;
};


static inline struct resource_list_entry *
linux_pci_get_rle(struct pci_dev *pdev, int type, int rid)
{
	struct pci_devinfo *dinfo;
	struct resource_list *rl;

	dinfo = device_get_ivars(pdev->dev.bsddev);
	rl = &dinfo->resources;
	return resource_list_find(rl, type, rid);
}

static inline struct resource_list_entry *
linux_pci_get_bar(struct pci_dev *pdev, int bar)
{
	struct resource_list_entry *rle;

	bar = PCIR_BAR(bar);
	if ((rle = linux_pci_get_rle(pdev, SYS_RES_MEMORY, bar)) == NULL)
		rle = linux_pci_get_rle(pdev, SYS_RES_IOPORT, bar);
	return (rle);
}

static inline struct device *
linux_pci_find_irq_dev(unsigned int irq)
{
	struct pci_dev *pdev;
	struct device *found;

	found = NULL;
	spin_lock(&pci_lock);
	list_for_each_entry(pdev, &pci_devices, links) {
		if (irq == pdev->dev.irq ||
		    (irq >= pdev->dev.msix && irq < pdev->dev.msix_max)) {
			found = &pdev->dev;
			break;
		}
	}
	spin_unlock(&pci_lock);
	return (found);
}

static inline unsigned long
pci_resource_start(struct pci_dev *pdev, int bar)
{
	struct resource_list_entry *rle;

	if ((rle = linux_pci_get_bar(pdev, bar)) == NULL)
		return (0);
	return rle->start;
}

static inline unsigned long
pci_resource_len(struct pci_dev *pdev, int bar)
{
	struct resource_list_entry *rle;

	if ((rle = linux_pci_get_bar(pdev, bar)) == NULL)
		return (0);
	return rle->count;
}

static inline int
pci_resource_type(struct pci_dev *pdev, int bar)
{
	struct pci_map *pm;

	pm = pci_find_bar(pdev->dev.bsddev, PCIR_BAR(bar));
	if (!pm)
		return (-1);

	if (PCI_BAR_IO(pm->pm_value))
		return (SYS_RES_IOPORT);
	else
		return (SYS_RES_MEMORY);
}

/*
 * All drivers just seem to want to inspect the type not flags.
 */
static inline int
pci_resource_flags(struct pci_dev *pdev, int bar)
{
	int type;

	type = pci_resource_type(pdev, bar);
	if (type < 0)
		return (0);
	return (1 << type);
}


static inline const char *
pci_name(struct pci_dev *d)
{

	return device_get_desc(d->dev.bsddev);
}


static inline void *
pci_get_drvdata(struct pci_dev *pdev)
{

	return dev_get_drvdata(&pdev->dev);
}

static inline void
pci_set_drvdata(struct pci_dev *pdev, void *data)
{

	dev_set_drvdata(&pdev->dev, data);
}

static inline int
pci_enable_device(struct pci_dev *pdev)
{

	pci_enable_io(pdev->dev.bsddev, SYS_RES_IOPORT);
	pci_enable_io(pdev->dev.bsddev, SYS_RES_MEMORY);
	return (0);
}

static inline void
pci_disable_device(struct pci_dev *pdev)
{
	pci_disable_io(pdev->dev.bsddev, SYS_RES_IOPORT);
	pci_disable_io(pdev->dev.bsddev, SYS_RES_MEMORY);
}

static inline int
pci_set_master(struct pci_dev *pdev)
{

	pci_enable_busmaster(pdev->dev.bsddev);
	return (0);
}

static inline int
pci_set_power_state(struct pci_dev *pdev, int state)
{

	pci_set_powerstate(pdev->dev.bsddev, state);
	return (0);
}

static inline int
pci_clear_master(struct pci_dev *pdev)
{

	pci_disable_busmaster(pdev->dev.bsddev);
	return (0);
}

static inline int
pci_request_region(struct pci_dev *pdev, int bar, const char *res_name)
{
	int rid;
	int type;

	type = pci_resource_type(pdev, bar);
	if (type < 0)
		return (-ENODEV);
	rid = PCIR_BAR(bar);
	if (bus_alloc_resource_any(pdev->dev.bsddev, type, &rid,
	    RF_ACTIVE) == NULL)
		return (-EINVAL);
	return (0);
}

static inline void
pci_release_region(struct pci_dev *pdev, int bar)
{
	struct resource_list_entry *rle;

	if ((rle = linux_pci_get_bar(pdev, bar)) == NULL)
		return;
	bus_release_resource(pdev->dev.bsddev, rle->type, rle->rid, rle->res);
}

static inline void
pci_release_regions(struct pci_dev *pdev)
{
	int i;

	for (i = 0; i <= PCIR_MAX_BAR_0; i++)
		pci_release_region(pdev, i);
}

static inline int
pci_request_regions(struct pci_dev *pdev, const char *res_name)
{
	int error;
	int i;

	for (i = 0; i <= PCIR_MAX_BAR_0; i++) {
		error = pci_request_region(pdev, i, res_name);
		if (error && error != -ENODEV) {
			pci_release_regions(pdev);
			return (error);
		}
	}
	return (0);
}


static inline int
linux_pci_enable_msi(struct pci_dev *pdev)
{
	/*  not clear what address to use - ignore for now*/
	return (0);
}

static inline void
linux_pci_disable_msi(struct pci_dev *pdev)
{
	/* disable until clear how to do enable */
	/* pci_disable_msi(pdev->dev.bsddev); */
}


struct pci_dev *linux_pci_get_class(unsigned int class, struct pci_dev *from);

struct pci_dev *linux_bsddev_to_pci_dev(device_t dev);

static inline void
pci_disable_msix(struct pci_dev *pdev)
{

	pci_release_msi(pdev->dev.bsddev);
}

static inline bus_addr_t
pci_bus_address(struct pci_dev *pdev, int bar)
{

	return (pci_resource_start(pdev, bar));
}

#define	PCI_CAP_ID_EXP	PCIY_EXPRESS
#define	PCI_CAP_ID_PCIX	PCIY_PCIX
#define PCI_CAP_ID_AGP  PCIY_AGP
#define PCI_CAP_ID_PM   PCIY_PMG

#define PCI_EXP_DEVCTL		PCIER_DEVICE_CTL
#define PCI_EXP_DEVCTL_PAYLOAD	PCIEM_CTL_MAX_PAYLOAD
#define PCI_EXP_DEVCTL_READRQ	PCIEM_CTL_MAX_READ_REQUEST
#define PCI_EXP_LNKCTL		PCIER_LINK_CTL
#define PCI_EXP_LNKSTA		PCIER_LINK_STA

static inline int
pci_find_capability(struct pci_dev *pdev, int capid)
{
	int reg;

	if (pci_find_cap(pdev->dev.bsddev, capid, &reg))
		return (0);
	return (reg);
}

static inline int pci_pcie_cap(struct pci_dev *dev)
{
        return pci_find_capability(dev, PCI_CAP_ID_EXP);
}


static inline int
pci_read_config_byte(struct pci_dev *pdev, int where, u8 *val)
{

	*val = (u8)pci_read_config(pdev->dev.bsddev, where, 1);
	return (0);
}

static inline int
pci_read_config_word(struct pci_dev *pdev, int where, u16 *val)
{

	*val = (u16)pci_read_config(pdev->dev.bsddev, where, 2);
	return (0);
}

static inline int
pci_read_config_dword(struct pci_dev *pdev, int where, u32 *val)
{

	*val = (u32)pci_read_config(pdev->dev.bsddev, where, 4);
	return (0);
}

static inline int
pci_write_config_byte(struct pci_dev *pdev, int where, u8 val)
{

	pci_write_config(pdev->dev.bsddev, where, val, 1);
	return (0);
}

static inline int
pci_write_config_word(struct pci_dev *pdev, int where, u16 val)
{

	pci_write_config(pdev->dev.bsddev, where, val, 2);
	return (0);
}

static inline int
pci_write_config_dword(struct pci_dev *pdev, int where, u32 val)
{

	pci_write_config(pdev->dev.bsddev, where, val, 4);
	return (0);
}

static inline int
pci_bus_read_config(struct pci_bus *bus, unsigned int devfn,
		    int where, uint32_t *val, int size)
{
	device_t dev;
	int dom, busid, slot, func;

	dom = pci_get_domain(bus->self->dev.bsddev);
	busid = pci_get_bus(bus->self->dev.bsddev);
	slot = ((devfn >> 3) & 0x1f);
	func = devfn & 0x7;
	dev = pci_find_dbsf(dom, busid, slot, func);
	*val = pci_read_config(dev, where, size);
	return (0);
}

static inline int
pci_bus_read_config_word(struct pci_bus *bus, unsigned int devfn, int where, u16 *val)
{
	return (pci_bus_read_config(bus, devfn, where, (uint32_t *)val, 2));
}

static inline int
pci_bus_read_config_byte(struct pci_bus *bus, unsigned int devfn, int where, u8 *val)
{
	return (pci_bus_read_config(bus, devfn, where, (uint32_t *)val, 1));
}

extern struct pci_dev *pci_get_bus_and_slot(unsigned int bus, unsigned int devfn);

void pci_dev_put(struct pci_dev *pdev);
extern int pci_register_driver(struct pci_driver *pdrv);
extern void pci_unregister_driver(struct pci_driver *pdrv);
extern void *pci_iomap(struct pci_dev *pdev, int bar, unsigned long max);
extern void pci_iounmap(struct pci_dev *pdev, void *regs);

struct msix_entry {
	int entry;
	int vector;
};

/*
 * Enable msix, positive errors indicate actual number of available
 * vectors.  Negative errors are failures.
 *
 * NB: define added to prevent this definition of pci_enable_msix from
 * clashing with the native FreeBSD version.
 */
#define	pci_enable_msix		linux_pci_enable_msix
static inline int
pci_enable_msix(struct pci_dev *pdev, struct msix_entry *entries, int nreq)
{
	struct resource_list_entry *rle;
	int error;
	int avail;
	int i;

	avail = pci_msix_count(pdev->dev.bsddev);
	if (avail < nreq) {
		if (avail == 0)
			return -EINVAL;
		return avail;
	}
	avail = nreq;
	if ((error = -pci_alloc_msix(pdev->dev.bsddev, &avail)) != 0)
		return error;
	/*
	 * Handle case where "pci_alloc_msix()" may allocate less
	 * interrupts than available and return with no error:
	 */
	if (avail < nreq) {
		pci_release_msi(pdev->dev.bsddev);
		return avail;
	}
	rle = linux_pci_get_rle(pdev, SYS_RES_IRQ, 1);
	pdev->dev.msix = rle->start;
	pdev->dev.msix_max = rle->start + avail;
	for (i = 0; i < nreq; i++)
		entries[i].vector = pdev->dev.msix + i;
	return (0);
}

#define	pci_enable_msix_range	linux_pci_enable_msix_range
static inline int
pci_enable_msix_range(struct pci_dev *dev, struct msix_entry *entries,
    int minvec, int maxvec)
{
	int nvec = maxvec;
	int rc;

	if (maxvec < minvec)
		return (-ERANGE);

	do {
		rc = pci_enable_msix(dev, entries, nvec);
		if (rc < 0) {
			return (rc);
		} else if (rc > 0) {
			if (rc < minvec)
				return (-ENOSPC);
			nvec = rc;
		}
	} while (rc);
	return (nvec);
}

static inline int pci_channel_offline(struct pci_dev *pdev)
{
        return false;
}

static inline int pci_enable_sriov(struct pci_dev *dev, int nr_virtfn)
{
        return -ENODEV;
}
static inline void pci_disable_sriov(struct pci_dev *dev)
{
}

#define DEFINE_PCI_DEVICE_TABLE(_table) \
	const struct pci_device_id _table[] __devinitdata


/* XXX This should not be necessary. */
#define	pcix_set_mmrbc(d, v)	0
#define	pcix_get_max_mmrbc(d)	0
#define	pcie_set_readrq(d, v)	0

#define	PCI_DMA_BIDIRECTIONAL	0
#define	PCI_DMA_TODEVICE	1
#define	PCI_DMA_FROMDEVICE	2
#define	PCI_DMA_NONE		3

#define	pci_pool		dma_pool
#define	pci_pool_destroy(...)	dma_pool_destroy(__VA_ARGS__)
#define	pci_pool_alloc(...)	dma_pool_alloc(__VA_ARGS__)
#define	pci_pool_free(...)	dma_pool_free(__VA_ARGS__)
#define	pci_pool_create(_name, _pdev, _size, _align, _alloc)		\
	    dma_pool_create(_name, &(_pdev)->dev, _size, _align, _alloc)
#define	pci_free_consistent(_hwdev, _size, _vaddr, _dma_handle)		\
	    dma_free_coherent((_hwdev) == NULL ? NULL : &(_hwdev)->dev,	\
		_size, _vaddr, _dma_handle)
#define	pci_map_sg(_hwdev, _sg, _nents, _dir)				\
	    dma_map_sg((_hwdev) == NULL ? NULL : &(_hwdev->dev),	\
		_sg, _nents, (enum dma_data_direction)_dir)
#define	pci_map_single(_hwdev, _ptr, _size, _dir)			\
	    dma_map_single((_hwdev) == NULL ? NULL : &(_hwdev->dev),	\
		(_ptr), (_size), (enum dma_data_direction)_dir)
#define	pci_unmap_single(_hwdev, _addr, _size, _dir)			\
	    dma_unmap_single((_hwdev) == NULL ? NULL : &(_hwdev)->dev,	\
		_addr, _size, (enum dma_data_direction)_dir)
#define	pci_unmap_sg(_hwdev, _sg, _nents, _dir)				\
	    dma_unmap_sg((_hwdev) == NULL ? NULL : &(_hwdev)->dev,	\
		_sg, _nents, (enum dma_data_direction)_dir)
#define	pci_map_page(_hwdev, _page, _offset, _size, _dir)		\
	    dma_map_page((_hwdev) == NULL ? NULL : &(_hwdev)->dev, _page,\
		_offset, _size, (enum dma_data_direction)_dir)
#define	pci_unmap_page(_hwdev, _dma_address, _size, _dir)		\
	    dma_unmap_page((_hwdev) == NULL ? NULL : &(_hwdev)->dev,	\
		_dma_address, _size, (enum dma_data_direction)_dir)
#define	pci_set_dma_mask(_pdev, mask)	dma_set_mask(&(_pdev)->dev, (mask))
#define	pci_dma_mapping_error(_pdev, _dma_addr)				\
	    dma_mapping_error(&(_pdev)->dev, _dma_addr)
#define	pci_set_consistent_dma_mask(_pdev, _mask)			\
	    dma_set_coherent_mask(&(_pdev)->dev, (_mask))
#define	DECLARE_PCI_UNMAP_ADDR(x)	DEFINE_DMA_UNMAP_ADDR(x);
#define	DECLARE_PCI_UNMAP_LEN(x)	DEFINE_DMA_UNMAP_LEN(x);
#define	pci_unmap_addr		dma_unmap_addr
#define	pci_unmap_addr_set	dma_unmap_addr_set
#define	pci_unmap_len		dma_unmap_len
#define	pci_unmap_len_set	dma_unmap_len_set

typedef unsigned int __bitwise pci_channel_state_t;
typedef unsigned int __bitwise pci_ers_result_t;

enum pci_channel_state {
        pci_channel_io_normal = 1,
        pci_channel_io_frozen = 2,
        pci_channel_io_perm_failure = 3,
};

enum pci_ers_result {
        PCI_ERS_RESULT_NONE = 1,
        PCI_ERS_RESULT_CAN_RECOVER = 2,
        PCI_ERS_RESULT_NEED_RESET = 3,
        PCI_ERS_RESULT_DISCONNECT = 4,
        PCI_ERS_RESULT_RECOVERED = 5,
};


/* PCI bus error event callbacks */
struct pci_error_handlers {
        pci_ers_result_t (*error_detected)(struct pci_dev *dev,
                        enum pci_channel_state error);
        pci_ers_result_t (*mmio_enabled)(struct pci_dev *dev);
        pci_ers_result_t (*link_reset)(struct pci_dev *dev);
        pci_ers_result_t (*slot_reset)(struct pci_dev *dev);
        void (*resume)(struct pci_dev *dev);
};

/* FreeBSD does not support SRIOV - yet */
static inline struct pci_dev *pci_physfn(struct pci_dev *dev)
{
        return dev;
}

static inline bool pci_is_pcie(struct pci_dev *dev)
{
        return !!pci_pcie_cap(dev);
}

static inline u16 pcie_flags_reg(struct pci_dev *dev)
{
        int pos;
        u16 reg16;

        pos = pci_find_capability(dev, PCI_CAP_ID_EXP);
        if (!pos)
                return 0;

        pci_read_config_word(dev, pos + PCI_EXP_FLAGS, &reg16);

        return reg16;
}


static inline int pci_pcie_type(struct pci_dev *dev)
{
        return (pcie_flags_reg(dev) & PCI_EXP_FLAGS_TYPE) >> 4;
}

static inline int pcie_cap_version(struct pci_dev *dev)
{
        return pcie_flags_reg(dev) & PCI_EXP_FLAGS_VERS;
}

static inline bool pcie_cap_has_lnkctl(struct pci_dev *dev)
{
        int type = pci_pcie_type(dev);

        return pcie_cap_version(dev) > 1 ||
               type == PCI_EXP_TYPE_ROOT_PORT ||
               type == PCI_EXP_TYPE_ENDPOINT ||
               type == PCI_EXP_TYPE_LEG_END;
}

static inline bool pcie_cap_has_devctl(const struct pci_dev *dev)
{
                return true;
}

static inline bool pcie_cap_has_sltctl(struct pci_dev *dev)
{
        int type = pci_pcie_type(dev);

        return pcie_cap_version(dev) > 1 ||
               type == PCI_EXP_TYPE_ROOT_PORT ||
               (type == PCI_EXP_TYPE_DOWNSTREAM &&
                pcie_flags_reg(dev) & PCI_EXP_FLAGS_SLOT);
}

static inline bool pcie_cap_has_rtctl(struct pci_dev *dev)
{
        int type = pci_pcie_type(dev);

        return pcie_cap_version(dev) > 1 ||
               type == PCI_EXP_TYPE_ROOT_PORT ||
               type == PCI_EXP_TYPE_RC_EC;
}

static bool pcie_capability_reg_implemented(struct pci_dev *dev, int pos)
{
        if (!pci_is_pcie(dev))
                return false;

        switch (pos) {
        case PCI_EXP_FLAGS_TYPE:
                return true;
        case PCI_EXP_DEVCAP:
        case PCI_EXP_DEVCTL:
        case PCI_EXP_DEVSTA:
                return pcie_cap_has_devctl(dev);
        case PCI_EXP_LNKCAP:
        case PCI_EXP_LNKCTL:
        case PCI_EXP_LNKSTA:
                return pcie_cap_has_lnkctl(dev);
        case PCI_EXP_SLTCAP:
        case PCI_EXP_SLTCTL:
        case PCI_EXP_SLTSTA:
                return pcie_cap_has_sltctl(dev);
        case PCI_EXP_RTCTL:
        case PCI_EXP_RTCAP:
        case PCI_EXP_RTSTA:
                return pcie_cap_has_rtctl(dev);
        case PCI_EXP_DEVCAP2:
        case PCI_EXP_DEVCTL2:
        case PCI_EXP_LNKCAP2:
        case PCI_EXP_LNKCTL2:
        case PCI_EXP_LNKSTA2:
                return pcie_cap_version(dev) > 1;
        default:
                return false;
        }
}

static inline int
pcie_capability_read_dword(struct pci_dev *dev, int pos, u32 *dst)
{
        if (pos & 3)
                return -EINVAL;

        if (!pcie_capability_reg_implemented(dev, pos))
                return -EINVAL;

        return pci_read_config_dword(dev, pci_pcie_cap(dev) + pos, dst);
}

static inline int
pcie_capability_read_word(struct pci_dev *dev, int pos, u16 *dst)
{
        if (pos & 3)
                return -EINVAL;

        if (!pcie_capability_reg_implemented(dev, pos))
                return -EINVAL;

        return pci_read_config_word(dev, pci_pcie_cap(dev) + pos, dst);
}

static inline int
pcie_capability_write_word(struct pci_dev *dev, int pos, u16 val)
{
        if (pos & 1)
                return -EINVAL;

        if (!pcie_capability_reg_implemented(dev, pos))
                return 0;

        return pci_write_config_word(dev, pci_pcie_cap(dev) + pos, val);
}

static inline int pcie_get_minimum_link(struct pci_dev *dev,
    enum pci_bus_speed *speed, enum pcie_link_width *width)
{
	*speed = PCI_SPEED_UNKNOWN;
	*width = PCIE_LNK_WIDTH_UNKNOWN;
	return (0);
}

static inline int
pci_num_vf(struct pci_dev *dev)
{
	return (0);
}

void pci_unmap_rom(struct pci_dev *pdev, u8 *bios);

void * pci_map_rom(struct pci_dev *pdev, size_t *size);

int pci_bus_read_config_byte(struct pci_bus *bus, unsigned int devfn,
			     int where, u8 *val);
int pci_bus_read_config_word(struct pci_bus *bus, unsigned int devfn,
			     int where, u16 *val);
int pci_bus_read_config_dword(struct pci_bus *bus, unsigned int devfn,
			      int where, u32 *val);
int pci_bus_write_config_byte(struct pci_bus *bus, unsigned int devfn,
			      int where, u8 val);
int pci_bus_write_config_word(struct pci_bus *bus, unsigned int devfn,
			      int where, u16 val);
int pci_bus_write_config_dword(struct pci_bus *bus, unsigned int devfn,
			       int where, u32 val);

int pci_generic_config_read(struct pci_bus *bus, unsigned int devfn,
			       int where, u32 val);


/* user-space driven config access */
/* From Linux:
 * 
 * The following routines are to prevent the user from accessing PCI config
 * space when it's unsafe to do so.  Some devices require this during BIST and
 * we're required to prevent it during D-state transitions.
 *
 * We have a bit per device to indicate it's blocked and a global wait queue
 * for callers to sleep on until devices are unblocked.
 *
 * I don't expect user space access during this time so ignoring for now
 */
#define pci_user_read_config_byte  pci_read_config_byte
#define pci_user_read_config_word  pci_read_config_word
#define pci_user_read_config_dword pci_read_config_dword

#define pci_user_write_config_byte  pci_write_config_byte
#define pci_user_write_config_word  pci_write_config_word
#define pci_user_write_config_dword pci_write_config_dword 


/* Functions for PCI Hotplug drivers to use */
int pci_bus_find_capability(struct pci_bus *bus, unsigned int devfn, int cap);
unsigned int pci_rescan_bus_bridge_resize(struct pci_dev *bridge);
unsigned int pci_rescan_bus(struct pci_bus *bus);
void pci_lock_rescan_remove(void);
void pci_unlock_rescan_remove(void);

/* Vital product data routines */

static inline ssize_t
pci_read_vpd(struct pci_dev *dev, loff_t pos, size_t count, void *buf)
{
	if (!dev->vpd || !dev->vpd->ops)
		return -ENODEV;
	return dev->vpd->ops->read(dev, pos, count, buf);
}

static inline ssize_t
pci_write_vpd(struct pci_dev *dev, loff_t pos, size_t count, const void *buf)
{
	if (!dev->vpd || !dev->vpd->ops)
		return -ENODEV;
	return dev->vpd->ops->write(dev, pos, count, buf);
}

int pci_set_vpd_size(struct pci_dev *dev, size_t len);

int pci_probe_reset_function(struct pci_dev *dev);
int pci_reset_function(struct pci_dev *dev);
struct pci_bus *pci_find_next_bus(const struct pci_bus *from);
void pci_stop_and_remove_bus_device(struct pci_dev *dev);
void pci_stop_and_remove_bus_device_locked(struct pci_dev *dev);

static inline bool pci_is_root_bus(struct pci_bus *pbus)
{

	return (pbus->self == NULL);
}

static inline struct pci_dev *pci_upstream_bridge(struct pci_dev *dev)
{
#ifdef __linux__
	dev = pci_physfn(dev);
	if (pci_is_root_bus(dev->bus))
		return NULL;

	return dev->bus->self;
#else
	UNIMPLEMENTED();
	return (NULL);
#endif
}

void *pci_platform_rom(struct pci_dev *pdev, size_t *size);


static inline void linux_pci_save_state(struct pci_dev *pdev){
	panic("implment me!!");
	UNIMPLEMENTED();
}

static inline void linux_pci_restore_state(struct pci_dev *pdev){
	panic("implment me!!");
	UNIMPLEMENTED();
}

static inline void pci_ignore_hotplug(struct pci_dev *pdev){
	UNIMPLEMENTED();
}

static inline void *
pci_alloc_consistent(struct pci_dev *hwdev, size_t size,
                     dma_addr_t *dma_handle)
{

	return dma_alloc_coherent(hwdev == NULL ? NULL : &hwdev->dev, size, dma_handle, GFP_ATOMIC);
}

static inline int
pcie_get_readrq(struct pci_dev *dev)
{
	u16 ctl;

	pcie_capability_read_word(dev, PCI_EXP_DEVCTL, &ctl);

	return 128 << ((ctl & PCI_EXP_DEVCTL_READRQ) >> 12);
}

static inline void
pci_resource_to_user(const struct pci_dev *dev, int bar,
		const struct linux_resource *rsrc, resource_size_t *start,
		resource_size_t *end)
{
	*start = rsrc->start;
	*end = rsrc->end;
}

#define PCI_DEVICE_ID_ATI_RADEON_QY     0x5159
#define pci_enable_msi linux_pci_enable_msi
#define pci_disable_msi linux_pci_disable_msi
#define pci_is_enabled(dev) (1)
#define resource linux_resource


#define pci_config_pm_runtime_get(dev)
#define pci_config_pm_runtime_put(dev)


#endif	/* _LINUX_PCI_H_ */
