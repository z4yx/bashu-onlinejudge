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

#ifndef PINKTRACE_EASY_GUARD_ATTACH_H
#define PINKTRACE_EASY_GUARD_ATTACH_H 1

/**
 * \file
 * \brief Pink's easy process attaching
 *
 * \ingroup g_easy_attach
 **/

#include <sys/types.h>

#include <pinktrace/pink.h>
#include <pinktrace/easy/context.h>

/**
 * Attach to a process for tracing.
 * Call this multiple times before pink_easy_loop() to attach to multiple
 * processes.
 *
 * \ingroup g_easy_attach
 *
 * \param ctx Tracing context
 * \param pid Process ID
 * \param ppid Parent process ID. Use this to specify the parent of the process
 * in case the process is a clone. This is useful when attaching to all
 * threads of a process and makes pinktrace track whether the process is a
 * thread. Specify -1 for non-clones.
 *
 * \return Depends on the callbacks.
 **/
int
pink_easy_attach(pink_easy_context_t *ctx, pid_t pid, pid_t ppid) PINK_GCC_ATTR((nonnull(1)));

#endif /* !PINKTRACE_EASY_GUARD_ATTACH_H */
