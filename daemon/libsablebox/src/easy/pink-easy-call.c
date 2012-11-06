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

#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

#include <pinktrace/pink.h>
#include <pinktrace/easy/pink.h>

int
pink_easy_call(pink_easy_context_t *ctx, pink_easy_child_func_t func, void *userdata)
{
	pink_easy_process_t *proc;

	assert(ctx != NULL);
	assert(func != NULL);

	proc = calloc(1, sizeof(pink_easy_process_t));
	if (!proc) {
		ctx->error = PINK_EASY_ERROR_ALLOC_ELDEST;
		if (ctx->callback_table.error)
			ctx->callback_table.error(ctx);
		return -ctx->error;
	}

	if ((proc->pid = fork()) < 0) {
		ctx->error = PINK_EASY_ERROR_FORK;
		if (ctx->callback_table.error)
			ctx->callback_table.error(ctx);
		goto fail;
	}
	else if (!proc->pid) { /* child */
		if (!pink_trace_me())
			_exit(ctx->callback_table.cerror ? ctx->callback_table.cerror(PINK_EASY_CHILD_ERROR_SETUP) : EXIT_FAILURE);
		kill(getpid(), SIGTRAP);
		_exit(func(userdata));
	}
	/* parent */

	if (!pink_easy_internal_init(ctx, proc))
		return 0;
fail:
	free(proc);
	return -ctx->error;
}
