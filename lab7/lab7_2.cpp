#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <filesystem>
#include <sys/stat.h>
#include <fcntl.h>

bool flag = false;
int fd;
char fname[]="/tmp/Sanya";



void *func(void *)
{
  int err_c;
  char buffer[256]={0};

  while(!flag)
  {
    if ((err_c=read(fd,buffer,sizeof(buffer)))==-1)
    {
      perror("read error:");
    }
    else if(err_c==0)
    {
      std::cout<< "pipe is empty" << std::endl;
    }
    else
    {
      std::cout << buffer << std::endl;
    }

    sleep(2);
  }
  pthread_exit((void*)0);
}


int main()
{
  pthread_t p;
  mkfifo(fname, 0644);
  fd = open(fname,O_RDONLY|O_NONBLOCK);
  pthread_create(&p,nullptr,func,nullptr);
  std::cin.get();
  flag = true;
  pthread_join(p,nullptr);
  close(fd);
  unlink(fname);
}
