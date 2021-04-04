#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <csignal>
#include <filesystem>
#include <sys/stat.h>
#include <fcntl.h>


pthread_t p,pf;
bool flag = false;
int fd;
char fname[]="/tmp/Sanya";

void *func(void *);


void sig_handler(int signo)
{
  std::cout << "SIGPIPE" << std::endl;
}

void *func_fopen(void *)
{
  while(!flag)
  {
    if ((fd = open(fname,O_WRONLY|O_NONBLOCK))==-1)
    {
      perror("file open error");
      sleep(2);
    }
    else
    {
        pthread_create(&p,nullptr,func,nullptr);
        pthread_exit((void*)0);
    }

  }
  pthread_exit((void*)0);

}

void *func(void *)
{
  char buffer[256]={0};
  struct sockaddr_in sa;
  unsigned int sa_len = sizeof(sa);
  int sockfd = socket(AF_INET,SOCK_STREAM, 0);
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  bind (sockfd,(struct sockaddr*)&sa,sizeof(sa));

  while(!flag)
  {
    if (getsockname(sockfd, (struct sockaddr*)&sa, &sa_len))
    {
      perror("getsockname error");
    }
    else
    {
      sprintf(buffer, "Family: %d Port: %d Addr: %d", sa.sin_family, sa.sin_port ,sa.sin_addr.s_addr);
      if ((write(fd,buffer,sizeof(buffer)))==-1)
      {
        perror("write error");
      }
      else
      {
        std::cout << buffer << std::endl;
      }
    }
    sleep(2);
  }
  pthread_exit((void*)0);
}


int main()
{
  int err_c;
  signal(SIGPIPE,sig_handler);
  pthread_create(&pf,nullptr,func_fopen,nullptr);

  std::cin.get();
  flag = true;
  pthread_join(pf,nullptr);
  err_c = pthread_join(p,nullptr);
  if (err_c)
  {
    std::cout << "func error code = " << err_c << std::endl;
  }
  close(fd);
  unlink(fname);
}
