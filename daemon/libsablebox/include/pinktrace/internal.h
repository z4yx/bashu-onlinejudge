/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet cin fdm=syntax : */

/*
 * Copyright (c) 2010 Ali Polatel <alip@exherbo.org>
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

#ifndef PINKTRACE_GUARD_INTERNAL_H
#define PINKTRACE_GUARD_INTERNAL_H 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <limits.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include <netinet/in.h>
#include <sys/un.h>

#ifdef HAVE_SYS_REG_H
#include <sys/reg.h>
#endif /*  HAVE_SYS_REG_H */

#ifdef HAVE_MACHINE_PSL_H
#include <machine/psl.h>
#endif /* HAVE_MACHINE_PSL_H */

#ifdef HAVE_MACHINE_REG_H
#include <machine/reg.h>
#endif /* HAVE_MACHINE_REG_H */

/* We need additional hackery on IA64 to include linux/ptrace.h. */
#if defined(PINKTRACE_LINUX)
#if defined(IA64)
#ifdef HAVE_STRUCT_IA64_FPREG
#define ia64_fpreg XXX_ia64_fpreg
#endif /* HAVE_STRUCT_IA64_FPREG */
#ifdef HAVE_STRUCT_PT_ALL_USER_REGS
#define pt_all_user_regs XXX_pt_all_user_regs
#endif /* HAVE_STRUCT_PT_ALL_USER_REGS */
#endif /* defined(IA64) */
#include <linux/ptrace.h>
#if defined(IA64)
#undef ia64_fpreg
#undef pt_all_user_regs
#endif /* defined(IA64) */
#endif /* defined(PINKTRACE_LINUX) */

#define ADDR_MUL	((64 == __WORDSIZE) ? 8 : 4)

#include <pinktrace/bitness.h>
#include <pinktrace/socket.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool
pink_internal_decode_socket_address(pid_t pid, long addr, long addrlen, pink_socket_address_t *paddr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !PINKTRACE_GUARD_INTERNAL_H */
