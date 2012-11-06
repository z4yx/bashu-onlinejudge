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

#ifndef PINKTRACE_GUARD_ENCODE_H
#define PINKTRACE_GUARD_ENCODE_H 1

/**
 * \file
 * \brief Pink's system call encoders
 *
 * \ingroup g_encode
 **/

#include <stdbool.h>
#include <sys/types.h>

#include <pinktrace/bitness.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Write the given data argument src to the address of the argument arg.
 *
 * \ingroup g_encode
 *
 * \param pid Process ID of the child whose argument is to be set.
 * \param bitness Bitness of the child
 * \param ind The index of the argument (0-5, see #PINK_MAX_INDEX)
 * \param src The data to be encoded
 * \param len Length of the data
 *
 * \return true on success, false on failure and sets errno accordingly.
 **/
bool
pink_encode_simple(pid_t pid, pink_bitness_t bitness, unsigned ind, const void *src, size_t len);

#if defined(PINKTRACE_LINUX) || defined(DOXYGEN)
/**
 * Write the given data argument src to the address of the argument arg safely.
 *
 * \ingroup g_encode
 *
 * \note Availability: Linux
 *
 * \param pid Process ID of the child whose argument is to be set.
 * \param bitness Bitness of the child
 * \param ind The index of the argument (0-5, see #PINK_MAX_INDEX)
 * \param src The data to be encoded
 * \param len Length of the data
 *
 * \return true on success, false on failure and sets errno accordingly.
 **/
bool
pink_encode_simple_safe(pid_t pid, pink_bitness_t bitness, unsigned ind, const void *src, size_t len);
#endif /* defined(PINKTRACE_LINUX)... */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !PINKTRACE_GUARD_ENCODE_H */
