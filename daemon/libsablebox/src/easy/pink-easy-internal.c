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

#include <pinktrace/easy/internal.h>
#include <pinktrace/easy/internal-util.h>

#include <assert.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/queue.h>

#include <pinktrace/pink.h>
#include <pinktrace/easy/pink.h>

pid_t
waitpid_nointr(pid_t pid, int *status)
{
	pid_t p;

	for (;;) {
		if ((p = waitpid(pid, status,
#ifdef __WALL
						__WALL
#else
						0
#endif /* __WALL */
				)) >= 0)
			return p;

		if (errno != EINTR)
			return p;
	}
	/* never reached */
	assert(false);
}

/** Initialize tracing **/
int
pink_easy_internal_init(pink_easy_context_t *ctx, pink_easy_process_t *proc)
{
	int status;

	/* Wait for the initial sig */
	if (waitpid_nointr(proc->pid, &status) < 0) {
		ctx->error = PINK_EASY_ERROR_WAIT_ELDEST;
		if (ctx->callback_table.error)
			ctx->callback_table.error(ctx, proc->pid);
		return -ctx->error;
	}
	if (!WIFSTOPPED(status) /* || WSTOPSIG(status) != SIGTRAP */) {
		ctx->error = PINK_EASY_ERROR_STOP_ELDEST;
		if (ctx->callback_table.error)
			ctx->callback_table.error(ctx, proc->pid, status);
		return -ctx->error;
	}

	/* Set up tracing options */
	if (!pink_trace_setup(proc->pid, ctx->ptrace_options)) {
		ctx->error = PINK_EASY_ERROR_SETUP_ELDEST;
		if (ctx->callback_table.error)
			ctx->callback_table.error(ctx, proc->pid);
		return -ctx->error;
	}

	/* Figure out bitness */
	if ((proc->bitness = pink_bitness_get(proc->pid)) == PINK_BITNESS_UNKNOWN) {
		ctx->error = PINK_EASY_ERROR_BITNESS_ELDEST;
		if (ctx->callback_table.error)
			ctx->callback_table.error(ctx, proc->pid);
		return -ctx->error;
	}

	/* Set up flags */
	proc->flags |= PINK_EASY_PROCESS_STARTUP;
	if (ctx->ptrace_options & PINK_TRACE_OPTION_FORK
			|| ctx->ptrace_options & PINK_TRACE_OPTION_VFORK
			|| ctx->ptrace_options & PINK_TRACE_OPTION_CLONE)
		proc->flags |= PINK_EASY_PROCESS_FOLLOWFORK;

	/* Insert the process into the list */
	proc->ppid = -1;
	proc->flags &= ~PINK_EASY_PROCESS_STARTUP;
	SLIST_INSERT_HEAD(&ctx->process_list, proc, entries);

	/* Happy birthday! */
	if (ctx->callback_table.birth)
		ctx->callback_table.birth(ctx, proc, NULL);

	/* Push the child to move! */
	if (!pink_trace_syscall(proc->pid, 0)) {
		ctx->error = PINK_EASY_ERROR_STEP_INITIAL;
		if (ctx->callback_table.error)
			ctx->callback_table.error(ctx, proc);
		return -ctx->error;
	}

	return 0;
}
