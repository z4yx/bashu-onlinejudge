/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet cin fdm=syntax : */

/*
 * Copyright (c) 2010, 2011 Ali Polatel <alip@exherbo.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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
 */

#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdio.h> /* NULL */

#include <pinktrace/internal.h>
#include <pinktrace/pink.h>

bool
pink_trace_me(void)
{
	return !(0 > ptrace(PTRACE_TRACEME, 0, NULL, NULL));
}

bool
pink_trace_cont(pid_t pid, int sig, PINK_GCC_ATTR((unused)) char *addr)
{
	return !(0 > ptrace(PTRACE_CONT, pid, NULL, sig));
}

bool
pink_trace_kill(pid_t pid)
{
	return !(0 > ptrace(PTRACE_KILL, pid, NULL, NULL));
}

bool
pink_trace_singlestep(pid_t pid, int sig)
{
	return !(0 > ptrace(PTRACE_SINGLESTEP, pid, NULL, sig));
}

bool
pink_trace_syscall(pid_t pid, int sig)
{
	return !(0 > ptrace(PTRACE_SYSCALL, pid, NULL, sig));
}

#ifdef PTRACE_SYSEMU
bool
pink_trace_sysemu(pid_t pid, int sig)
{
	return !(0 > ptrace(PTRACE_SYSEMU, pid, NULL, sig));
}
#else
bool
pink_trace_sysemu(PINK_GCC_ATTR((unused)) pid_t pid, PINK_GCC_ATTR((unused)) int sig)
{
	errno = ENOTSUP;
	return false;
}
#endif /* PTRACE_SYSEMU */

#ifdef PTRACE_SYSEMU_SINGLESTEP
bool
pink_trace_sysemu_singlestep(pid_t pid, int sig)
{
	return !(0 > ptrace(PTRACE_SYSEMU_SINGLESTEP, pid, NULL, sig));
}
#else
bool
pink_trace_sysemu_singlestep(PINK_GCC_ATTR((unused)) pid_t pid, PINK_GCC_ATTR((unused)) int sig)
{
	errno = ENOTSUP;
	return false;
}
#endif /* PTRACE_SYSEMU_SINGLESTEP */

bool
pink_trace_geteventmsg(pid_t pid, unsigned long *data)
{
	return !(0 > ptrace(PTRACE_GETEVENTMSG, pid, NULL, data));
}

bool
pink_trace_setup(pid_t pid, int options)
{
	int ptrace_options;

	ptrace_options = 0;
	if (options & PINK_TRACE_OPTION_SYSGOOD)
		ptrace_options |= PTRACE_O_TRACESYSGOOD;
	if (options & PINK_TRACE_OPTION_FORK)
		ptrace_options |= PTRACE_O_TRACEFORK;
	if (options & PINK_TRACE_OPTION_VFORK)
		ptrace_options |= PTRACE_O_TRACEVFORK;
	if (options & PINK_TRACE_OPTION_CLONE)
		ptrace_options |= PTRACE_O_TRACECLONE;
	if (options & PINK_TRACE_OPTION_EXEC)
		ptrace_options |= PTRACE_O_TRACEEXEC;
	if (options & PINK_TRACE_OPTION_VFORK_DONE)
		ptrace_options |= PTRACE_O_TRACEVFORKDONE;
	if (options & PINK_TRACE_OPTION_EXIT)
		ptrace_options |= PTRACE_O_TRACEEXIT;

	return !(0 > ptrace(PTRACE_SETOPTIONS, pid, NULL, ptrace_options));
}

bool
pink_trace_attach(pid_t pid)
{
	return !(0 > ptrace(PTRACE_ATTACH, pid, NULL, NULL));
}

bool
pink_trace_detach(pid_t pid, int sig)
{
	return !(0 > ptrace(PTRACE_DETACH, pid, NULL, sig));
}
