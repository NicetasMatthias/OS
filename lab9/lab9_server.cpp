#include <iostream>
#include <pthread.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <string.h>

int listenSocket;
int workSocket;
std::vector <std::string> requests;
std::vector <std::string> answers;
bool reception_flag = false;
bool processing_flag = false;
bool transmission_flag = false;
bool waiting_flag = false;

static pthread_mutex_t mutex_r;
static pthread_mutex_t mutex_a;

struct sockaddr_in workSockAddr;
struct sockaddr_in checkSockAddr;

pthread_t reception_p;
pthread_t processing_p;
pthread_t transmission_p;
pthread_t waiting_p;

//поток приема запросов
void *reception(void *)
{
  char rcvbuf[256];
  while(!reception_flag)
  {
    int reccount = recv(workSocket,(void*)rcvbuf,256,0);
    if (reccount == -1)
    {
      perror("# recv error");
      sleep(1);
    }
    else if (reccount == 0)
    {
      std::cout << "!disconnect" << std::endl;
      sleep(1);
    }
    else
    {
      pthread_mutex_lock(&mutex_r);
      requests.push_back((std::string)rcvbuf);
      pthread_mutex_unlock(&mutex_r);
    }
  }
  pthread_exit((void*)0);
}

//поток обработки запросов
void *processing(void *)
{
  struct sockaddr_in sa;
  socklen_t sa_len=sizeof(sa);
  char anwbuf[256];
  while(!processing_flag)
  {
    pthread_mutex_lock(&mutex_r);
    if(!requests.empty())
    {
      std::string s = requests.back();
      requests.pop_back();
      pthread_mutex_unlock(&mutex_r);
      if (getsockname(workSocket, (struct sockaddr*)&sa, &sa_len))
      {
        perror("# getsockname error");
      }
      else
      {
        sprintf(anwbuf,"Request: %s\nAnswer: Family = %d, Port = %d, Addr = %d",s.c_str(),sa.sin_family, sa.sin_port ,sa.sin_addr.s_addr);
        pthread_mutex_lock(&mutex_a);
        answers.push_back(anwbuf);
        pthread_mutex_unlock(&mutex_a);
      }
    }
    else
    {
      pthread_mutex_unlock(&mutex_r);
      sleep(1);
    }
  }
  pthread_exit((void*)0);
}

//поток передачи ответов
void *transmission(void *)
{
  char sndbuf[256];
  while(!transmission_flag)
  {
    pthread_mutex_lock(&mutex_a);
    if(!answers.empty())
    {
      std::string s = answers.back();
      answers.pop_back();
      pthread_mutex_unlock(&mutex_a);
      strcpy(sndbuf,s.c_str());
      int sentcount = send(workSocket,sndbuf,sizeof(sndbuf),0);
      if (sentcount == -1)
      {
        perror("# send error");
      }
      else
      {
        std::cout << sndbuf << std::endl << std::endl;
      }
    }
    else
    {
      pthread_mutex_unlock(&mutex_a);
      sleep(1);
    }
  }
  pthread_exit((void*)0);
}

//поток ожидания соединения
void *waiting(void *)
{
  struct sockaddr_in checkSockAddr;

  socklen_t size = sizeof(workSockAddr);
  while(!waiting_flag)
  {
    workSocket = accept(listenSocket,(struct sockaddr*)&workSockAddr,&size);
    if (workSocket == -1)
    {
      perror("# accept error");
      sleep(1);
    }
    else
    {
        getsockname(workSocket, (struct sockaddr*)&checkSockAddr, &size);

        std::cout << "connection established" << std::endl;
        std::cout << "\nme:" << "\naddr: " << checkSockAddr.sin_addr.s_addr << "\nport: " << checkSockAddr.sin_port << std::endl;
        std::cout << "\nclient:" << "\naddr: " << workSockAddr.sin_addr.s_addr << "\nport: " << workSockAddr.sin_port << std::endl << std::endl;
        pthread_create(&reception_p,nullptr,reception,nullptr);
        pthread_create(&processing_p,nullptr,processing,nullptr);
        pthread_create(&transmission_p,nullptr,transmission,nullptr);
        pthread_exit((void*)0);
    }
  }
  pthread_exit((void*)0);
}

int main()
{
  int err_code;
  struct sockaddr_in listenSockAddr;

  listenSocket = socket(AF_INET,SOCK_STREAM,0);
  fcntl(listenSocket,F_SETFL,O_NONBLOCK);
  listenSockAddr.sin_family = AF_INET;
  listenSockAddr.sin_port = htons(2000);
  listenSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  bind(listenSocket,(struct sockaddr*)&listenSockAddr,sizeof(listenSockAddr));
  listen(listenSocket,SOMAXCONN);


  pthread_mutex_init(&mutex_r,nullptr);
  pthread_mutex_init(&mutex_a,nullptr);

  pthread_create(&waiting_p,nullptr,waiting,nullptr);
  std::cin.get();
  reception_flag = true;
  processing_flag = true;
  transmission_flag = true;
  waiting_flag = true;

  pthread_join(waiting_p,nullptr);
  err_code = pthread_join(reception_p,nullptr);
  if (err_code)
  {
    std::cout << "reception pthread_join() error: " << strerror(err_code) << std::endl;
  }
  err_code = pthread_join(processing_p,nullptr);
  if (err_code)
  {
    std::cout << "processing pthread_join() error: " << strerror(err_code) << std::endl;
  }
  err_code = pthread_join(transmission_p,nullptr);
  if (err_code)
  {
    std::cout << "transmission pthread_join() error: " << strerror(err_code) << std::endl;
  }

  pthread_mutex_destroy(&mutex_r);
  pthread_mutex_destroy(&mutex_a);
  shutdown(workSocket,2);

  close(listenSocket);
  close(workSocket);

  return 0;
}
