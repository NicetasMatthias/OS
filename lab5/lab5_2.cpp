#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <fstream>
#include <semaphore.h>
#include <fcntl.h>

bool flag=false;
sem_t *st;
std::ofstream f;
char s_name[] = "Sanya";

void *func(void *)
{
  char s = '2';
  while(!flag)
  {
    sem_wait(st);
    for (int i = 0; i<5; i++)
    {
      std::cout << s << std::flush;
      f << s << std::flush;
      sleep(1);
    }
    sem_post(st);
    sleep(1);
  }
  pthread_exit((void*)0);
}

int main()
{
  int e_val;
  st = sem_open(s_name, O_CREAT, 0644, 1);
  pthread_t p;
  pthread_create(&p,nullptr,func,nullptr);
  f.open("data",std::ios::out|std::ios::app);
  std::cin.get();
  flag = true;
  pthread_join(p,(void **)&e_val);
  sem_close(st);
  sem_unlink(s_name);
  f.close();
  return 0;
}
