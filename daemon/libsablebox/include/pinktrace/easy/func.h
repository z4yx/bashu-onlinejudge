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
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LpIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PINKTRACE_EASY_GUARD_FUNC_H
#define PINKTRACE_EASY_GUARD_FUNC_H 1

/**
 * \file
 * \brief Pink's easy function pointers
 *
 * \ingroup g_easy_func
 **/

struct pink_easy_process;

/**
 * This type definition represents a generic free() function.
 *
 * \ingroup g_easy_func
 *
 * \see pink_easy_context_new
 * \see pink_easy_process_set_data
 **/
typedef void (*pink_easy_free_func_t) (void *data);

/**
 * This type definition represents the process tree walk function.
 * It takes a process entry and userdata as argument. If this function returns
 * false, pink_easy_process_tree_walk() stops iterating through the process
 * tree and returns immediately.
 *
 * \ingroup g_easy_func
 *
 * \see pink_easy_process_tree_walk
 **/
typedef bool (*pink_easy_walk_func_t) (struct pink_easy_process *proc, void *userdata);

/**
 * This type definition represents a function to be executed by the child under
 * tracing. Its return value is passed directly to _exit().
 *
 * \ingroup g_easy_func
 *
 * \see pink_easy_call
 **/
typedef int (*pink_easy_child_func_t) (void *userdata);

#endif /* !PINKTRACE_EASY_GUARD_FUNC_H */
