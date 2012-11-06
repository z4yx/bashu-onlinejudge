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

#include <assert.h>
#include <sys/wait.h>

#include <pinktrace/internal.h>
#include <pinktrace/pink.h>

pink_event_t
pink_event_decide(int status)
{
	unsigned int event;

	if (WIFSTOPPED(status)) {
		switch (WSTOPSIG(status)) {
		case SIGSTOP:
			return PINK_EVENT_STOP;
		case SIGTRAP | 0x80:
			return PINK_EVENT_SYSCALL;
		case SIGTRAP:
			event = (status >> 16) & 0xffff;
			switch (event) {
			case PTRACE_EVENT_FORK:
				return PINK_EVENT_FORK;
			case PTRACE_EVENT_VFORK:
				return PINK_EVENT_VFORK;
			case PTRACE_EVENT_CLONE:
				return PINK_EVENT_CLONE;
			case PTRACE_EVENT_VFORK_DONE:
				return PINK_EVENT_VFORK_DONE;
			case PTRACE_EVENT_EXEC:
				return PINK_EVENT_EXEC;
			case PTRACE_EVENT_EXIT:
				return PINK_EVENT_EXIT;
			default:
				return PINK_EVENT_TRAP;
			}
		default:
			return PINK_EVENT_GENUINE;
		}
	}
	else if (WIFEXITED(status))
		return PINK_EVENT_EXIT_GENUINE;
	else if (WIFSIGNALED(status))
		return PINK_EVENT_EXIT_SIGNAL;

	return PINK_EVENT_UNKNOWN;
}

const char *
pink_event_name(pink_event_t event)
{
	switch (event) {
	case PINK_EVENT_STOP:
		return "stop";
	case PINK_EVENT_TRAP:
		return "trap";
	case PINK_EVENT_SYSCALL:
		return "syscall";
	case PINK_EVENT_FORK:
		return "fork";
	case PINK_EVENT_VFORK:
		return "vfork";
	case PINK_EVENT_CLONE:
		return "clone";
	case PINK_EVENT_VFORK_DONE:
		return "vfork_done";
	case PINK_EVENT_EXEC:
		return "exec";
	case PINK_EVENT_EXIT:
		return "exit";
	case PINK_EVENT_GENUINE:
		return "genuine";
	case PINK_EVENT_EXIT_GENUINE:
		return "exit_genuine";
	case PINK_EVENT_EXIT_SIGNAL:
		return "exit_signal";
	case PINK_EVENT_UNKNOWN:
	default:
		return "unknown";
	}
}
