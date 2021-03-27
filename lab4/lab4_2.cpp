#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>


int main()
{
  int status;
  pid_t pid = fork();
  if (pid == -1)
  {
    std::cout << "Ошибка создания дочернего процесса" << std::endl;
  }
  else if (pid == 0)
  {
    execl("lab4_1","1 аргумент", "2 аргумент", "3 аргумент", nullptr);
  }
  else
  {
    std::cout << "Родительский: id текущего процесса: " << getpid() << std::endl;
    std::cout << "Родительский: id родительского процесса: " << getppid() << std::endl;
    std::cout << "Родительский: id дочернего процесса: " << pid << std::endl;
    while(waitpid(pid,&status,WNOHANG)==0)
    {
      std::cout << "Родительский: ожидаем завершения дочернего" << std::endl;
      sleep(1);
    }
    if (WIFEXITED(status))
    {
      std::cout << "Код завершения дочернего процесса: " << WEXITSTATUS(status) <<  std::endl;
    }
    else
    {
      std::cout << "Дочерний процесс завершился с ошибкой" << std::endl;
    }
  }
  return 0;
}
