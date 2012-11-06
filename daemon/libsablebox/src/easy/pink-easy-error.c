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

#include <pinktrace/pink.h>
#include <pinktrace/easy/pink.h>

const char *
pink_easy_child_strerror(pink_easy_child_error_t e)
{
	switch (e) {
	case PINK_EASY_CHILD_ERROR_SUCCESS:
		return "Success";
	case PINK_EASY_CHILD_ERROR_SETUP:
		return "Failed to set up trace";
	case PINK_EASY_CHILD_ERROR_EXEC:
		return "execve() failed";
	case PINK_EASY_CHILD_ERROR_MAX:
	default:
		return "Unknown error";
	}
}

const char *
pink_easy_strerror(pink_easy_error_t e)
{
	switch (e) {
	case PINK_EASY_ERROR_SUCCESS:
		return "Success";
	case PINK_EASY_ERROR_CALLBACK_ABORT:
		return "Operation aborted by callback";
	case PINK_EASY_ERROR_ATTACH:
		return "Attaching to the child failed";
	case PINK_EASY_ERROR_ALLOC_ELDEST:
		return "Allocating memory for eldest child failed";
	case PINK_EASY_ERROR_ALLOC:
		return "Allocating memory for new child failed";
	case PINK_EASY_ERROR_FORK:
		return "fork() failed";
	case PINK_EASY_ERROR_VFORK:
		return "vfork() failed";
	case PINK_EASY_ERROR_STOP_ELDEST:
		return "Eldest child didn't stop";
	case PINK_EASY_ERROR_SETUP_ELDEST:
		return "Failed to set up eldest child";
	case PINK_EASY_ERROR_SETUP:
		return "Failed to set up child";
	case PINK_EASY_ERROR_BITNESS_ELDEST:
		return "Failed to update bitness of eldest child";
	case PINK_EASY_ERROR_BITNESS:
		return "Failed to update bitness after successful execve()";
	case PINK_EASY_ERROR_STEP_INITIAL:
		return "Initial step failed";
	case PINK_EASY_ERROR_STEP_STOP:
		return "Failed to step after event stop";
	case PINK_EASY_ERROR_STEP_TRAP:
		return "Failed to step after event trap";
	case PINK_EASY_ERROR_STEP_SYSCALL:
		return "Failed to step after event syscall";
	case PINK_EASY_ERROR_STEP_EXIT:
		return "Failed to setp after event exit";
	case PINK_EASY_ERROR_STEP_FORK:
		return "Failed to step after fork()";
	case PINK_EASY_ERROR_STEP_EXEC:
		return "Failed to step after execve()";
	case PINK_EASY_ERROR_STEP_SIGNAL:
		return "Failed to step after genuine signal";
	case PINK_EASY_ERROR_WAIT:
		return "Failed to wait() for child";
	case PINK_EASY_ERROR_WAIT_ELDEST:
		return "Failed to wait() for eldest child";
	case PINK_EASY_ERROR_GETEVENTMSG_FORK:
		return "Failed to get event message after fork()";
	case PINK_EASY_ERROR_GETEVENTMSG_EXIT:
		return "Failed to get event message after exit()";
	case PINK_EASY_ERROR_EVENT_UNKNOWN:
		return "Unknown event";
	case PINK_EASY_ERROR_MAX:
	default:
		return "Unknown error";
	}
}
