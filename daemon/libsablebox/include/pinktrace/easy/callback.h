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

#ifndef PINKTRACE_EASY_GUARD_CALLBACK_H
#define PINKTRACE_EASY_GUARD_CALLBACK_H 1

#include <stdbool.h>

#include <pinktrace/easy/error.h>
#include <pinktrace/easy/process.h>

/**
 * \file
 * \brief Pink's easy \c ptrace(2) event callbacks
 *
 * \ingroup g_easy_callback
 **/

/**
 * Implies that the loop should be aborted immediately.
 *
 * \ingroup g_easy_callback
 **/
#define PINK_EASY_CFLAG_ABRT		(1 << 0)

/**
 * Implies that the current process should be removed from the
 * process tree. Useful for handling -ESRCH in callbacks.
 *
 * \ingroup g_easy_callback
 **/
#define PINK_EASY_CFLAG_DROP		(1 << 1)

/**
 * Implies that the child of the current process should be removed from the
 * process tree. Useful for handling -ESRCH.
 *
 * This only makes sense for fork, vfork and clone events.
 *
 * \ingroup g_easy_callback
 **/
#define PINK_EASY_CFLAG_DROP_CHILD	(1 << 2)

/**
 * Do not deliver the signal to the traced child.
 *
 * \ingroup g_easy_callback
 **/
#define PINK_EASY_CFLAG_SIGIGN		(1 << 3)

struct pink_easy_context;

/**
 * Errback for errors in the main process.
 * - Use pink_easy_context_get_error() to get the error code.
 *
 * There are a few important points about this callback:
 * - The variable arguments give extra information about the error condition
 *   and they vary between different error conditions.
 * - After some error conditions, the global variable errno may also give
 *   information about the failure reason of the underlying library call.
 *
 * Here's a list of possible error conditions, in no particular order:
 *
 * \verbatim
     ----------------------------------------------------------------------
     - Error             errno  Arguments                                 -
     ----------------------------------------------------------------------
     - ALLOC_ELDEST      +      -                                         -
     - ALLOC             +      pid_t pid                                 -
     - ATTACH            +      pid_t pid                                 -
     - FORK              +      -                                         -
     - VFORK             +      -                                         -
     - WAIT_ELDEST       +      pid_t pid                                 -
     - WAIT              +      -                                         -
     - STOP_ELDEST       -      pid_t pid, int status                     -
     - SETUP_ELDEST      +      pid_t pid                                 -
     - SETUP             -      pink_easy_process_t *current              -
     - BITNESS_ELDEST    +      pid_t pid                                 -
     - BITNESS           +      pink_easy_process_t *current              -
     - STEP_INITIAL      +      pink_easy_process_t *current              -
     - STEP_STOP         +      pink_easy_process_t *current              -
     - STEP_TRAP         +      pink_easy_process_t *current              -
     - STEP_SYSCALL      +      pink_easy_process_t *current              -
     - STEP_FORK         +      pink_easy_process_t *current              -
     - STEP_EXEC         +      pink_easy_process_t *current              -
     - STEP_EXIT         +      pink_easy_process_t *current              -
     - STEP_SIGNAL       +      pink_easy_process_t *current, int status  -
     - GETEVENTMSG_FORK  +      pink_easy_process_t *current              -
     - GETEVENTMSG_EXIT  +      pid_t pid                                 -
     - EVENT_UNKNOWN     -      pink_easy_process_t *current, int status  -
     ----------------------------------------------------------------------
   \endverbatim
 *
 * \ingroup g_easy_callback
 *
 * \param ctx Tracing context
 * \param ... Variable arguments give extra information about the error.
 **/
typedef void (*pink_easy_errback_t) (const struct pink_easy_context *ctx, ...);

/**
 * Errback for errors in the spawned child.
 *
 * \ingroup g_easy_callback
 *
 * \param e Error code
 *
 * \return Child exists with this return value
 **/
typedef int (*pink_easy_errback_child_t) (pink_easy_child_error_t e);

/**
 * Callback for child birth.
 *
 * This is called when a new process is created.
 *
 * \ingroup g_easy_callback
 *
 * \param ctx Tracing context
 * \param current New born child
 * \param parent The parent of the new child or NULL for the eldest child.
 **/
typedef void (*pink_easy_callback_birth_t) (const struct pink_easy_context *ctx, pink_easy_process_t *current, pink_easy_process_t *parent);

/**
 * Callback for child death.
 *
 * This is called in case a process exited.
 *
 * In this case, the process is removed from the process tree.
 * This is the last callback that is called before the child is freed.
 *
 * \ingroup g_easy_callback
 *
 * \param ctx Tracing context
 * \param current Dead child
 **/
typedef void (*pink_easy_callback_death_t) (const struct pink_easy_context *ctx, const pink_easy_process_t *current);

/**
 * Callback for the end of tracing.
 *
 * This is called when the count of the process tree dropped to zero, or
 * waitpid() returned -ECHILD. If this callback is NULL, pink_easy_loop() will
 * just return with success, which may not always be what you want.
 *
 * \ingroup g_easy_callback
 *
 * \see pink_easy_loop
 *
 * \param ctx Tracing context
 * \param echild true if waitpid() failed with -ECHILD, false if process tree
 * dropped to zero.
 *
 * \return This value is returned by pink_easy_loop()
 **/
typedef int (*pink_easy_callback_end_t) (const struct pink_easy_context *ctx, bool echild);

/**
 * Callback for #PINK_EVENT_TRAP
 *
 * \ingroup g_easy_callback
 *
 * \param ctx Tracing context
 * \param current Current child
 *
 * \return See PINK_EASY_CFLAG_* for flags to set in the return value.
 **/
typedef int (*pink_easy_callback_trap_t) (const struct pink_easy_context *ctx, pink_easy_process_t *current);

/**
 * Callback for #PINK_EVENT_SYSCALL
 *
 * \ingroup g_easy_callback
 *
 * \param ctx Tracing context
 * \param current Current child
 * \param entering true if the child is entering the system call, false
 * otherwise.
 *
 * \return See PINK_EASY_CFLAG_* for flags to set in the return value.
 **/
typedef int (*pink_easy_callback_syscall_t) (const struct pink_easy_context *ctx, pink_easy_process_t *current, bool entering);

/**
 * Callback for #PINK_EVENT_FORK, #PINK_EVENT_VFORK and #PINK_EVENT_CLONE
 *
 * \ingroup g_easy_callback
 *
 * \param ctx Tracing context
 * \param current Current child
 * \param alive true if the child was born and suspended before, false
 * otherwise.
 *
 * \return See PINK_EASY_CFLAG_* for flags to set in the return value.
 **/
typedef int (*pink_easy_callback_fork_t) (const struct pink_easy_context *ctx, pink_easy_process_t *current, pink_easy_process_t *child, bool alive);

/**
 * Callback for #PINK_EVENT_EXEC
 *
 * \ingroup g_easy_callback
 *
 * \note The bitness of the child is updated before this callback is called.
 *
 * \param ctx Tracing context
 * \param current Current child
 * \param old_bitness Old bitness of the child
 *
 * \return See PINK_EASY_CFLAG_* for flags to set in the return value.
 **/
typedef int (*pink_easy_callback_exec_t) (const struct pink_easy_context *ctx, pink_easy_process_t *current, pink_bitness_t old_bitness);

/**
 * Callback for #PINK_EVENT_EXIT
 *
 * \ingroup g_easy_callback
 *
 * \warning The current child may or may not exist in the process tree at this
 * point so this callback is called with a pid_t value. You may manually use
 * pink_easy_process_tree_search() to get a #pink_easy_process_t value.
 *
 * \param ctx Tracing context
 * \param current Process ID
 * \param status Exit status
 *
 * \return See PINK_EASY_CFLAG_* for flags to set in the return value.
 **/
typedef int (*pink_easy_callback_pre_exit_t) (const struct pink_easy_context *ctx, pid_t pid, unsigned long status);

/**
 * Callback for #PINK_EVENT_GENUINE
 *
 * \ingroup g_easy_callback
 *
 * \param ctx Tracing context
 * \param current Current process
 * \param stopsig Stop signal
 *
 * \return See PINK_EASY_CFLAG_* for flags to set in the return value.
 **/
typedef int (*pink_easy_callback_signal_t) (const struct pink_easy_context *ctx, pink_easy_process_t *current, int stopsig);

/**
 * Callback for #PINK_EVENT_EXIT_GENUINE and #PINK_EVENT_EXIT_SIGNAL
 *
 * \ingroup g_easy_callback
 *
 * \param ctx Tracing context
 * \param pid Process ID of the child
 * \param cs Exit code for #PINK_EVENT_EXIT_GENUINE, termination signal for
 * #PINK_EVENT_EXIT_SIGNAL.
 *
 * \return See PINK_EASY_CFLAG_* for flags to set in the return value.
 **/
typedef int (*pink_easy_callback_exit_t) (const struct pink_easy_context *ctx, pid_t pid, int cs);

/**
 * \brief Structure which represents a callback table
 *
 * \ingroup g_easy_callback
 **/
typedef struct pink_easy_callback_table {
	/** "error" errback **/
	pink_easy_errback_t error;
	/** "cerror" errback **/
	pink_easy_errback_child_t cerror;

	/** "birth" callback **/
	pink_easy_callback_birth_t birth;
	/** "death" callback **/
	pink_easy_callback_death_t death;
	/** "end" callback **/
	pink_easy_callback_end_t end;

	/** "trap" callback **/
	pink_easy_callback_trap_t trap;
	/** "syscall" callback **/
	pink_easy_callback_syscall_t syscall;
	/** "fork" callback **/
	pink_easy_callback_fork_t fork;
	/** "vfork" callback **/
	pink_easy_callback_fork_t vfork;
	/** "clone" callback **/
	pink_easy_callback_fork_t clone;
	/** "exec" callback **/
	pink_easy_callback_exec_t exec;
	/** "pre_exit" callback **/
	pink_easy_callback_pre_exit_t pre_exit;
	/** "signal" callback **/
	pink_easy_callback_signal_t signal;

	/** "exit" callback **/
	pink_easy_callback_exit_t exit;
	/** "exit_signal" callback **/
	pink_easy_callback_exit_t exit_signal;
} pink_easy_callback_table_t;

#endif /* !PINKTRACE_EASY_GUARD_CALLBACK_H */
