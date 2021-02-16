#include <iostream>
#include <pthread.h>
#include <unistd.h>

typedef struct
{
  bool flag;
  char symb;
} args;

void *f1(void *a);
void *f2(void *a);

int main()
{
  args a1,a2;
  a1.flag = false;
  a2.flag = false;
  a1.symb = '1';
  a2.symb = '2';
  int   e_val1,
        e_val2;
  pthread_t p1, p2;
  pthread_create(&p1,nullptr,f1,&a1);
  pthread_create(&p2,nullptr,f2,&a2);
  std::cin.get();
  a1.flag=true;
  a2.flag=true;
  pthread_join(p1,(void **)&e_val1);
  pthread_join(p2,(void **)&e_val2);
  std::cout << "1 thread exit code: " << e_val1 << std::endl;
  std::cout << "2 thread exit code: " << e_val2 << std::endl;
  return 0;
}


void *f1(void *a)
{
  args *arg = (args *) a;
  while(! arg->flag)
  {
    std::cout << arg->symb << std::flush;
    sleep(1);
  }
  pthread_exit((void*)11);
}

void *f2(void *a)
{
  args *arg = (args *) a;
  while(! arg->flag)
  {
    std::cout << arg->symb << std::flush;
    sleep(2);
  }
  pthread_exit((void*)22);
}
