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
#include <stdlib.h>

#include <pinktrace/internal.h>
#include <pinktrace/pink.h>

bool
pink_decode_string_array_member(pid_t pid, pink_bitness_t bitness, long arg, unsigned ind, char *dest, size_t len, bool *nil)
{
	unsigned short wordsize;
	union {
		unsigned int p32;
		unsigned long p64;
		char data[sizeof(long)];
	} cp;

	wordsize = pink_bitness_wordsize(bitness);
	arg += ind * wordsize;

	if (PINK_GCC_UNLIKELY(!pink_util_moven(pid, arg, cp.data, wordsize)))
		return false;
	if (bitness == PINK_BITNESS_32)
		cp.p64 = cp.p32;
	if (cp.p64 == 0) {
		/* hit NULL, end of the array */
		if (nil)
			*nil = true;
		return true;
	}
	if (nil)
		*nil = false;
	return pink_util_movestr(pid, cp.p64, dest, len);
}

char *
pink_decode_string_array_member_persistent(pid_t pid, pink_bitness_t bitness, long arg, unsigned ind)
{
	int save_errno;
	unsigned short wordsize;
	union {
		unsigned int p32;
		unsigned long p64;
		char data[sizeof(long)];
	} cp;

	save_errno = errno;
	wordsize = pink_bitness_wordsize(bitness);
	arg += ind * wordsize;

	if (PINK_GCC_UNLIKELY(!pink_util_moven(pid, arg, cp.data, wordsize)))
		return NULL;
	if (bitness == PINK_BITNESS_32)
		cp.p64 = cp.p32;
	if (cp.p64 == 0) {
		/* hit NULL, end of the array */
		errno = save_errno;
		return NULL;
	}
	return pink_util_movestr_persistent(pid, cp.p64);
}
