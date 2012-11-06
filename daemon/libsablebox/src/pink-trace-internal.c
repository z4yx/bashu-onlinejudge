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

#include <string.h>

#include <pinktrace/internal.h>
#include <pinktrace/pink.h>

/**
 * Internal tracing/decoding functions common among architectures.
 **/

bool
pink_internal_decode_socket_address(pid_t pid, long addr, long addrlen, pink_socket_address_t *paddr)
{
	if (addr == 0) {
		/* NULL */
		paddr->family = -1;
		paddr->length = 0;
		return true;
	}
	if (addrlen < 2 || (unsigned long)addrlen > sizeof(paddr->u))
		addrlen = sizeof(paddr->u);

	memset(&paddr->u, 0, sizeof(paddr->u));
	if (PINK_GCC_UNLIKELY(!pink_util_moven(pid, addr, paddr->u._pad, addrlen)))
		return false;
	paddr->u._pad[sizeof(paddr->u._pad) - 1] = '\0';

	paddr->family = paddr->u._sa.sa_family;
	paddr->length = addrlen;
	return true;
}
