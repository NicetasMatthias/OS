#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <mqueue.h>
#include <netinet/in.h>


bool flag=false;
char name[] = "/Sanya";
mqd_t qd;

void *func(void *)
{
  int err_c;
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
      if ((err_c = mq_send(qd,buffer,sizeof(buffer),0)) == -1)
      {
        perror("mq_send error");
      }
      else
      {
        std::cout << buffer << std::endl;
      }

    }
    sleep(1);
  }
  pthread_exit((void*)0);
}

int main()
{
  int e_val;
  pthread_t p;
  mq_attr attr;
  attr.mq_maxmsg = 15;
  attr.mq_msgsize = 256;
  if ((qd = mq_open(name,O_CREAT|O_WRONLY|O_NONBLOCK, 0644, &attr))==-1)
  {
    perror("mq_open error");
    return 1;
  }
  pthread_create(&p,nullptr,func,nullptr);

  std::cin.get();
  flag = true;
  pthread_join(p,(void **)&e_val);
  mq_close(qd);
  mq_unlink(name);
  return 0;
}
