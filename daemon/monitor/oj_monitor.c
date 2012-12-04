#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pwd.h>
#include <signal.h>
#include <syslog.h>

#define JUDGE_USER_NAME "judge"
#define DAEMON_PATH "/home/zhang/daemon"

#define MY_SPECIAL_CODE 135

struct passwd * user_info;
uid_t judge_uid;
pid_t cpid;
int stopped;

void signal_handle(int p)
{
	kill(-cpid, SIGKILL);
	stopped = 1;
}
int main(int argc, char *argv[])
{
	if(0 != getuid()) {
		puts("getuid() != 0");
		exit(EXIT_FAILURE);
	}
	if(!(user_info = getpwnam(JUDGE_USER_NAME))) {
		puts("Cannot find judge user");
		exit(EXIT_FAILURE);
	}
	judge_uid = user_info->pw_uid;

	if(0 == fork()) {
		int fd;
		fd = open("/var/lock/ojdaemon.lock", O_WRONLY|O_CREAT);
		if(fd==-1 || flock(fd, LOCK_EX|LOCK_NB)) {
			puts("Cannot get lock");
			exit(EXIT_FAILURE);
		}

		if(-1 == setsid()) {
			perror("setsid");
			exit(EXIT_FAILURE);
		}
		if(SIG_ERR == signal(SIGTERM, signal_handle)) {
			perror("signal");
			exit(EXIT_FAILURE);
		}

		openlog("bashu-onlinejudge", LOG_PERROR|LOG_PID, LOG_DAEMON);

		puts("Monitor started");
		for(;;) {
			cpid = fork();
			if (cpid == -1) {
				perror("fork");
				syslog(LOG_ERR, "Can't fork");
				exit(EXIT_FAILURE);
			}

			if (cpid == 0) {
				freopen("/var/log/onlinejudge.log", "a", stdout);

				if(-1 == setsid()) {
					perror("setsid");
					_exit(MY_SPECIAL_CODE);
				}

				setuid(judge_uid);
				
				if(-1 == execl(DAEMON_PATH, DAEMON_PATH, NULL)) {
					perror("Cannot execute daemon");
					_exit(MY_SPECIAL_CODE);
				}
			} else {
				pid_t w;
				int status;

				syslog(LOG_INFO, "Child PID is %ld\n", (long) cpid);
				syslog(LOG_INFO, "Set UID %ld\n", (long) judge_uid);

				w = waitpid(cpid, &status, WUNTRACED);
				if (w == -1) {
					syslog(LOG_ERR, "waitpid failed");
					perror("waitpid");
					exit(EXIT_FAILURE);
				}

				kill(-cpid, SIGKILL); //kill runner

				if (WIFEXITED(status)) {
					syslog(LOG_ERR, "exited, status=%d\n", WEXITSTATUS(status));
					if(MY_SPECIAL_CODE == WEXITSTATUS(status))
						break;
				} else if (WIFSIGNALED(status)) {
					syslog(LOG_ERR, "killed by signal %d\n", WTERMSIG(status));
				} else if (WIFSTOPPED(status)) {
					int ign;
					waitpid(cpid, &ign, 0); //avoid zombie
					syslog(LOG_ERR, "stopped by signal %d\n", WSTOPSIG(status));
				} else if (WIFCONTINUED(status)) {
					syslog(LOG_ERR, "continued\n");
				}

				if(stopped) {
					syslog(LOG_INFO, "Monitor stopped...\n");
					break;
				}
				syslog(LOG_INFO, "Sleep 2s...\n");
				sleep(2);
			}
		}
		flock(fd, LOCK_UN);
		close(fd);
	}
	return 0;
}
