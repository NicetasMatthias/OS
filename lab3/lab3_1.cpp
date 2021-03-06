#include <iostream>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

bool  flag1 = false,
      flag2 = false;

int fields[2];

void *f1(void *);
void *f2(void *);

int main()
{
  int e_code;
  pthread_t p1,p2;
  e_code = pipe(fields);
  if (e_code)
  {
    perror("pipe create error: ");
  }
  std::cout << "Waiting for input" << std::endl;
  pthread_create(&p1,nullptr,f1,nullptr);
  pthread_create(&p2,nullptr,f2,nullptr);
  std::cin.get();
  std::cout << "Input received" << std::endl;
  flag1=true;
  flag2=true;
  pthread_join(p1,nullptr);
  pthread_join(p2,nullptr);

  close(fields[0]);
  close(fields[1]);
  std::cout << "\nProgram finish" << std::endl;
  return 0;
}

void *f1(void *)
{
  std::cout << "\nthread 1 start" << std::endl;
  char buffer[256]={0};
  socklen_t size;
  struct sockaddr_in sa;
  unsigned int sa_len = sizeof(sa);
  int sockfd = socket(AF_INET,SOCK_STREAM, 0);
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  bind (sockfd,(struct sockaddr*)&sa,sizeof(sa));
  while(!flag1)
  {
    if (getsockname(sockfd, (struct sockaddr*)&sa, &sa_len))
    {
      perror("getsockname error");
    }
    else
    {
    sprintf(buffer, "Family: %d Port: %d Addr: %d", sa.sin_family, sa.sin_port ,sa.sin_addr.s_addr);
    write(fields[1], buffer, sizeof(buffer));
    }
    sleep(4);
  }
  close(sockfd);
  std::cout << "\nthread 1 finish" << std::endl;
  pthread_exit(nullptr);
}

void *f2(void *)
{
  std::cout << "\nthread 2 start" << std::endl;
  char buffer[256]={0};
  while(!flag2)
  {
    memset(buffer,0,sizeof(buffer));
    read(fields[0],buffer,sizeof(buffer));
    std::cout << buffer << std::endl;
  }
  std::cout << "\nthread 2 finish" << std::endl;
  pthread_exit(nullptr);
}
