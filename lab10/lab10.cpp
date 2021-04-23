#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

static char child_stack[1024*1024];

static int childFunc(void *arg)
{

    std::cout << "Дочерний: id текущего процесса: " << getpid() << std::endl
      << "Дочерний: id родительского процесса: " << getppid() << std::endl;
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    pid_t child_pid;
    child_pid = clone(childFunc, child_stack + (1024*1024), CLONE_NEWPID | SIGCHLD, NULL);
    if (child_pid==-1)
    {
      perror("clone error:");
    }
    std::cout << "Родительский: id текущего процесса: " << getpid() << std::endl
      << "Родительский: id родительского процесса: " << getppid() << std::endl
      << "Родительский: id дочернего процесса: " << child_pid << std::endl;
    waitpid(child_pid, NULL, 0);
    exit(EXIT_SUCCESS);
}
