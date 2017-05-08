#ifndef _ASM_PROCESSOR_H
#define _ASM_PROCESSOR_H

#include <linux/types.h>

#include <asm/cpufeatures.h>

#include <machine/cpu.h>

#if defined(__i386__) || defined(__amd64__)
#include "processor-x86.h"
#endif

#define smp_mb() mb()
#define smp_wmb() wmb()
#define smp_rmb() rmb()

static __always_inline void cpu_relax(void)
{
	cpu_spinwait();
}

#endif
