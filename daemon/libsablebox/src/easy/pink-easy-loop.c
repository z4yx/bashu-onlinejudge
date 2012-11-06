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
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <sys/wait.h>

#include <pinktrace/pink.h>
#include <pinktrace/easy/pink.h>

inline
static pink_easy_error_t
error_step(pink_event_t event)
{
	switch (event) {
	case PINK_EVENT_STOP:
		return PINK_EASY_ERROR_STEP_STOP;
	case PINK_EVENT_TRAP:
		return PINK_EASY_ERROR_STEP_TRAP;
	case PINK_EVENT_SYSCALL:
		return PINK_EASY_ERROR_STEP_SYSCALL;
	case PINK_EVENT_EXEC:
		return PINK_EASY_ERROR_STEP_EXEC;
	case PINK_EVENT_EXIT:
		return PINK_EASY_ERROR_STEP_EXIT;
	case PINK_EVENT_FORK:
	case PINK_EVENT_VFORK:
	case PINK_EVENT_CLONE:
		return PINK_EASY_ERROR_STEP_FORK;
	case PINK_EVENT_GENUINE:
	case PINK_EVENT_UNKNOWN:
		return PINK_EASY_ERROR_STEP_SIGNAL;
	default:
		abort();
	}

	/* never reached */
	assert(0);
}

static void
handle_death(pink_easy_context_t *ctx, pink_easy_process_t *proc)
{
	assert(proc != NULL);

	pink_easy_process_list_remove(&ctx->process_list, proc);

	/* R.I.P. */
	if (ctx->callback_table.death)
		ctx->callback_table.death(ctx, proc);
	if (proc->userdata_destroy && proc->userdata)
		proc->userdata_destroy(proc->userdata);
	free(proc);
}

static bool
handle_ptrace_error(pink_easy_context_t *ctx, pink_easy_error_t error, pink_easy_process_t *proc, pid_t pid)
{
	if (errno == ESRCH) {
		/* Child is dead! */
		if (proc)
			handle_death(ctx, proc);
		return true;
	}

	/* Fatal ptrace() error! */
	ctx->error = error;
	if (ctx->callback_table.error) {
		if (proc)
			ctx->callback_table.error(ctx, proc);
		else
			ctx->callback_table.error(ctx, pid);
	}

	return false;
}

static bool
handle_step(pink_easy_context_t *ctx, pink_easy_process_t *proc, int sig, pink_event_t event)
{
	assert(proc != NULL);

	if (pink_trace_syscall(proc->pid, sig))
		return true;
	return handle_ptrace_error(ctx, error_step(event), proc, -1);
}

static pink_easy_tribool_t
handle_stop(pink_easy_context_t *ctx, pid_t pid, pink_easy_process_t **nproc)
{
	pink_easy_process_t *proc, *pproc;

	proc = pink_easy_process_list_lookup(&ctx->process_list, pid);
	if (proc) {
		if (!(proc->flags & PINK_EASY_PROCESS_STARTUP)) {
			/*
			 * FIXME: Process is not starting but received a SIGSTOP.
			 * Just ignore the signal for now, maybe there is a
			 * saner way to treat this?
			 */
			*nproc = proc;
			return PINK_EASY_TRIBOOL_NONE;
		}

		assert(proc->ppid > 0);
		pproc = pink_easy_process_list_lookup(&ctx->process_list, proc->ppid);
		assert(pproc != NULL);

		/* Set up the child */
		if (!pink_trace_setup(proc->pid, ctx->ptrace_options))
			return handle_ptrace_error(ctx, PINK_EASY_ERROR_SETUP, proc, -1)
				? PINK_EASY_TRIBOOL_TRUE
				: PINK_EASY_TRIBOOL_FALSE;

		/*
		 * Happy birthday kiddo!
		 */
		if (ctx->callback_table.birth)
			ctx->callback_table.birth(ctx, proc, pproc);

		proc->flags &= ~PINK_EASY_PROCESS_STARTUP;
		*nproc = proc;

		return PINK_EASY_TRIBOOL_NONE;
	}

	/*
	 * Child is born before PINK_EVENT_FORK
	 * Not much we can do other than allocating the process structure,
	 * inserting into the list and marking it suspended.
	 */
	proc = calloc(1, sizeof(pink_easy_process_t));
	if (!proc) {
		/* OOM */
		ctx->error = PINK_EASY_ERROR_ALLOC;
		if (ctx->callback_table.error)
			ctx->callback_table.error(ctx, pid);
		return PINK_EASY_TRIBOOL_FALSE;
	}

	/*
	 * fork() handler will take care of the rest.
	 */
	proc->pid = pid;
	proc->flags |= PINK_EASY_PROCESS_SUSPENDED;
	SLIST_INSERT_HEAD(&ctx->process_list, proc, entries);

	return PINK_EASY_TRIBOOL_NONE;
}

static pink_easy_tribool_t
handle_trap(pink_easy_context_t *ctx, pink_easy_process_t *proc)
{
	bool abrt;
	int flags;

	/* Run the "trap" callback */
	if (ctx->callback_table.trap) {
		flags = ctx->callback_table.trap(ctx, proc);

		abrt = flags & PINK_EASY_CFLAG_ABRT;
		if (abrt)
			ctx->error = PINK_EASY_ERROR_CALLBACK_ABORT;

		if (flags & PINK_EASY_CFLAG_DROP) {
			handle_death(ctx, proc);
			return abrt ? PINK_EASY_TRIBOOL_FALSE : PINK_EASY_TRIBOOL_TRUE;
		}
		else if (abrt)
			return PINK_EASY_TRIBOOL_FALSE;
	}

	return PINK_EASY_TRIBOOL_NONE;
}

static pink_easy_tribool_t
handle_syscall(pink_easy_context_t *ctx, pink_easy_process_t *proc)
{
	bool abrt, entering;
	int flags;

	entering = !(proc->flags & PINK_EASY_PROCESS_INSYSCALL);
	proc->flags ^= PINK_EASY_PROCESS_INSYSCALL;

	if (ctx->callback_table.syscall) {
		flags = ctx->callback_table.syscall(ctx, proc, entering);

		abrt = flags & PINK_EASY_CFLAG_ABRT;
		if (abrt)
			ctx->error = PINK_EASY_ERROR_CALLBACK_ABORT;

		if (flags & PINK_EASY_CFLAG_DROP) {
			handle_death(ctx, proc);
			return abrt ? PINK_EASY_TRIBOOL_FALSE : PINK_EASY_TRIBOOL_TRUE;
		}
		else if (abrt)
			return PINK_EASY_TRIBOOL_FALSE;
	}

	return PINK_EASY_TRIBOOL_NONE;
}

static pink_easy_tribool_t
handle_exec(pink_easy_context_t *ctx, pink_easy_process_t *proc)
{
	bool abrt;
	int flags;
	pink_bitness_t old_bitness;

	/* Update bitness */
	old_bitness = proc->bitness;
	if ((proc->bitness = pink_bitness_get(proc->pid)) == PINK_BITNESS_UNKNOWN)
		return handle_ptrace_error(ctx, PINK_EASY_ERROR_BITNESS, proc, -1)
			? PINK_EASY_TRIBOOL_TRUE
			: PINK_EASY_TRIBOOL_FALSE;

	/* Run the "exec" callback */
	if (ctx->callback_table.exec) {
		flags = ctx->callback_table.exec(ctx, proc, old_bitness);

		abrt = flags & PINK_EASY_CFLAG_ABRT;
		if (abrt)
			ctx->error = PINK_EASY_ERROR_CALLBACK_ABORT;

		if (flags & PINK_EASY_CFLAG_DROP) {
			handle_death(ctx, proc);
			return abrt ? PINK_EASY_TRIBOOL_FALSE : PINK_EASY_TRIBOOL_TRUE;
		}
		else if (abrt)
			return PINK_EASY_TRIBOOL_FALSE;
	}

	return PINK_EASY_TRIBOOL_NONE;
}

static pink_easy_tribool_t
handle_pre_exit(pink_easy_context_t *ctx, pid_t pid, pink_easy_process_t *proc)
{
	bool abrt;
	int flags;
	unsigned long status;

	/* Run the "pre_exit" callback */
	if (ctx->callback_table.pre_exit) {
		if (!pink_trace_geteventmsg(pid, &status))
			return handle_ptrace_error(ctx, PINK_EASY_ERROR_GETEVENTMSG_EXIT, proc, pid)
				? PINK_EASY_TRIBOOL_TRUE
				: PINK_EASY_TRIBOOL_FALSE;

		flags = ctx->callback_table.pre_exit(ctx, pid, status);

		abrt = flags & PINK_EASY_CFLAG_ABRT;
		if (abrt)
			ctx->error = PINK_EASY_ERROR_CALLBACK_ABORT;

		if (flags & PINK_EASY_CFLAG_DROP) {
			if (proc)
				handle_death(ctx, proc);
			return abrt ? PINK_EASY_TRIBOOL_FALSE : PINK_EASY_TRIBOOL_TRUE;
		}
		else if (abrt)
			return PINK_EASY_TRIBOOL_FALSE;
	}

	return PINK_EASY_TRIBOOL_NONE;
}

PINK_GCC_ATTR((nonnull(1,4)))
static pink_easy_tribool_t
handle_fork(pink_easy_context_t *ctx, pink_easy_process_t *proc, pink_event_t event, pink_easy_process_t **nproc)
{
	bool abrt, suspended;
	int flags;
	unsigned long cpid;
	pink_easy_process_t *cproc;
	pink_easy_callback_fork_t cbfork;

	if (!pink_trace_geteventmsg(proc->pid, &cpid))
		return handle_ptrace_error(ctx, PINK_EASY_ERROR_GETEVENTMSG_FORK, proc, -1)
			? PINK_EASY_TRIBOOL_TRUE
			: PINK_EASY_TRIBOOL_FALSE;

	cproc = pink_easy_process_list_lookup(&ctx->process_list, (pid_t)cpid);
	if (cproc) {
		/* Child was born before PINK_EVENT_FORK */
		assert(cproc->flags & PINK_EASY_PROCESS_SUSPENDED);

		/* Inherit crucial data */
		cproc->bitness = proc->bitness;
		if (proc->flags & PINK_EASY_PROCESS_ATTACHED)
			cproc->flags |= PINK_EASY_PROCESS_ATTACHED;
		if (proc->flags & PINK_EASY_PROCESS_FOLLOWFORK)
			cproc->flags |= PINK_EASY_PROCESS_FOLLOWFORK;

		/* Set the child up */
		if (!pink_trace_setup(cproc->pid, ctx->ptrace_options))
			return handle_ptrace_error(ctx, PINK_EASY_ERROR_SETUP, proc, -1)
				? PINK_EASY_TRIBOOL_TRUE
				: PINK_EASY_TRIBOOL_FALSE;

		return PINK_EASY_TRIBOOL_NONE;
	}

	/* Child hasn't been born yet! */
	cproc = calloc(1, sizeof(pink_easy_process_t));
	if (!cproc) {
		ctx->error = PINK_EASY_ERROR_ALLOC;
		if (ctx->callback_table.error)
			ctx->callback_table.error(ctx, proc, cpid);
		return PINK_EASY_TRIBOOL_FALSE;
	}

	cproc->pid = (pid_t)cpid;
	cproc->ppid = proc->pid;
	cproc->bitness = proc->bitness;
	cproc->flags |= PINK_EASY_PROCESS_STARTUP;
	if (proc->flags & PINK_EASY_PROCESS_ATTACHED)
		cproc->flags |= PINK_EASY_PROCESS_ATTACHED;
	if (proc->flags & PINK_EASY_PROCESS_FOLLOWFORK)
		cproc->flags |= PINK_EASY_PROCESS_FOLLOWFORK;
	SLIST_INSERT_HEAD(&ctx->process_list, cproc, entries);

	/* Last but not least, run the callback */
	cbfork = (event == PINK_EVENT_FORK)
		? ctx->callback_table.fork
		: ((event == PINK_EVENT_VFORK)
				? ctx->callback_table.vfork
				: ctx->callback_table.clone);

	if (cbfork) {
		suspended = cproc->flags & PINK_EASY_PROCESS_SUSPENDED;
		flags = cbfork(ctx, proc, cproc, suspended);

		abrt = flags & PINK_EASY_CFLAG_ABRT;
		if (abrt)
			ctx->error = PINK_EASY_ERROR_CALLBACK_ABORT;

		if (flags & PINK_EASY_CFLAG_DROP_CHILD) {
			/* Received ESRCH from the new-born child */

			/*
			 * No way to receive ESRCH from child if she is not yet
			 * born, hence the assertion.
			 */
			assert(suspended);

			handle_death(ctx, cproc);
		}
		else
			*nproc = cproc;

		if (flags & PINK_EASY_CFLAG_DROP) {
			handle_death(ctx, proc);
			return abrt ? PINK_EASY_TRIBOOL_FALSE : PINK_EASY_TRIBOOL_TRUE;
		}
		else if (abrt)
			return PINK_EASY_TRIBOOL_FALSE;
	}

	return PINK_EASY_TRIBOOL_NONE;
}

static bool
handle_exit(pink_easy_context_t *ctx, pid_t pid, int status)
{
	int flags;
	pink_easy_process_t *proc;

	/* R.I.P. */
	proc = pink_easy_process_list_lookup(&ctx->process_list, pid);
	if (proc) {
		/* Child is not dead yet!
		 * Kill it with fire!
		 */
		handle_death(ctx, proc);
	}

	if (WIFSIGNALED(status)) {
		if (ctx->callback_table.exit_signal) {
			flags = ctx->callback_table.exit_signal(ctx, pid, WTERMSIG(status));
			if (flags & PINK_EASY_CFLAG_ABRT) {
				ctx->error = PINK_EASY_ERROR_CALLBACK_ABORT;
				return false;
			}
		}
	}
	else if (ctx->callback_table.exit) {
		flags = ctx->callback_table.exit(ctx, pid, WEXITSTATUS(status));
		if (flags & PINK_EASY_CFLAG_ABRT) {
			ctx->error = PINK_EASY_ERROR_CALLBACK_ABORT;
			return false;
		}
	}

	return true;
}

static pink_easy_tribool_t
handle_signal(pink_easy_context_t *ctx, pink_easy_process_t *proc, int status, bool unknown, int *mysig)
{
	bool abrt;
	int flags;
	int stopsig;

	assert(mysig);

	if (unknown && !WIFSTOPPED(status)) {
		/* In an ideal world, this should not happen,
		 * but it does...
		 */
		ctx->error = PINK_EASY_ERROR_EVENT_UNKNOWN;
		if (ctx->callback_table.error)
			ctx->callback_table.error(ctx, proc, status);
		return false;
	}

	stopsig = WSTOPSIG(status);
	*mysig = stopsig;

	/* Run the "signal" callback */
	if (ctx->callback_table.signal) {
		flags = ctx->callback_table.signal(ctx, proc, stopsig);

		if (flags & PINK_EASY_CFLAG_SIGIGN)
			*mysig = 0;

		abrt = flags & PINK_EASY_CFLAG_ABRT;
		if (abrt)
			ctx->error = PINK_EASY_ERROR_CALLBACK_ABORT;

		if (flags & PINK_EASY_CFLAG_DROP) {
			handle_death(ctx, proc);
			return abrt ? PINK_EASY_TRIBOOL_FALSE : PINK_EASY_TRIBOOL_TRUE;
		}
		else if (abrt)
			return PINK_EASY_TRIBOOL_FALSE;
	}

	return PINK_EASY_TRIBOOL_NONE;
}

int
pink_easy_loop(pink_easy_context_t *ctx)
{
	int status, mysig;
	pid_t pid;
	pink_event_t event;
	pink_easy_tribool_t ret;
	pink_easy_process_t *proc, *nproc;

	assert(ctx != NULL);

	/* Enter the event loop */
	for (;;) {
		/* Wait for children */
		if ((pid = waitpid_nointr(-1, &status)) < 0) {
			if (errno == ECHILD) {
				/* Received ECHILD, end of tracing */
				return ctx->callback_table.end ? ctx->callback_table.end(ctx, true) : EXIT_SUCCESS;
			}

			/*
			 * wait(2) error. ECHILD and EINTR are handled.
			 * The rest are fatal errors.
			 */
			ctx->error = PINK_EASY_ERROR_WAIT, ctx->fatal = true;
			if (ctx->callback_table.error)
				ctx->callback_table.error(ctx);
			return -ctx->error;
		}

		/* Decide the event */
		event = pink_event_decide(status);

		switch (event) {
		case PINK_EVENT_STOP:
			/* Search the child in the process list */
			nproc = NULL;
			ret = handle_stop(ctx, pid, &nproc);
			if (ret == PINK_EASY_TRIBOOL_NONE) {
				if (nproc && !(nproc->flags & PINK_EASY_PROCESS_SUSPENDED)) {
					/* Push the process to move! */
					ret = handle_step(ctx, nproc, 0, event);
					if (ret == PINK_EASY_TRIBOOL_FALSE)
						return -ctx->error;
				}
			}
			else if (ret == PINK_EASY_TRIBOOL_FALSE) {
				/* Nothing is fine, abort the loop! */
				return -ctx->error;
			}
			/* else if (ret == PINK_EASY_TRIBOOL_TRUE); */
			break;
		case PINK_EVENT_TRAP:
			/* Search the child in the process list */
			proc = pink_easy_process_list_lookup(&ctx->process_list, pid);
			assert(proc != NULL);
			ret = handle_trap(ctx, proc);
			if (ret == PINK_EASY_TRIBOOL_NONE) {
				/* Push the process to move! */
				ret = handle_step(ctx, proc, SIGTRAP, event);
				if (ret == PINK_EASY_TRIBOOL_FALSE)
					return -ctx->error;
			}
			else if (ret == PINK_EASY_TRIBOOL_FALSE) {
				/* Nothing is fine, abort the loop! */
				return -ctx->error;
			}
			/* else if (ret == PINK_EASY_TRIBOOL_TRUE); */
			break;
		case PINK_EVENT_SYSCALL:
			/* Search the child in the process list */
			proc = pink_easy_process_list_lookup(&ctx->process_list, pid);
			assert(proc != NULL);
			ret = handle_syscall(ctx, proc);
			if (ret == PINK_EASY_TRIBOOL_NONE) {
				/* "Alles in Ordnung", continue to step */
				ret = handle_step(ctx, proc, 0, event);
				if (ret == PINK_EASY_TRIBOOL_FALSE)
					return -ctx->error;
			}
			else if (ret == PINK_EASY_TRIBOOL_FALSE) {
				/* Nothing is fine, abort the loop! */
				return -ctx->error;
			}
			/* else if (ret == PINK_EASY_TRIBOOL_TRUE); */
			break;
		case PINK_EVENT_EXEC:
			/* Search the child in the process list */
			proc = pink_easy_process_list_lookup(&ctx->process_list, pid);
			assert(proc != NULL);
			ret = handle_exec(ctx, proc);
			if (ret == PINK_EASY_TRIBOOL_NONE) {
				/* "Alles in Ordnung", continue to step */
				ret = handle_step(ctx, proc, 0, event);
				if (ret == PINK_EASY_TRIBOOL_FALSE)
					return -ctx->error;
			}
			else if (ret == PINK_EASY_TRIBOOL_FALSE) {
				/* Nothing is fine, abort the loop! */
				return -ctx->error;
			}
			/* else if (ret == PINK_EASY_TRIBOOL_TRUE); */
			break;
		case PINK_EVENT_EXIT:
			/* Search the child in the process list,
			 * she may or may not exist in the list at this point.
			 */
			proc = pink_easy_process_list_lookup(&ctx->process_list, pid);
			ret = handle_pre_exit(ctx, pid, proc);
			if (ret == PINK_EASY_TRIBOOL_NONE) {
				/* "Alles in Ordnung", resume the child */
				if (!pink_trace_resume(pid, 0)) {
					ret = handle_ptrace_error(ctx, error_step(event), proc, pid);
					if (ret == PINK_EASY_TRIBOOL_FALSE)
						return -ctx->error;
				}
			}
			else if (ret == PINK_EASY_TRIBOOL_FALSE) {
				/* Nothing is fine, abort the loop! */
				return -ctx->error;
			}
			/* else if (ret == PINK_EASY_TRIBOOL_TRUE); */
			break;
		case PINK_EVENT_FORK:
		case PINK_EVENT_VFORK:
		case PINK_EVENT_CLONE:
			/* Search the child in the process list */
			proc = pink_easy_process_list_lookup(&ctx->process_list, pid);
			assert(proc != NULL);
			nproc = NULL;
			ret = handle_fork(ctx, proc, event, &nproc);
			if (ret == PINK_EASY_TRIBOOL_NONE) {
				/* Step the parent first */
				ret = handle_step(ctx, proc, 0, event);
				if (ret == PINK_EASY_TRIBOOL_FALSE)
					return -ctx->error;
				/* And then the child */
				if (nproc) {
					ret = handle_step(ctx, proc, 0, event);
					if (ret == PINK_EASY_TRIBOOL_FALSE)
						return -ctx->error;
				}
			}
			else if (ret == PINK_EASY_TRIBOOL_FALSE) {
				/* It all went wrong, abort the loop! */
				return -ctx->error;
			}
			/* else if (ret == PINK_EASY_TRIBOOL_TRUE); */
			break;
		case PINK_EVENT_GENUINE:
		case PINK_EVENT_UNKNOWN:
			/* Search the child in the process list */
			proc = pink_easy_process_list_lookup(&ctx->process_list, pid);
			assert(proc != NULL);
			ret = handle_signal(ctx, proc, status, event == PINK_EVENT_UNKNOWN, &mysig);
			if (ret == PINK_EASY_TRIBOOL_NONE) {
				/* "Alles in Ordnung", continue to step */
				ret = handle_step(ctx, proc, mysig, event);
				if (ret == PINK_EASY_TRIBOOL_FALSE)
					return -ctx->error;
			}
			else if (ret == PINK_EASY_TRIBOOL_FALSE) {
				/* Nothing is fine, abort the loop! */
				return -ctx->error;
			}
			/* else if (ret == PINK_EASY_TRIBOOL_TRUE); */
			break;
		case PINK_EVENT_EXIT_GENUINE:
		case PINK_EVENT_EXIT_SIGNAL:
			if (!handle_exit(ctx, pid, status))
				return -ctx->error;
			if (SLIST_EMPTY(&ctx->process_list))
				return ctx->callback_table.end ? ctx->callback_table.end(ctx, false) : EXIT_SUCCESS;
			break;
		default:
			abort();
		}
	}

	/* never reached */
	assert(0);
}
