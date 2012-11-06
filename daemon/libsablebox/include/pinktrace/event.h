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

#ifndef PINKTRACE_GUARD_EVENT_H
#define PINKTRACE_GUARD_EVENT_H 1

/**
 * \file
 * \brief Pink's event handling
 *
 * \ingroup g_event
 **/

#include <stdbool.h>

#include <pinktrace/gcc.h>

/**
 * Event constants
 *
 * \ingroup g_event
 *
 * \note Some of these events are only available on Linux.
 **/
typedef enum {
	/** Child has been stopped **/
	PINK_EVENT_STOP,
	/** Child has been trapped **/
	PINK_EVENT_TRAP,
	/**
	 * Child has entered/exited a system call
	 *
	 * \note Availability: Linux
	 **/
	PINK_EVENT_SYSCALL,
	/**
	 * Child has called fork()
	 *
	 * \note Availability: Linux
	 **/
	PINK_EVENT_FORK,
	/**
	 * Child has called vfork()
	 *
	 * \note Availability: Linux
	 **/
	PINK_EVENT_VFORK,
	/**
	 * Child has called clone()
	 *
	 * \note Availability: Linux
	 **/
	PINK_EVENT_CLONE,
	/**
	 * Child has exited a vfork() call
	 *
	 * \note Availability: Linux
	 **/
	PINK_EVENT_VFORK_DONE,
	/**
	 * Child has called execve()
	 *
	 * \note Availability: Linux
	 **/
	PINK_EVENT_EXEC,
	/**
	 * Child is exiting (ptrace way, stopped before exit)
	 *
	 * \note Availability: Linux
	 **/
	PINK_EVENT_EXIT,
	/** Child has received a genuine signal **/
	PINK_EVENT_GENUINE,
	/** Child has exited normally **/
	PINK_EVENT_EXIT_GENUINE,
	/** Child has been terminated with a signal **/
	PINK_EVENT_EXIT_SIGNAL,
	/** Unknown event, shouldn't happen **/
	PINK_EVENT_UNKNOWN,
} pink_event_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Return the last event made by child.
 *
 * \ingroup g_event
 *
 * \note On FreeBSD, this function gives only limited information about the
 * event. To get more information use pink_trace_lwpinfo()
 *
 * \param status The status argument, received from wait(2) system call.
 *
 * \return One of PINK_EVENT_* constants
 **/
pink_event_t
pink_event_decide(int status) PINK_GCC_ATTR((pure));

/**
 * Return a string representation of the event.
 *
 * \ingroup g_event
 *
 * \param event The event to be stringified.
 *
 * \return The string representation of the event.
 **/
const char *
pink_event_name(pink_event_t event) PINK_GCC_ATTR((pure));

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !PINKTRACE_GUARD_EVENT_H */
