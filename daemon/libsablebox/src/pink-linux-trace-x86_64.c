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

#include <assert.h>
#include <stdlib.h>

#include <pinktrace/internal.h>
#include <pinktrace/pink.h>

#define ORIG_ACCUM	(8 * ORIG_RAX)
#define ACCUM		(8 * RAX)
static const long syscall_args[2][PINK_MAX_INDEX] = {
	{8 * RBX, 8 * RCX, 8 * RDX, 8 * RSI, 8 * RDI, 8 * RBP},
	{8 * RDI, 8 * RSI, 8 * RDX, 8 * R10, 8 * R8, 8 * R9}
};

pink_bitness_t
pink_bitness_get(pid_t pid)
{
	long cs;

	/*
	 * Check CS register value,
	 * On x86-64 linux this is:
	 * 0x33    for long mode (64 bit)
	 * 0x23    for compatibility mode (32 bit)
	 */

	if (PINK_GCC_UNLIKELY(!pink_util_peek(pid, 8 * CS, &cs)))
		return PINK_BITNESS_UNKNOWN;

	switch (cs) {
	case 0x33:
		return PINK_BITNESS_64;
	case 0x23:
		return PINK_BITNESS_32;
	default:
		return PINK_BITNESS_UNKNOWN;
	}
}

inline
unsigned short
pink_bitness_wordsize(pink_bitness_t bitness)
{
	assert(bitness == PINK_BITNESS_32 || bitness == PINK_BITNESS_64);

	switch (bitness) {
	case PINK_BITNESS_32:
		return 4;
	case PINK_BITNESS_64:
		return 8;
	default:
		abort();
	}
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
	assert(res != NULL);

	return pink_util_peek(pid, ACCUM, res);
}

bool
pink_util_set_return(pid_t pid, long ret)
{
	return pink_util_poke(pid, ACCUM, ret);
}

bool
pink_util_get_arg(pid_t pid, pink_bitness_t bitness, unsigned ind, long *res)
{
	assert(bitness == PINK_BITNESS_32 || bitness == PINK_BITNESS_64);
	assert(ind < PINK_MAX_INDEX);

	return pink_util_peek(pid, syscall_args[bitness][ind], res);
}

bool
pink_util_set_arg(pid_t pid, pink_bitness_t bitness, unsigned ind, long arg)
{
	assert(bitness == PINK_BITNESS_32 || bitness == PINK_BITNESS_64);
	assert(ind < PINK_MAX_INDEX);

	return pink_util_poke(pid, syscall_args[bitness][ind], arg);
}

bool
pink_decode_simple(pid_t pid, pink_bitness_t bitness, unsigned ind, void *dest, size_t len)
{
	long addr;

	assert(bitness == PINK_BITNESS_32 || bitness == PINK_BITNESS_64);
	assert(ind < PINK_MAX_INDEX);
	assert(dest != NULL);

	return pink_util_get_arg(pid, bitness, ind, &addr) && pink_util_moven(pid, addr, dest, len);
}

bool
pink_decode_string(pid_t pid, pink_bitness_t bitness, unsigned ind, char *dest, size_t len)
{
	long addr;

	assert(bitness == PINK_BITNESS_32 || bitness == PINK_BITNESS_64);
	assert(ind < PINK_MAX_INDEX);
	assert(dest != NULL);

	return pink_util_get_arg(pid, bitness, ind, &addr) && pink_util_movestr(pid, addr, dest, len);
}

char *
pink_decode_string_persistent(pid_t pid, pink_bitness_t bitness, unsigned ind)
{
	long addr;

	assert(bitness == PINK_BITNESS_32 || bitness == PINK_BITNESS_64);
	assert(ind < PINK_MAX_INDEX);

	if (PINK_GCC_UNLIKELY(!pink_util_get_arg(pid, bitness, ind, &addr)))
		return NULL;

	return pink_util_movestr_persistent(pid, addr);
}

bool
pink_encode_simple(pid_t pid, pink_bitness_t bitness, unsigned ind, const void *src, size_t len)
{
	long addr;

	assert(bitness == PINK_BITNESS_32 || bitness == PINK_BITNESS_64);
	assert(ind < PINK_MAX_INDEX);

	return pink_util_get_arg(pid, bitness, ind, &addr) && pink_util_putn(pid, addr, src, len);
}

bool
pink_encode_simple_safe(pid_t pid, pink_bitness_t bitness, unsigned ind, const void *src, size_t len)
{
	long addr;

	assert(bitness == PINK_BITNESS_32 || bitness == PINK_BITNESS_64);
	assert(ind < PINK_MAX_INDEX);

	return pink_util_get_arg(pid, bitness, ind, &addr) && pink_util_putn_safe(pid, addr, src, len);
}

bool
pink_has_socketcall(pink_bitness_t bitness)
{
	assert(bitness == PINK_BITNESS_32 || bitness == PINK_BITNESS_64);

	switch (bitness) {
	case PINK_BITNESS_32:
		return true;
	case PINK_BITNESS_64:
		return false;
	case PINK_BITNESS_UNKNOWN:
	default:
		abort();
	}
}

bool
pink_decode_socket_call(pid_t pid, pink_bitness_t bitness, long *subcall)
{
	assert(bitness == PINK_BITNESS_32 || bitness == PINK_BITNESS_64);
	assert(subcall != NULL);

	switch (bitness) {
	case PINK_BITNESS_32:
		/* Decode socketcall(2) */
		return pink_util_get_arg(pid, PINK_BITNESS_32, 0, subcall);
	case PINK_BITNESS_64:
		return pink_util_get_syscall(pid, PINK_BITNESS_64, subcall);
	case PINK_BITNESS_UNKNOWN:
	default:
		abort();
	}
}

bool
pink_decode_socket_fd(pid_t pid, pink_bitness_t bitness, unsigned ind, long *fd)
{
	long args;

	assert(bitness == PINK_BITNESS_32 || bitness == PINK_BITNESS_64);
	assert(ind < PINK_MAX_INDEX);
	assert(fd != NULL);

	switch (bitness) {
	case PINK_BITNESS_32:
		/* Decode socketcall(2) */
		if (PINK_GCC_UNLIKELY(!pink_util_get_arg(pid, PINK_BITNESS_32, 1, &args)))
			return false;
		args += ind * sizeof(unsigned int);
		return pink_util_move(pid, args, fd);
	case PINK_BITNESS_64:
		return pink_util_get_arg(pid, PINK_BITNESS_64, ind, fd);
	case PINK_BITNESS_UNKNOWN:
	default:
		abort();
	}
}

bool
pink_decode_socket_address(pid_t pid, pink_bitness_t bitness, unsigned ind, long *fd, pink_socket_address_t *paddr)
{
	unsigned int iaddr, iaddrlen;
	long addr, addrlen, args;

	assert(bitness == PINK_BITNESS_32 || bitness == PINK_BITNESS_64);
	assert(ind < PINK_MAX_INDEX);
	assert(paddr != NULL);

	switch (bitness) {
	case PINK_BITNESS_32:
		/* Decode socketcall(2) */
		if (PINK_GCC_UNLIKELY(!pink_util_get_arg(pid, PINK_BITNESS_32, 1, &args)))
			return false;
		if (PINK_GCC_UNLIKELY(fd && !pink_util_move(pid, args, fd)))
			return false;
		args += ind * sizeof(unsigned int);
		if (PINK_GCC_UNLIKELY(!pink_util_move(pid, args, &iaddr)))
			return false;
		args += sizeof(unsigned int);
		if (PINK_GCC_UNLIKELY(!pink_util_move(pid, args, &iaddrlen)))
			return false;
		addr = iaddr;
		addrlen = iaddrlen;
		break;
	case PINK_BITNESS_64:
		if (PINK_GCC_UNLIKELY(fd && !pink_util_get_arg(pid, PINK_BITNESS_64, 0, fd)))
			return false;
		if (PINK_GCC_UNLIKELY(!pink_util_get_arg(pid, PINK_BITNESS_64, ind, &addr)))
			return false;
		if (PINK_GCC_UNLIKELY(!pink_util_get_arg(pid, PINK_BITNESS_64, ind + 1, &addrlen)))
			return false;
		break;
	case PINK_BITNESS_UNKNOWN:
	default:
		abort();
	}

	return pink_internal_decode_socket_address(pid, addr, addrlen, paddr);
}
