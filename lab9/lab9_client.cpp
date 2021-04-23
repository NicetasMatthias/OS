#include <iostream>
#include <pthread.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

int workSocket;

bool connection_flag = false;
bool transmission_flag = false;
bool reception_flag = false;

pthread_t connection_p;
pthread_t transmission_p;
pthread_t reception_p;

struct sockaddr_in workSockAddr;

//поток отправки запросов
void *transmission(void *)
{
  int count = 1;
  char sndbuf[256];
  while(!transmission_flag)
  {
    sleep(1);
    int len = sprintf(sndbuf,"request №%d",count);
    int sentcount = send(workSocket,sndbuf,len,0);
    if (sentcount == -1)
    {
      perror("# send error");
    }
    else
    {
		    std::cout << std::endl << "TRANSMITED: " << std::endl << sndbuf << std::endl;
        count++;
    }
  }
  pthread_exit((void*)0);
}

//поток приема ответов
void *reception(void *)
{
  char rcvbuf[256];
  while(!reception_flag)
  {
    memset(rcvbuf,0,256);
    int reccount = recv(workSocket,rcvbuf,256,0);
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
      std::cout << std::endl << "RECIVED:" << std::endl << rcvbuf << std::endl;
    }
  }
  pthread_exit((void*)0);
}

//поток установления соединения
void *connection(void *)
{

    struct sockaddr_in checkSockAddr;
    socklen_t size = sizeof(checkSockAddr);

  while(!connection_flag)
  {
    int result = connect(workSocket,(struct sockaddr*)&workSockAddr,sizeof(workSockAddr));
    if (result != -1)
    {
      perror("# connect error");
      sleep(2);
    }
    else
    {
      getsockname(workSocket, (struct sockaddr*)&checkSockAddr, &size);


      std::cout << "connection established" << std::endl;
      std::cout << "\nme:" << "\naddr: " << checkSockAddr.sin_addr.s_addr << "\nport: " << checkSockAddr.sin_port << std::endl;
      std::cout << "\nserver:" << "\naddr: " << workSockAddr.sin_addr.s_addr << "\nport: " << workSockAddr.sin_port << std::endl << std::endl;
      pthread_create(&transmission_p,nullptr,transmission,nullptr);
      pthread_create(&reception_p,nullptr,reception,nullptr);
      pthread_exit((void*)0);
    }
  }
  pthread_exit((void*)0);
}

int main()
{
  int err_code;
  workSocket = socket(AF_INET,SOCK_STREAM,0);
  fcntl(workSocket,F_SETFL,O_NONBLOCK);
  workSockAddr.sin_family = AF_INET;
  workSockAddr.sin_port = htons(2000);
  workSockAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  pthread_create(&connection_p,nullptr,connection,nullptr);

  std::cin.get();
  reception_flag = true;
  transmission_flag = true;
  connection_flag = true;

  pthread_join(connection_p,nullptr);
  err_code = pthread_join(reception_p,nullptr);
  if (err_code)
  {
    std::cout << "reception pthread_join() error: " << strerror(err_code) << std::endl;
  }
  err_code = pthread_join(transmission_p,nullptr);
  if (err_code)
  {
    std::cout << "transmission pthread_join() error: " << strerror(err_code) << std::endl;
  }

  shutdown(workSocket,2);
  close(workSocket);

  return 0;
}
