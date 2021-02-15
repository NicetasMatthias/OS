#include <iostream>
#include <pthread.h>
#include <unistd.h>
//#include <stdio.h>

void *f1(void *flag);
void *f2(void *flag);

int main()
{
  bool  flag1=false,
        flag2=false;
  int   e_val1,
        e_val2;
  pthread_t p1, p2;
  pthread_create(&p1,nullptr,f1,&flag1);
  pthread_create(&p2,nullptr,f2,&flag2);
  getchar();
  flag1=true;
  flag2=true;
  pthread_join(p1,(void **)&e_val1);
  pthread_join(p2,(void **)&e_val2);
  std::cout << "1 thread exit code: " << e_val1 << std::endl;
  std::cout << "2 thread exit code: " << e_val2 << std::endl;
  return 0;
}


void *f1(void *flag)
{
  while(! *(bool *)flag)
  {
    std::cout << "1\n";
    sleep(1);
  }
  pthread_exit((void*)11);
}

void *f2(void *flag)
{
  while(! *(bool *)flag)
  {
    std::cout << "2\n";
    sleep(2);
  }
  pthread_exit((void*)22);
}
