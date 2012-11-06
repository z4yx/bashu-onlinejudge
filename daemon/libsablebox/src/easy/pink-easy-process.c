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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <sys/syscall.h>
#include <asm/unistd.h>

#include <pinktrace/pink.h>
#include <pinktrace/easy/pink.h>

int
pink_easy_process_kill(const pink_easy_process_t *proc, int sig)
{
	if (proc->flags & PINK_EASY_PROCESS_CLONE_THREAD) {
#if defined(__NR_tgkill)
		return syscall(__NR_tgkill, proc->ppid > 0 ? proc->ppid : proc->pid, proc->pid, sig);
#elif defined(__NR_tkill)
		return syscall(__NR_tkill, proc->pid, sig);
#else
#warning "Neither tgkill(2) nor tkill(2) available, killing threads may not work correctly!"
		/* void */;
#endif
	}

	return kill(proc->pid, sig);
}

bool
pink_easy_process_resume(const pink_easy_process_t *proc, int sig)
{
	if (proc->flags & PINK_EASY_PROCESS_ATTACHED)
		return pink_trace_detach(proc->pid, sig);
	else
		return pink_trace_resume(proc->pid, sig);
}

pid_t
pink_easy_process_get_pid(const pink_easy_process_t *proc)
{
	return proc->pid;
}

pid_t
pink_easy_process_get_ppid(const pink_easy_process_t *proc)
{
	return proc->ppid;
}

pink_bitness_t
pink_easy_process_get_bitness(const pink_easy_process_t *proc)
{
	return proc->bitness;
}

bool
pink_easy_process_is_attached(const pink_easy_process_t *proc)
{
	return !!(proc->flags & PINK_EASY_PROCESS_ATTACHED);
}

bool
pink_easy_process_is_clone(const pink_easy_process_t *proc)
{
	return !!(proc->flags & PINK_EASY_PROCESS_CLONE_THREAD);
}

void *
pink_easy_process_get_userdata(const pink_easy_process_t *proc)
{
	return proc->userdata;
}

void
pink_easy_process_set_userdata(pink_easy_process_t *proc, void *userdata, pink_easy_free_func_t userdata_destroy)
{
	proc->userdata = userdata;
	proc->userdata_destroy = userdata_destroy;
}

pink_easy_process_t *
pink_easy_process_list_lookup(const pink_easy_process_list_t *list, pid_t pid)
{
	pink_easy_process_t *node;

	assert(list != NULL);

	SLIST_FOREACH(node, list, entries) {
		if (node->pid == pid)
			return node;
	}

	return NULL;
}

void
pink_easy_process_list_remove(pink_easy_process_list_t *list, const pink_easy_process_t *proc)
{
	assert(list != NULL);

	SLIST_REMOVE(list, proc, pink_easy_process, entries);
}

unsigned
pink_easy_process_list_walk(const pink_easy_process_list_t *list, pink_easy_walk_func_t func, void *userdata)
{
	unsigned count;
	pink_easy_process_t *node;

	assert(list != NULL);
	assert(func != NULL);

	count = 0;
	SLIST_FOREACH(node, list, entries) {
		++count;
		if (!func(node, userdata))
			break;
	}

	return count;
}
