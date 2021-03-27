#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cstring>

//эта прога передает

bool flag=false;
sem_t *s_w,*s_r;
int m_ind;
void *addr;
char m_name[] = "/SanyaMem";
char s_name_r[] = "/SanyaR";
char s_name_w[] = "/SanyaW";

void *func(void *)
{
  int data = 0;
  while(!flag)
  {
    memcpy(addr, &data, sizeof(data));
    sem_post(s_w);
    sem_wait(s_r);
    sleep(1);
    data++;
  }
  pthread_exit((void*)0);
}

int main()
{
  int e_val;
  pthread_t p;
  int mem_size = sizeof(int); //ТУТ БУДЕТ ТИП ЗНАЧЕНИЯ КОТОРОЕ БУДЕМ ПЕРЕДАВАТЬ
  s_w = sem_open(s_name_w, O_CREAT, 0644, 0);
  s_r = sem_open(s_name_r, O_CREAT, 0644, 0);
  m_ind = shm_open(m_name, O_CREAT|O_RDWR, 0644);
  ftruncate(m_ind,mem_size);
  addr = (int*)mmap(0,mem_size,PROT_WRITE|PROT_READ,MAP_SHARED,m_ind,0);
  pthread_create(&p,nullptr,func,nullptr);

  std::cin.get();
  flag = true;
  pthread_join(p,(void **)&e_val);
  sem_close(s_w);
  sem_close(s_r);
  sem_unlink(s_name_w);
  sem_unlink(s_name_r);
  munmap(addr,mem_size);
  close(m_ind);
  shm_unlink(m_name);
  return 0;
}
