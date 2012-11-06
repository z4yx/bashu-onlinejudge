/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet cin fdm=syntax : */

/*
 * Copyright (c) 2010, 2011 Ali Polatel <alip@exherbo.org>
 * Based in part upon strace which is:
 *   Copyright (c) 1991, 1992 Paul Kranenburg <pk@cs.few.eur.nl>
 *   Copyright (c) 1993 Branko Lankester <branko@hacktic.nl>
 *   Copyright (c) 1993, 1994, 1995, 1996 Rick Sladkey <jrs@world.std.com>
 *   Copyright (c) 1996-1999 Wichert Akkerman <wichert@cistron.nl>
 *   Copyright (c) 1999 IBM Deutschland Entwicklung GmbH, IBM Corporation
 *                       Linux for s390 port by D.J. Barrow
 *                      <barrow_dj@mail.yahoo.com,djbarrow@de.ibm.com>
 *   Copyright (c) 2000 PocketPenguins Inc.  Linux for Hitachi SuperH
 *                      port by Greg Banks <gbanks@pocketpenguins.com>
 *
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

#include <assert.h>
#include <errno.h>
#include <stdbool.h>

#include <pinktrace/internal.h>
#include <pinktrace/pink.h>

#define OFFSET_R0 0
#define OFFSET_R7 28
#define OFFSET_IP 48
#define OFFSET_SP 52
#define OFFSET_PC 60

#ifndef PTRACE_SET_SYSCALL
#define PTRACE_SET_SYSCALL 23
#endif /* !PTRACE_SET_SYSCALL */

pink_bitness_t
pink_bitness_get(PINK_GCC_ATTR((unused)) pid_t pid)
{
	return PINK_BITNESS_32;
}

inline
unsigned short
pink_bitness_wordsize(PINK_GCC_ATTR((unused)) pink_bitness_t bitness)
{
	return 4;
}

bool
pink_util_get_syscall(pid_t pid, PINK_GCC_ATTR((unused)) pink_bitness_t bitness, long *res)
{
	long pc, swi, scno;

	assert(res != NULL);

	if (!pink_util_peek(pid, OFFSET_PC, &pc)
			|| !pink_util_peekdata(pid, pc - sizeof(long), &swi))
		return false;

	if (swi == 0xef000000 || swi == 0x0f000000) {
		/* EABI system call */
		if (!pink_util_peek(pid, OFFSET_R7, &scno))
			return false;
	}
	else if ((swi & 0xfff00000) == 0xef900000) {
		/* old ABI system call */
		scno = swi & 0xfffff;
	}
	else {
		errno = ENOTSUP;
		return false;
	}

	if ((scno & 0xf0000) == 0xf0000) {
		/* Architecture specific system call
		 * Hack: Negate the system call number, so the user can
		 * distinguish between normal system calls and architecture
		 * specific system calls.
		 */
		scno &= ~0xf0000;
		scno = -scno;
	}

	*res = scno;
	return true;
}

bool
pink_util_set_syscall(pid_t pid, PINK_GCC_ATTR((unused)) pink_bitness_t bitness, long scno)
{
	return (0 == ptrace(PTRACE_SET_SYSCALL, pid, 0, scno & 0xffff));
}

bool
pink_util_get_return(pid_t pid, long *res)
{
	return pink_util_peek(pid, OFFSET_R0, res);
}

bool
pink_util_set_return(pid_t pid, long ret)
{
	return pink_util_poke(pid, OFFSET_R0, ret);
}

bool
pink_util_get_arg(pid_t pid, PINK_GCC_ATTR((unused)) pink_bitness_t bitness, unsigned ind, long *res)
{
	long sp;

	assert(ind < PINK_MAX_INDEX);
	assert(res != NULL);

	if (ind < 5)
		return pink_util_peek(pid, ind * sizeof(long), res);

	return pink_util_peek(pid, OFFSET_SP, &sp) && pink_util_peekdata(pid, sp + sizeof(long) * ind, res);
}

bool
pink_util_set_arg(pid_t pid, PINK_GCC_ATTR((unused)) pink_bitness_t bitness, unsigned ind, long arg)
{
	long sp;

	assert(ind < PINK_MAX_INDEX);

	if (ind < 5)
		return pink_util_poke(pid, ind * sizeof(long), arg);

	return pink_util_peek(pid, OFFSET_SP, &sp) && pink_util_pokedata(pid, sp + sizeof(long) * ind, arg);
}

bool
pink_decode_simple(pid_t pid, pink_bitness_t bitness, unsigned ind, void *dest, size_t len)
{
	long addr;

	return pink_util_get_arg(pid, bitness, ind, &addr) && pink_util_moven(pid, addr, dest, len);
}

bool
pink_decode_string(pid_t pid, pink_bitness_t bitness, unsigned ind, char *dest, size_t len)
{
	long addr;

	return pink_util_get_arg(pid, bitness, ind, &addr) && pink_util_movestr(pid, addr, dest, len);
}

char *
pink_decode_string_persistent(pid_t pid, pink_bitness_t bitness, unsigned ind)
{
	long addr;

	if (PINK_GCC_UNLIKELY(!pink_util_get_arg(pid, bitness, ind, &addr)))
		return NULL;

	return pink_util_movestr_persistent(pid, addr);
}

bool
pink_encode_simple(pid_t pid, pink_bitness_t bitness, unsigned ind, const void *src, size_t len)
{
	long addr;

	return pink_util_get_arg(pid, bitness, ind, &addr) && pink_util_putn(pid, addr, src, len);
}

bool
pink_encode_simple_safe(pid_t pid, pink_bitness_t bitness, unsigned ind, const void *src, size_t len)
{
	long addr;

	return pink_util_get_arg(pid, bitness, ind, &addr) && pink_util_putn_safe(pid, addr, src, len);
}

bool
pink_has_socketcall(PINK_GCC_ATTR((unused)) pink_bitness_t bitness)
{
	return false;
}

bool
pink_decode_socket_call(pid_t pid, pink_bitness_t bitness, long *subcall)
{
	/* No decoding needed */
	return pink_util_get_syscall(pid, bitness, subcall);
}

bool
pink_decode_socket_fd(pid_t pid, pink_bitness_t bitness, unsigned ind, long *fd)
{
	/* No decoding needed */
	return pink_util_get_arg(pid, bitness, ind, fd);
}

bool
pink_decode_socket_address(pid_t pid, pink_bitness_t bitness, unsigned ind, long *fd, pink_socket_address_t *paddr)
{
	long addr, addrlen;

	assert(paddr != NULL);

	/* No decoding needed */
	if (PINK_GCC_UNLIKELY(fd && !pink_util_get_arg(pid, bitness, 0, fd)))
		return false;
	if (PINK_GCC_UNLIKELY(!pink_util_get_arg(pid, bitness, ind, &addr)))
		return false;
	if (PINK_GCC_UNLIKELY(!pink_util_get_arg(pid, bitness, ind + 1, &addrlen)))
		return false;

	return pink_internal_decode_socket_address(pid, addr, addrlen, paddr);
}
