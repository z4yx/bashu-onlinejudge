/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet cin fdm=syntax : */

/*
 * Copyright (c) 2010, 2011 Ali Polatel <alip@exherbo.org>
 * Based in part upon truss which is:
 *   Copyright (c) 1997 Sean Eric Fagan
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
#include <stdbool.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include <pinktrace/internal.h>
#include <pinktrace/pink.h>

inline
static unsigned
pink_util_arg_offset_i386(struct reg r, unsigned ind)
{
	long off, scno;

	/*
	 * FreeBSD has two special kinds of system call redirections --
	 * SYS_syscall, and SYS___syscall.  The former is the old syscall()
	 * routine, basicly; the latter is for quad-aligned arguments.
	 */
	off = r.r_esp + sizeof(int);
	scno = r.r_eax;
	switch (scno) {
	case SYS_syscall:
		off += sizeof(int);
		break;
	case SYS___syscall:
		off += sizeof(quad_t);
		break;
	default:
		break;
	}

	return off + ind * sizeof(int);
}

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
	unsigned parm_offset;
	struct reg r;

	if (PINK_GCC_UNLIKELY(!pink_util_get_regs(pid, &r)))
		return false;

	/*
	 * FreeBSD has two special kinds of system call redirections --
	 * SYS_syscall, and SYS___syscall.  The former is the old syscall()
	 * routine, basicly; the latter is for quad-aligned arguments.
	 */
	*res = r.r_eax;
	switch (*res) {
	case SYS_syscall:
	case SYS___syscall:
		parm_offset = r.r_esp + sizeof(int);
		if (PINK_GCC_UNLIKELY(!pink_util_peekdata(pid, parm_offset, res)))
			return false;
		return true;
	default:
		return true;
	}
}

bool
pink_util_set_syscall(pid_t pid, PINK_GCC_ATTR((unused)) pink_bitness_t bitness, long scno)
{
	struct reg r;

	if (PINK_GCC_UNLIKELY(!pink_util_get_regs(pid, &r)))
		return false;

	r.r_eax = scno;

	return pink_util_set_regs(pid, &r)
}

bool
pink_util_get_return(pid_t pid, long *res)
{
	bool errorp;
	struct reg r;

	assert(res != NULL);

	if (PINK_GCC_UNLIKELY(!pink_util_get_regs(pid, &r)))
		return false;

	errorp = !!(r.r_eflags & PSL_C);
	*res = errorp ? -r.r_eax : r.r_eax;

	return true;
}

bool
pink_util_set_return(pid_t pid, long ret)
{
	struct reg r;

	if (PINK_GCC_UNLIKELY(!pink_util_get_regs(pid, &r)))
		return false;

	if (ret < 0) {
		r.r_eax = -ret;
		r.r_eflags |= PSL_C;
	}
	else
		r.r_eax = ret;

	return pink_util_set_regs(pid, &r);
}

bool
pink_util_get_arg(pid_t pid, PINK_GCC_ATTR((unused)) pink_bitness_t bitness, unsigned ind, long *res)
{
	struct reg r;

	assert(ind < PINK_MAX_INDEX);
	assert(res != NULL);

	return pink_util_get_regs(pid, &r) && pink_util_peekdata(pid, pink_util_arg_offset_i386(r, ind), res);
}

bool
pink_util_set_arg(pid_t pid, PINK_GCC_ATTR((unused)) pink_bitness_t bitness, unsigned ind, long arg)
{
	struct reg r;

	assert(ind < PINK_MAX_INDEX);

	return pink_util_get_regs(pid, &r) && pink_util_pokedata(pid, pink_util_arg_offset_i386(r, ind), arg);
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

	if (!pink_util_get_arg(pid, bitness, ind, &addr))
		return NULL;

	return pink_util_movestr_persistent(pid, addr);
}

bool
pink_decode_socket_address(pid_t pid, pink_bitness_t bitness, unsigned ind, long *fd, pink_socket_address_t *paddr)
{
	long addr;
	long addrlen;

	assert(ind < PINK_MAX_INDEX);
	assert(paddr != NULL);

	if (PINK_GCC_UNLIKELY(fd && !pink_util_get_arg(pid, bitness, 0, fd)))
		return false;

	if (PINK_GCC_UNLIKELY(!pink_util_get_arg(pid, bitness, ind, &addr)
				|| !pink_util_get_arg(pid, bitness, ind + 1, &addrlen)))
		return false;

	return pink_internal_decode_socket_address(pid, addr, addrlen, paddr);
}

bool
pink_encode_simple(pid_t pid, pink_bitness_t bitness, unsigned ind, const void *src, size_t len)
{
	long addr;

	assert(ind < PINK_MAX_INDEX);

	return pink_util_get_arg(pid, bitness, ind, &addr) && pink_util_putn(pid, addr, src, len);
}
