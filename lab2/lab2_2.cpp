#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string.h>


#define SYMB_COUNT 5

bool  flag1=false,
      flag2=false;

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
  std::cin.get();
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
  int err_c;
  std::cout << "\nthread 1 start" << std::endl;
  while(!flag1)
  {
    if ((err_c=pthread_mutex_trylock(&mutex))==0)
    {
      for (size_t i = 0; i < SYMB_COUNT; i++)
      {
        std::cout << '1' << std::flush;
        sleep(1);
      }
      pthread_mutex_unlock(&mutex);
    }
    else
    {
      std::cout << "\n1 thread error code: " << err_c << " - " << strerror(err_c) << std::endl;
    }
    sleep(5);
  }
  std::cout << "\nthread 1 finish" << std::endl;
  pthread_exit(nullptr);
}

void *f2(void*)
{
  int err_c;
  std::cout << "\nthread 2 start" << std::endl;
  while(!flag2)
  {
    if ((err_c=pthread_mutex_trylock(&mutex))==0)
    {
      for (size_t i = 0; i < SYMB_COUNT; i++)
      {
        std::cout << '2' << std::flush;
        sleep(1);
      }
      pthread_mutex_unlock(&mutex);
    }
    else
    {
      std::cout << "\n2 thread error code: " << err_c << " - " << strerror(err_c) << std::endl;
    }
    sleep(5);
  }
  std::cout << "\nthread 2 finish" << std::endl;
  pthread_exit(nullptr);
}
