#ifndef _ASM_X86_FTRACE_H
#define _ASM_X86_FTRACE_H

#ifdef __ASSEMBLY__

	.macro MCOUNT_SAVE_FRAME
	/* taken from glibc */
	subq $0x38, %rsp
	movq %rax, (%rsp)
	movq %rcx, 8(%rsp)
	movq %rdx, 16(%rsp)
	movq %rsi, 24(%rsp)
	movq %rdi, 32(%rsp)
	movq %r8, 40(%rsp)
	movq %r9, 48(%rsp)
	.endm

	.macro MCOUNT_RESTORE_FRAME
	movq 48(%rsp), %r9
	movq 40(%rsp), %r8
	movq 32(%rsp), %rdi
	movq 24(%rsp), %rsi
	movq 16(%rsp), %rdx
	movq 8(%rsp), %rcx
	movq (%rsp), %rax
	addq $0x38, %rsp
	.endm

#endif

#ifdef CONFIG_FUNCTION_TRACER
#define MCOUNT_ADDR		((long)(mcount))
#define MCOUNT_INSN_SIZE	5 /* sizeof mcount call */

#ifndef __ASSEMBLY__
extern void mcount(void);

static inline unsigned long ftrace_call_adjust(unsigned long addr)
{
	/*
	 * addr is the address of the mcount call instruction.
	 * recordmcount does the necessary offset calculation.
	 */
	return addr;
}

#ifdef CONFIG_DYNAMIC_FTRACE

struct dyn_arch_ftrace {
	/* No extra data needed for x86 */
};

#endif /*  CONFIG_DYNAMIC_FTRACE */
#endif /* __ASSEMBLY__ */
#endif /* CONFIG_FUNCTION_TRACER */


#if !defined(__ASSEMBLY__) && !defined(COMPILE_OFFSETS)

#if defined(CONFIG_FTRACE_SYSCALLS) && defined(CONFIG_IA32_EMULATION)
#include <asm/compat.h>

/*
 * Because ia32 syscalls do not map to x86_64 syscall numbers
 * this screws up the trace output when tracing a ia32 task.
 * Instead of reporting bogus syscalls, just do not trace them.
 *
 * If the user realy wants these, then they should use the
 * raw syscall tracepoints with filtering.
 */
#define ARCH_TRACE_IGNORE_COMPAT_SYSCALLS 1
static inline bool arch_trace_is_compat_syscall(struct pt_regs *regs)
{
	if (is_compat_task())
		return true;
	return false;
}
#endif /* CONFIG_FTRACE_SYSCALLS && CONFIG_IA32_EMULATION */
#endif /* !__ASSEMBLY__  && !COMPILE_OFFSETS */

#endif /* _ASM_X86_FTRACE_H */
