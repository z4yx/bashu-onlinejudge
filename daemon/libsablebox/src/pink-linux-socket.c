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

#include <pinktrace/pink.h>

const char *
pink_name_socket_subcall(pink_socket_subcall_t subcall)
{
	switch (subcall) {
	case PINK_SOCKET_SUBCALL_SOCKET:
		return "socket";
	case PINK_SOCKET_SUBCALL_BIND:
		return "bind";
	case PINK_SOCKET_SUBCALL_CONNECT:
		return "connect";
	case PINK_SOCKET_SUBCALL_LISTEN:
		return "listen";
	case PINK_SOCKET_SUBCALL_ACCEPT:
		return "accept";
	case PINK_SOCKET_SUBCALL_GETSOCKNAME:
		return "getsockname";
	case PINK_SOCKET_SUBCALL_GETPEERNAME:
		return "getpeername";
	case PINK_SOCKET_SUBCALL_SOCKETPAIR:
		return "socketpair";
	case PINK_SOCKET_SUBCALL_SEND:
		return "send";
	case PINK_SOCKET_SUBCALL_RECV:
		return "recv";
	case PINK_SOCKET_SUBCALL_SENDTO:
		return "sendto";
	case PINK_SOCKET_SUBCALL_RECVFROM:
		return "recvfrom";
	case PINK_SOCKET_SUBCALL_SHUTDOWN:
		return "shutdown";
	case PINK_SOCKET_SUBCALL_SETSOCKOPT:
		return "setsockopt";
	case PINK_SOCKET_SUBCALL_GETSOCKOPT:
		return "getsockopt";
	case PINK_SOCKET_SUBCALL_SENDMSG:
		return "sendmsg";
	case PINK_SOCKET_SUBCALL_RECVMSG:
		return "recvmsg";
	case PINK_SOCKET_SUBCALL_ACCEPT4:
		return "accept4";
	default:
		return "unknown";
	}
}
