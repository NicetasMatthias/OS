#include <iostream>
#include <pthread.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
  std::cout << "Дочерний: id текущего процесса: " << getpid() << std::endl;
  std::cout << "Дочерний: id родительского процесса: " << getppid() << std::endl;

  for (int i = 0; i<sizeof(argv)-1; i++)
  {
    std::cout << argv[i] << std::endl;
    sleep(1);
  }

  exit(66);
  return 0;
}
