#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <mqueue.h>


bool flag=false;
char name[] = "/Sanya";
mqd_t qd;

void *func(void *)
{
  int ret_val;
  char buffer[256]={0};
  while(!flag)
  {
    if ((ret_val = mq_receive(qd,buffer,sizeof(buffer),0)) == -1)
    {
      perror("mq_recieve error");
    }
    else
    {
      std::cout << "return value: " << ret_val << "|" << buffer << std::endl;
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
  attr.mq_maxmsg = 5;
  attr.mq_msgsize = 256;
  if ((qd = mq_open(name,O_CREAT|O_RDONLY|O_NONBLOCK, 0644, &attr))==-1)
  {
    perror("mq_open error");
  }
  pthread_create(&p,nullptr,func,nullptr);

  std::cin.get();
  flag = true;
  pthread_join(p,(void **)&e_val);
  mq_close(qd);
  mq_unlink(name);
  return 0;
}
