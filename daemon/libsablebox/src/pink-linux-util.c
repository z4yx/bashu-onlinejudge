/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet cin fdm=syntax : */

/*
 * Copyright (c) 2010 Ali Polatel <alip@exherbo.org>
 * Based in part upon strace which is:
 *   Copyright (c) 1991, 1992 Paul Kranenburg <pk@cs.few.eur.nl>
 *   Copyright (c) 1993 Branko Lankester <branko@hacktic.nl>
 *   Copyright (c) 1993, 1994, 1995, 1996 Rick Sladkey <jrs@world.std.com>
 *   Copyright (c) 1996-1999 Wichert Akkerman <wichert@cistron.nl>
 *   Copyright (c) 1999 IBM Deutschland Entwicklung GmbH, IBM Corporation
 *                      Linux for s390 port by D.J. Barrow
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
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <pinktrace/internal.h>
#include <pinktrace/pink.h>

bool
pink_util_peek(pid_t pid, long off, long *res)
{
	long val;

	errno = 0;
	val = ptrace(PTRACE_PEEKUSER, pid, off, NULL);
	if (PINK_GCC_UNLIKELY(val == -1 && errno != 0))
		return false;

	if (res)
		*res = val;

	return true;
}

bool
pink_util_peekdata(pid_t pid, long off, long *res)
{
	long val;

	errno = 0;
	val = ptrace(PTRACE_PEEKDATA, pid, off, NULL);
	if (PINK_GCC_UNLIKELY(val == -1 && errno != 0))
		return false;

	if (res)
		*res = val;

	return true;
}

bool
pink_util_poke(pid_t pid, long off, long val)
{
	return (0 == ptrace(PTRACE_POKEUSER, pid, off, val));
}

bool
pink_util_pokedata(pid_t pid, long off, long val)
{
	return (0 == ptrace(PTRACE_POKEDATA, pid, off, val));
}

bool
pink_util_get_regs(pid_t pid, void *regs)
{
	return !(ptrace(PTRACE_GETREGS, pid, NULL, regs) < 0);
}

bool
pink_util_set_regs(pid_t pid, const void *regs)
{
	return !(ptrace(PTRACE_SETREGS, pid, NULL, regs) < 0);
}

bool
pink_util_putn(pid_t pid, long addr, const char *src, size_t len)
{
	int n, m;
	union {
		long val;
		char x[sizeof(long)];
	} u;

	n = 0;
	m = len / sizeof(long);

	while (n < m) {
		memcpy(u.x, src, sizeof(long));
		if (PINK_GCC_UNLIKELY(!pink_util_pokedata(pid, addr + n * ADDR_MUL, u.val)))
			return false;
		++n;
		src += sizeof(long);
	}

	m = len % sizeof(long);
	if (m) {
		memcpy(u.x, src, m);
		if (PINK_GCC_UNLIKELY(!pink_util_pokedata(pid, addr + n * ADDR_MUL, u.val)))
			return false;
	}

	return true;
}

bool
pink_util_putn_safe(pid_t pid, long addr, const char *src, size_t len)
{
	int n, m;
	union {
		long val;
		char x[sizeof(long)];
	} u;

	n = 0;
	m = len / sizeof(long);

	while (n < m) {
		memcpy(u.x, src, sizeof(long));
		if (PINK_GCC_UNLIKELY(!pink_util_peekdata(pid, addr + n * ADDR_MUL, NULL)))
			return false;
		if (PINK_GCC_UNLIKELY(!pink_util_pokedata(pid, addr + n * ADDR_MUL, u.val)))
			return false;
		++n;
		src += sizeof(long);
	}

	m = len % sizeof(long);
	if (m) {
		memcpy(u.x, src, m);
		if (PINK_GCC_UNLIKELY(!pink_util_peekdata(pid, addr + n * ADDR_MUL, NULL)))
			return false;
		if (PINK_GCC_UNLIKELY(!pink_util_pokedata(pid, addr + n * ADDR_MUL, u.val)))
			return false;
	}

	return true;
}

#define MIN(a,b)	(((a) < (b)) ? (a) : (b))
bool
pink_util_moven(pid_t pid, long addr, char *dest, size_t len)
{
	int n, m;
	int started = 0;
	union {
		long val;
		char x[sizeof(long)];
	} u;

	if (addr & (sizeof(long) -1)) {
		/* addr not a multiple of sizeof(long) */
		n = addr - (addr & -sizeof(long)); /* residue */
		addr &= -sizeof(long); /* residue */

		if (PINK_GCC_UNLIKELY(!pink_util_peekdata(pid, addr, &u.val))) {
			if (PINK_GCC_LIKELY(started && (errno == EPERM || errno == EIO))) {
				/* Ran into end of memory */
				return true;
			}
			/* But if not started, we had a bogus address */
			return false;
		}
		started = 1;
		memcpy(dest, &u.x[n], m = MIN(sizeof(long) - n, len));
		addr += sizeof(long), dest += m, len -= m;
	}
	while (len > 0) {
		if (PINK_GCC_UNLIKELY(!pink_util_peekdata(pid, addr, &u.val))) {
			if (PINK_GCC_LIKELY(started && (errno == EPERM || errno == EIO))) {
				/* Ran into end of memory */
				return true;
			}
			/* But if not started, we had a bogus address */
			return false;
		}
		started = 1;
		memcpy(dest, u.x, m = MIN(sizeof(long), len));
		addr += sizeof(long), dest += m, len -= m;
	}
	return true;
}

bool
pink_util_movestr(pid_t pid, long addr, char *dest, size_t len)
{
	int n, m;
	int started = 0;
	union {
		long val;
		char x[sizeof(long)];
	} u;

	if (addr & (sizeof(long) -1)) {
		/* addr not a multiple of sizeof(long) */
		n = addr - (addr & -sizeof(long)); /* residue */
		addr &= -sizeof(long); /* residue */

		if (PINK_GCC_UNLIKELY(!pink_util_peekdata(pid, addr, &u.val))) {
			if (PINK_GCC_LIKELY(started && (errno == EPERM || errno == EIO || errno == EFAULT))) {
				/* Ran into end of memory */
				return true;
			}
			/* But if not started, we had a bogus address */
			return false;
		}
		started = 1;
		memcpy(dest, &u.x[n], m = MIN(sizeof(long) - n, len));
		while (n & (sizeof(long) - 1))
			if (u.x[n++] == '\0')
				return true;
		addr += sizeof(long), dest += m, len -= m;
	}
	while (len > 0) {
		if (PINK_GCC_UNLIKELY(!pink_util_peekdata(pid, addr, &u.val))) {
			if (PINK_GCC_LIKELY(started && (errno == EPERM || errno == EIO || errno == EFAULT))) {
				/* Ran into end of memory */
				return true;
			}
			/* But if not started, we had a bogus address */
			return false;
		}
		started = 1;
		memcpy(dest, u.x, m = MIN(sizeof(long), len));
		for (unsigned int i = 0; i < sizeof(long); i++)
			if (u.x[i] == '\0')
				return true;
		addr += sizeof(long), dest += m, len -= m;
	}
	return true;
}

char *
pink_util_movestr_persistent(pid_t pid, long addr)
{
	int n, m, sum, save_errno;
	bool started;
	union {
		long val;
		char x[sizeof(long)];
	} u;
	char *res, *res_ptr;

#define XFREE(x)			\
	do {				\
		if ((x)) {		\
			free((x));	\
		}			\
	} while (0)

	save_errno = errno;
	started = false;
	res = res_ptr = NULL;
	sum = 0;

	if (addr & (sizeof(long) -1)) {
		/* addr not a multiple of sizeof(long) */
		n = addr - (addr & -sizeof(long)); /* residue */
		addr &= -sizeof(long); /* residue */

		if (PINK_GCC_UNLIKELY(!pink_util_peekdata(pid, addr, &u.val))) {
			if (PINK_GCC_LIKELY(started && (errno == EPERM || errno == EIO || errno == EFAULT))) {
				/* Ran into end of memory */
				return res;
			}
			/* But if not started, we had a bogus address */
			XFREE(res);
			if (PINK_GCC_UNLIKELY(!started)) {
				/* NULL */
				errno = save_errno;
			}
			return NULL;
		}
		m = sizeof(long) - n;
		sum += m;
		if ((res = realloc(res, sum)) == NULL)
			return NULL;
		res_ptr = started ? res + (sum - m) : res;
		started = true;
		memcpy(res_ptr, &u.x[n], m);
		while (n & (sizeof(long) - 1))
			if (u.x[n++] == '\0')
				return res;
		addr += sizeof(long);
	}

	for (;;) {
		if (PINK_GCC_UNLIKELY(!pink_util_peekdata(pid, addr, &u.val))) {
			if (PINK_GCC_LIKELY(started && (errno == EPERM || errno == EIO || errno == EFAULT))) {
				/* Ran into end of memory */
				return res;
			}
			/* But if not started, we had a bogus address */
			XFREE(res);
			if (PINK_GCC_UNLIKELY(!started)) {
				/* NULL */
				errno = save_errno;
			}
			return NULL;
		}
		sum += sizeof(long);
		if ((res = realloc(res, sum)) == NULL)
			return NULL;
		res_ptr = started ? res + (sum - sizeof(long)) : res;
		started = true;
		memcpy(res_ptr, u.x, sizeof(long));
		for (unsigned int i = 0; i < sizeof(long); i++)
			if (u.x[i] == '\0')
				return res;
		addr += sizeof(long);
	}
	/* never reached */
	assert(false);
#undef XFREE
}
