#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <time.h>


#define SYMB_COUNT 5

bool  flag1=false,
      flag2=false;

timespec  t1,
          t2;

static pthread_mutex_t mutex;

void *f1(void*);
void *f2(void*);



int main()
{
  std::cout << "Program start" << std::endl;
  pthread_t p1,p2;
  pthread_mutex_init(&mutex,nullptr);
  std::cout << "Waiting for input" << std::endl;
  pthread_create(&p1,nullptr,f1,nullptr);
  pthread_create(&p2,nullptr,f2,nullptr);
  getchar();
  std::cout << "Input received" << std::endl;
  flag1=true;
  flag2=true;
  pthread_join(p1,nullptr);
  pthread_join(p2,nullptr);
  pthread_mutex_destroy(&mutex);
  std::cout << "\nProgram finish" << std::endl;
  return 0;
}

void *f1(void*)
{
  std::cout << "\nthread 1 start" << std::endl;
  while(!flag1)
  {
    clock_gettime(CLOCK_REALTIME,&t1);
    t1.tv_sec+=5;
    if (pthread_mutex_timedlock(&mutex,&t1)!=ETIMEDOUT)
    {
      for (size_t i = 0; i < SYMB_COUNT; i++)
      {
        putchar('1');
        fflush(stdout);
        sleep(1);
      }
      pthread_mutex_unlock(&mutex);
    }
    sleep(2);
  }
  std::cout << "\nthread 1 finish" << std::endl;
  pthread_exit(nullptr);
}

void *f2(void*)
{
  std::cout << "\nthread 2 start" << std::endl;
  while(!flag2)
  {
    clock_gettime(CLOCK_REALTIME,&t2);
    t2.tv_sec+=5;
    if (pthread_mutex_timedlock(&mutex,&t2)!=ETIMEDOUT)
    {
      for (size_t i = 0; i < SYMB_COUNT; i++)
      {
        putchar('2');
        fflush(stdout);
        sleep(1);
      }
      pthread_mutex_unlock(&mutex);
    }
    sleep(2);
  }
  std::cout << "\nthread 2 finish" << std::endl;
  pthread_exit(nullptr);
}