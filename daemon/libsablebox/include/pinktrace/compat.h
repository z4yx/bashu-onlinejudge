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

#ifndef PINKTRACE_GUARD_COMPAT_H
#define PINKTRACE_GUARD_COMPAT_H

/**
 * \file
 * \brief Pink's compatibility macros
 *
 * \ingroup g_compat
 **/

/**
 * Define for the availability of pink_bitness_wordsize() function.
 *
 * \ingroup g_compat
 *
 * \see pink_bitness_wordsize
 *
 * \since 0.0.3
 **/
#define PINK_BITNESS_WORDSIZE_AVAILABLE 1

/**
 * Define for the availability of pink_decode_string_array_member() function.
 *
 * \ingroup g_compat
 *
 * \see pink_decode_string_array_member
 *
 * \since 0.0.3
 **/
#define PINK_DECODE_STRING_ARRAY_MEMBER_AVAILABLE 1

/**
 * Define for the availability of pink_decode_string_array_member_persistent() function.
 *
 * \ingroup g_compat
 *
 * \see pink_decode_string_array_member_persistent
 *
 * \since 0.0.3
 **/
#define PINK_DECODE_STRING_ARRAY_MEMBER_PERSISTENT_AVAILABLE 1

/**
 * Define for the availability of pink_trace_sysemu() function.
 *
 * \ingroup g_compat
 *
 * \see pink_trace_sysemu()
 *
 * \since 0.0.5
 **/
#define PINK_TRACE_SYSEMU_AVAILABLE 1

/**
 * Define for the availability of pink_trace_sysemu_singlestep() function.
 *
 * \ingroup g_compat
 *
 * \see pink_trace_sysemu_singlestep()
 *
 * \since 0.0.5
 **/
#define PINK_TRACE_SYSEMU_SINGLESTEP_AVAILABLE 1

/**
 * Define for the availability of nl member (netlink) of #pink_socket_address_t
 *
 * \ingroup g_compat
 *
 * \see pink_socket_address_t
 *
 * \since 0.0.5
 **/
#define PINK_SOCKET_ADDRESS_T_NL_AVAILABLE 1

/**
 * Define for the availability of length member of #pink_socket_address_t
 *
 * \ingroup g_compat
 *
 * \see pink_socket_address_t
 *
 * \since 0.0.5
 **/
#define PINK_SOCKET_ADDRESS_T_LENGTH_AVAILABLE 1

/**
 * Define for the availability of pink_util_set_arg() function.
 *
 * \ingroup g_compat
 *
 * \see pink_util_set_arg()
 *
 * \since 0.0.5
 **/
#define PINK_UTIL_SET_ARG_AVAILABLE 1

/**
 * Define for the availability of #PINK_EVENT_TRAP constant.
 *
 * \see pink_event_decide()
 *
 * \since 0.0.6
 **/
#define PINK_EVENT_TRAP_AVAILABLE 1

/**
 * Define for the availability of pink_trace_lwpinfo() function.
 *
 * \see pink_trace_lwpinfo()
 *
 * \since 0.0.6
 **/
#define PINK_TRACE_LWPINFO_AVAILABLE 1

/**
 * Define for the availability of pink_trace_followfork() function.
 *
 * \see pink_trace_followfork()
 *
 * \since 0.0.6
 **/
#define PINK_TRACE_FOLLOWFORK_AVAILABLE 1

/**
 * Define for the availability of the pink_name_lookup_with_length() function.
 *
 * \see pink_name_lookup_with_length()
 *
 * \since 0.1.2
 **/
#define PINK_NAME_LOOKUP_WITH_LENGTH_AVAILABLE 1

#endif /* PINKTRACE_GUARD_COMPAT_H */
