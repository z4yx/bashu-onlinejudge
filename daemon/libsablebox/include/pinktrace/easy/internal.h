/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet cin fdm=syntax : */

/*
 * Copyright (c) 2010, 2011 Ali Polatel <alip@exherbo.org>
 * Based in part upon strace which is:
 *   Copyright (c) 1991, 1992 Paul Kranenburg <pk@cs.few.eur.nl>
 *   Copyright (c) 1993 Branko Lankester <branko@hacktic.nl>
 *   Copyright (c) 1993, 1994, 1995, 1996 Rick Sladkey <jrs@world.std.com>
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

#ifndef PINKTRACE_EASY_GUARD_INTERNAL_H
#define PINKTRACE_EASY_GUARD_INTERNAL_H 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdbool.h>
#include <sys/queue.h>
#include <sys/types.h>

#include <pinktrace/pink.h>
#include <pinktrace/easy/callback.h>
#include <pinktrace/easy/error.h>

/** We have just begun ptracing this process. **/
#define PINK_EASY_PROCESS_STARTUP		00001
/** This table entry is in use **/
#define PINK_EASY_PROCESS_INUSE			00002
/** A system call is in progress **/
#define PINK_EASY_PROCESS_INSYSCALL		00004
/** Process is not our own child **/
#define PINK_EASY_PROCESS_ATTACHED		00010
/** As far as we know, this process is exiting **/
#define PINK_EASY_PROCESS_EXITING		00020
/** Process can not be allowed to resume just now **/
#define PINK_EASY_PROCESS_SUSPENDED		00040
/** Process should have forks followed **/
#define PINK_EASY_PROCESS_FOLLOWFORK		00100
/** Process is a clone **/
#define PINK_EASY_PROCESS_CLONE_THREAD		00200

typedef enum {
	PINK_EASY_TRIBOOL_FALSE = 0,
	PINK_EASY_TRIBOOL_TRUE,
	PINK_EASY_TRIBOOL_NONE,
} pink_easy_tribool_t;

/** Process entry **/
struct pink_easy_process {
	/** PINK_EASY_PROCESS_* flags **/
	short flags;

	/** Process Id of this entry **/
	pid_t pid;

	/** Parent of this process **/
	pid_t ppid;

	/** Bitness (e.g. 32bit, 64bit) of this process **/
	pink_bitness_t bitness;

	/** Per-process user data **/
	void *userdata;

	/** Destructor for user data **/
	pink_easy_free_func_t userdata_destroy;

	SLIST_ENTRY(pink_easy_process) entries;
};
SLIST_HEAD(pink_easy_process_list, pink_easy_process);

/** Tracing context **/
struct pink_easy_context {
	/** Process list **/
	struct pink_easy_process_list process_list;

	/** Callback table **/
	pink_easy_callback_table_t callback_table;

	/** pink_trace_setup() options **/
	int ptrace_options;

	/** Last error **/
	pink_easy_error_t error;

	/** Was the error fatal? **/
	bool fatal;

	/** User data **/
	void *userdata;

	/** Destructor for the user data **/
	pink_easy_free_func_t userdata_destroy;
};

/** Initialize tracing **/
int
pink_easy_internal_init(struct pink_easy_context *ctx, pink_easy_process_t *proc);

#endif /* !PINKTRACE_EASY_GUARD_INTERNAL_H */
