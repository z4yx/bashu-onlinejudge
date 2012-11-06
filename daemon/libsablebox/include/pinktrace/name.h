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

#ifndef PINKTRACE_GUARD_NAME_H
#define PINKTRACE_GUARD_NAME_H 1

/**
 * \file
 * \brief Pink's system call naming
 *
 * \ingroup g_name
 **/

#include <pinktrace/bitness.h>
#include <pinktrace/gcc.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Return the name of the given system call.
 *
 * \ingroup g_name
 *
 * \note On ARM architecture, this function considers the system call
 * architecture specific if scno is smaller than zero.
 *
 * \see pink_util_get_syscall
 *
 * \param scno System call number
 * \param bitness Bitness of the child
 *
 * \return The name of the system call, NULL if system call name is unknown.
 **/
const char *
pink_name_syscall(long scno, pink_bitness_t bitness) PINK_GCC_ATTR((pure));

/**
 * Look up the number of the given system call name.
 *
 * \ingroup g_name
 *
 * \param name Name of the system call
 * \param bitness Bitness of the child
 *
 * \return The system call number on success, -1 on failure.
 **/
long
pink_name_lookup(const char *name, pink_bitness_t bitness) PINK_GCC_ATTR((pure));

/**
 * Look up the number of the given system call name.
 *
 * \ingroup g_name
 *
 * \param name Name of the system call
 * \param length Length of the name
 * \param bitness Bitness of the child
 *
 * \return The system call number on success, -1 on failure.
 **/
long
pink_name_lookup_with_length(const char *name, size_t length, pink_bitness_t bitness) PINK_GCC_ATTR((pure));

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !PINKTRACE_GUARD_NAME_H */
