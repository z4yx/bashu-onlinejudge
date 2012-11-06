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
	return !(0 > ptrace(PT_TRACE_ME, 0, NULL, 0));
}

bool
pink_trace_cont(pid_t pid, int sig, char *addr)
{
	return !(0 > ptrace(PT_CONTINUE, pid, addr, sig));
}

bool
pink_trace_kill(pid_t pid)
{
	return !(0 > ptrace(PT_KILL, pid, NULL, 0));
}

bool
pink_trace_singlestep(pid_t pid, int sig)
{
	return !(0 > ptrace(PT_STEP, pid, (caddr_t)1, sig));
}

bool
pink_trace_syscall(pid_t pid, int sig)
{
	return !(0 > ptrace(PT_SYSCALL, pid, (caddr_t)1, sig));
}

bool
pink_trace_syscall_entry(pid_t pid, int sig)
{
	return !(0 > ptrace(PT_TO_SCE, pid, (caddr_t)1, sig));
}

bool
pink_trace_syscall_exit(pid_t pid, int sig)
{
	return !(0 > ptrace(PT_TO_SCX, pid, (caddr_t)1, sig));
}

bool
pink_trace_lwpinfo(pid_t pid, void *info, size_t size)
{
	return !(0 > ptrace(PT_LWPINFO, pid, (caddr_t)info, size));
}

#ifdef PT_FOLLOW_FORK
bool
pink_trace_followfork(pid_t pid, bool on)
{
	return !(0 > ptrace(PT_FOLLOW_FORK, pid, (caddr_t)1, on));
}
#else
bool
pink_trace_followfork(PINK_GCC_ATTR((unused)) pid_t pid, PINK_GCC_ATTR((unused)) bool on)
{
	errno = ENOTSUP;
	return false;
}
#endif /* PT_FOLLOW_FORK */

bool
pink_trace_attach(pid_t pid)
{
	return !(0 > ptrace(PT_ATTACH, pid, NULL, 0));
}

bool
pink_trace_detach(pid_t pid, int sig)
{
	return !(0 > ptrace(PT_DETACH, pid, (caddr_t)1, sig));
}
