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

#ifndef PINKTRACE_GUARD_SOCKET_H
#define PINKTRACE_GUARD_SOCKET_H 1

/**
 * \file
 * \brief Pink's socket related data structures and functions
 *
 * \ingroup g_socket
 **/

#if defined(PINKTRACE_FREEBSD) || defined(DOXYGEN)
#include <sys/socket.h>
#endif

#include <netinet/in.h>
#include <sys/un.h>

#if PINKTRACE_HAVE_NETLINK || defined(DOXYGEN)
#include <linux/netlink.h>
#endif /* PINKTRACE_HAVE_NETLINK... */

#include <pinktrace/gcc.h>

/**
 * \brief Structure which defines a decoded socket address.
 *
 * \ingroup g_socket
 **/
typedef struct {
	/** Family of the socket address **/
	int family;

	/**
	 * Length of the socket address
	 *
	 * \since 0.0.5
	 **/
	socklen_t length;

	/**
	 * This union contains type-safe pointers to the real socket address.
	 * Check the family before attempting to obtain the real object.
	 **/
	union {
		/** Padding, for internal use only **/
		char _pad[128];

		/** Socket address, for internal use only **/
		struct sockaddr _sa;

		/** Unix socket address, only valid if family is AF_UNIX. **/
		struct sockaddr_un sa_un;

		/** Inet socket address, only valid if family is AF_INET. **/
		struct sockaddr_in sa_in;

#if PINKTRACE_HAVE_IPV6 || defined(DOXYGEN)
		/**
		 * Inet6 socket address, only valid if family is AF_INET6.
		 * This member is only available if IPV6 support was enabled at
		 * compile time. Check with PINKTRACE_HAVE_IPV6.
		 **/
		struct sockaddr_in6 sa6;
#endif /* PINKTRACE_HAVE_IPV6... */

#if PINKTRACE_HAVE_NETLINK || defined(DOXYGEN)
		/**
		 * Netlink socket address, only valid if family is AF_NETLINK.
		 * Check with PINKTRACE_HAVE_NETLINK if this member is
		 * available.
		 *
		 * \since 0.0.5
		 **/
		struct sockaddr_nl nl;
#endif /* PINKTRACE_HAVE_NETLINK... */
	} u;
} pink_socket_address_t;

#if defined(PINKTRACE_LINUX) || defined(DOXYGEN)
/**
 * Decoded socket subcalls
 *
 * \ingroup g_socket
 *
 * \note Availability: Linux
 **/
typedef enum {
	/** socket() subcall **/
	PINK_SOCKET_SUBCALL_SOCKET = 1,
	/** bind() subcall **/
	PINK_SOCKET_SUBCALL_BIND,
	/** connect() subcall **/
	PINK_SOCKET_SUBCALL_CONNECT,
	/** listen() subcall **/
	PINK_SOCKET_SUBCALL_LISTEN,
	/** accept() subcall **/
	PINK_SOCKET_SUBCALL_ACCEPT,
	/** getsockname() subcall **/
	PINK_SOCKET_SUBCALL_GETSOCKNAME,
	/** getpeername() subcall **/
	PINK_SOCKET_SUBCALL_GETPEERNAME,
	/** socketpair() subcall **/
	PINK_SOCKET_SUBCALL_SOCKETPAIR,
	/** send() subcall **/
	PINK_SOCKET_SUBCALL_SEND,
	/** recv() subcall **/
	PINK_SOCKET_SUBCALL_RECV,
	/** sendto() subcall **/
	PINK_SOCKET_SUBCALL_SENDTO,
	/** recvfrom() subcall **/
	PINK_SOCKET_SUBCALL_RECVFROM,
	/** shutdown() subcall **/
	PINK_SOCKET_SUBCALL_SHUTDOWN,
	/** setsockopt() subcall **/
	PINK_SOCKET_SUBCALL_SETSOCKOPT,
	/** getsockopt() subcall **/
	PINK_SOCKET_SUBCALL_GETSOCKOPT,
	/** sendmsg() subcall **/
	PINK_SOCKET_SUBCALL_SENDMSG,
	/** recvmsg() subcall **/
	PINK_SOCKET_SUBCALL_RECVMSG,
	/** accept4() subcall **/
	PINK_SOCKET_SUBCALL_ACCEPT4,
} pink_socket_subcall_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Decide whether the socketcall() system call is defined for the given
 * bitness. On some architectures the socket system calls - socket, connect,
 * bind etc. - are implemented as separate system calls. On others these calls
 * are subcalls of the socketcall() system call.
 *
 * \ingroup g_socket
 *
 * \note Availability: Linux
 *
 * \see pink_socket_subcall_t
 *
 * \param bitness Bitness of the child
 * \return true if socketcall() system call is available, false otherwise.
 **/
bool
pink_has_socketcall(pink_bitness_t bitness) PINK_GCC_ATTR((pure));

/**
 * Name the given socket subcall
 *
 * \ingroup g_socket
 *
 * \note Availability: Linux
 *
 * \param subcall The socket subcall
 *
 * \return The name of the subcall
 **/
const char *
pink_name_socket_subcall(pink_socket_subcall_t subcall) PINK_GCC_ATTR((pure));

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* defined(PINKTRACE_LINUX)... */

#endif /* !PINKTRACE_GUARD_SOCKET_H */
