#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void DumpBacktrace() {
  pid_t dying_pid = getpid();
  pid_t child_pid = fork();
  if (child_pid < 0) {
    perror("fork() while collecting backtrace:");
  } else if (child_pid == 0) {
    char buf[1024];
    sprintf(buf, "gdb -p %d -batch -ex bt 2>/dev/null | "
            "sed '0,/<signal handler/d'", dying_pid);
    const char* argv[] = {"sh", "-c", buf, NULL};
    execve("/bin/sh", (char**)argv, NULL);
    _exit(1);
  } else {
    waitpid(child_pid, NULL, 0);
  }
  _exit(1);
}

void BacktraceOnSegv() {
  struct sigaction action = {};
  action.sa_handler = DumpBacktrace;
  if (sigaction(SIGSEGV, &action, NULL) < 0) {
    perror("sigaction(SEGV)");
  }
}



