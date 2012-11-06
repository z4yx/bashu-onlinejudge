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
#include <stdlib.h>

#include <pinktrace/internal.h>
#include <pinktrace/pink.h>

#ifndef PT_ORIG_R3
#define PT_ORIG_R3 34
#endif /* !PT_ORIG_R3 */

#define ORIG_ACCUM	(sizeof(unsigned long) * PT_R0)
#define ACCUM		(sizeof(unsigned long) * PT_R3)
#define ACCUM_FLAGS	(sizeof(unsigned long) * PT_CCR)
#define SO_MASK		0x10000000

#define ARG_OFFSET(i)	(((i) == 0)				\
		? (sizeof(unsigned long) * PT_ORIG_R3)		\
		: (sizeof(unsigned long) * ((i) + PT_R3)))

pink_bitness_t
pink_bitness_get(PINK_GCC_ATTR((unused)) pid_t pid)
{
#if defined(POWERPC)
	return PINK_BITNESS_32;
#elif defined(POWERPC64)
	return PINK_BITNESS_64;
#else
#error unsupported architecture
#endif
}

inline
unsigned short
pink_bitness_wordsize(PINK_GCC_ATTR((unused)) pink_bitness_t bitness)
{
#if defined(POWERPC)
	return sizeof(int);
#elif defined(POWERPC64)
	return sizeof(long);
#else
#error unsupported architecture
#endif
}

bool
pink_util_get_syscall(pid_t pid, PINK_GCC_ATTR((unused)) pink_bitness_t bitness, long *res)
{
	return pink_util_peek(pid, ORIG_ACCUM, res);
}

bool
pink_util_set_syscall(pid_t pid, PINK_GCC_ATTR((unused)) pink_bitness_t bitness, long scno)
{
	return pink_util_poke(pid, ORIG_ACCUM, scno);
}

bool
pink_util_get_return(pid_t pid, long *res)
{
	long flags;

	assert(res != NULL);

	if (PINK_GCC_UNLIKELY(!pink_util_peek(pid, ACCUM, res)
				|| !pink_util_peek(pid, ACCUM_FLAGS, &flags)))
		return false;

	if (PINK_GCC_UNLIKELY(flags & SO_MASK))
		*res = -(*res);

	return true;
}

bool
pink_util_set_return(pid_t pid, long ret)
{
	long flags;

	if (PINK_GCC_UNLIKELY(!pink_util_peek(pid, ACCUM_FLAGS, &flags)))
		return false;

	if (ret < 0) {
		flags |= SO_MASK;
		ret = -ret;
	}
	else
		flags &= ~SO_MASK;

	return pink_util_poke(pid, ACCUM, ret) && pink_util_poke(pid, ACCUM_FLAGS, flags);
}

bool
pink_util_get_arg(pid_t pid, PINK_GCC_ATTR((unused)) pink_bitness_t bitness, unsigned ind, long *res)
{
	assert(ind < PINK_MAX_INDEX);

	return pink_util_peek(pid, ARG_OFFSET(ind), res);
}

bool
pink_util_set_arg(pid_t pid, PINK_GCC_ATTR((unused)) pink_bitness_t bitness, unsigned ind, long arg)
{
	assert(ind < PINK_MAX_INDEX);

	return pink_util_poke(pid, ARG_OFFSET(ind), arg);
}

bool
pink_decode_simple(pid_t pid, pink_bitness_t bitness, unsigned ind, void *dest, size_t len)
{
	long addr;

	assert(ind < PINK_MAX_INDEX);
	assert(dest != NULL);

	return pink_util_get_arg(pid, bitness, ind, &addr) && pink_util_moven(pid, addr, dest, len);
}

bool
pink_decode_string(pid_t pid, pink_bitness_t bitness, unsigned ind, char *dest, size_t len)
{
	long addr;

	assert(ind < PINK_MAX_INDEX);
	assert(dest != NULL);

	return pink_util_get_arg(pid, bitness, ind, &addr) && pink_util_movestr(pid, addr, dest, len);
}

char *
pink_decode_string_persistent(pid_t pid, pink_bitness_t bitness, unsigned ind)
{
	long addr;

	assert(ind < PINK_MAX_INDEX);

	if (PINK_GCC_UNLIKELY(!pink_util_get_arg(pid, bitness, ind, &addr)))
		return false;

	return pink_util_movestr_persistent(pid, addr);
}

bool
pink_encode_simple(pid_t pid, pink_bitness_t bitness, unsigned ind, const void *src, size_t len)
{
	long addr;

	assert(ind < PINK_MAX_INDEX);

	return pink_util_get_arg(pid, bitness, ind, &addr) && pink_util_putn(pid, addr, src, len);
}

bool
pink_encode_simple_safe(pid_t pid, pink_bitness_t bitness, unsigned ind, const void *src, size_t len)
{
	long addr;

	assert(ind < PINK_MAX_INDEX);

	return pink_util_get_arg(pid, bitness, ind, &addr) && pink_util_putn_safe(pid, addr, src, len);
}

bool
pink_has_socketcall(PINK_GCC_ATTR((unused)) pink_bitness_t bitness)
{
	return true;
}

bool
pink_decode_socket_call(pid_t pid, pink_bitness_t bitness, long *subcall)
{
	assert(subcall != NULL);

	/* Decode socketcall(2) */
	return pink_util_get_arg(pid, bitness, 0, subcall);
}

bool
pink_decode_socket_fd(pid_t pid, pink_bitness_t bitness, unsigned ind, long *fd)
{
	unsigned short size;
	long args;

	assert(ind < PINK_MAX_INDEX);

	/* Decode socketcall(2) */
	if (PINK_GCC_UNLIKELY(!pink_util_get_arg(pid, bitness, 1, &args)))
		return false;

	size = pink_bitness_wordsize(bitness);
	switch (size) {
	case sizeof(int):
		args += ind * sizeof(unsigned int);
		break;
	case sizeof(long):
		args += ind * sizeof(unsigned long);
		break;
	default:
		abort();
	}

	return pink_util_move(pid, args, fd);

}

bool
pink_decode_socket_address(pid_t pid, pink_bitness_t bitness, unsigned ind, long *fd, pink_socket_address_t *paddr)
{
	unsigned short size;
	long addr, addrlen, args;

	assert(ind < PINK_MAX_INDEX);
	assert(paddr != NULL);

	/* Decode socketcall(2) */
	if (PINK_GCC_UNLIKELY(!pink_util_get_arg(pid, bitness, 1, &args)))
		return false;
	if (fd && PINK_GCC_UNLIKELY(!pink_util_move(pid, args, fd)))
		return false;

	size = pink_bitness_wordsize(bitness);
	if (size == sizeof(int)) {
		unsigned int iaddr, iaddrlen;
		args += ind * size;
		if (PINK_GCC_UNLIKELY(!pink_util_move(pid, args, &iaddr)))
			return false;
		args += size;
		if (PINK_GCC_UNLIKELY(!pink_util_move(pid, args, &iaddrlen)))
			return false;

		addr = iaddr;
		addrlen = iaddrlen;
	}
	else if (size == sizeof(long)) {
		unsigned long laddr, laddrlen;
		args += ind * size;
		if (PINK_GCC_UNLIKELY(!pink_util_move(pid, args, &laddr)))
			return false;
		args += size;
		if (PINK_GCC_UNLIKELY(!pink_util_move(pid, args, &laddrlen)))
			return false;

		addr = laddr;
		addrlen = laddrlen;
	}
	else
		abort();

	return pink_internal_decode_socket_address(pid, addr, addrlen, paddr);
}
