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

#include <errno.h>
#include <stdlib.h>

#include <pinktrace/internal.h>
#include <pinktrace/pink.h>

bool
pink_util_peek(pid_t pid, long off, long *res)
{
	long val;

	errno = 0;
	val = ptrace(PT_READ_I, pid, (caddr_t)off, 0);
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
	val = ptrace(PT_READ_D, pid, (caddr_t)off, 0);
	if (PINK_GCC_UNLIKELY(val == -1 && errno != 0))
		return false;

	if (res)
		*res = val;

	return true;
}

bool
pink_util_poke(pid_t pid, long off, long val)
{
	return (0 == ptrace(PT_WRITE_I, pid, (caddr_t)off, val));
}

bool
pink_util_pokedata(pid_t pid, long off, long val)
{
	return (0 == ptrace(PT_WRITE_D, pid, (caddr_t)off, val));
}

bool
pink_util_get_regs(pid_t pid, void *regs)
{
	return !(ptrace(PT_GETREGS, pid, (caddr_t)regs, 0) < 0);
}

bool
pink_util_set_regs(pid_t pid, const void *regs)
{
	return !(ptrace(PT_SETREGS, pid, (caddr_t)regs, 0) < 0);
}

bool
pink_util_moven(pid_t pid, long addr, char *dest, size_t len)
{
	struct ptrace_io_desc ioreq;

	ioreq.piod_op = PIOD_READ_D;
	ioreq.piod_offs = (void *)addr;
	ioreq.piod_addr = dest;
	ioreq.piod_len = len;

	return !(ptrace(PT_IO, pid, (caddr_t)&ioreq, 0) < 0);
}

bool
pink_util_movestr(pid_t pid, long addr, char *dest, size_t len)
{
	return pink_util_moven(pid, addr, dest, len);
}

#define MAXSIZE 4096
#define BLOCKSIZE 1024

char *
pink_util_movestr_persistent(pid_t pid, long addr)
{
	int diff, save_errno;
	size_t totalsize, size;
	char *buf;

	save_errno = errno;
	diff = 0;
	totalsize = size = BLOCKSIZE;

	buf = malloc(sizeof(char) * totalsize);
	if (PINK_GCC_UNLIKELY(!buf))
		return NULL;

	for (;;) {
		diff = totalsize - size;
		if (PINK_GCC_UNLIKELY(!pink_util_moven(pid, addr + diff, buf + diff, size))) {
			if (diff == 0 && (errno == EFAULT || errno == EIO)) {
				/* NULL */
				free(buf);
				errno = save_errno;
			}
			else {
				save_errno = errno;
				free(buf);
				errno = save_errno;
			}
			return NULL;
		}
		for (unsigned int i = 0; i < size; i++) {
			if (buf[diff + i] == '\0')
				return buf;
		}
		if (totalsize < MAXSIZE - BLOCKSIZE) {
			totalsize += BLOCKSIZE;
			buf = realloc(buf, totalsize);
			if (PINK_GCC_UNLIKELY(!buf))
				return NULL;
			size = BLOCKSIZE;
		}
		else {
			buf[totalsize - 1] = '\0';
			return buf;
		}
	}
}

bool
pink_util_putn(pid_t pid, long addr, const char *src, size_t len)
{
	struct ptrace_io_desc ioreq;

	ioreq.piod_op = PIOD_WRITE_D;
	ioreq.piod_offs = (char *)addr;
	ioreq.piod_addr = src;
	ioreq.piod_len = len;

	return !(ptrace(PT_IO, pid, (caddr_t)&ioreq, 0) < 0);
}
