#include <stdio.h>
#include <signal.h>
#include <semaphore.h>
#include <ucontext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mythread.h"

void f(void* args)
{
  for(int i=65; i<75; i++){
    printf("f : %c\n", i);
    sleep(1);
  }
}

void g(void* args)
{
  const int n = *((int*)args);

  for (int i = 1; i < n+1; i++) {
    printf("g : %d\n", i);
    sleep(1);
  }
}

int main()
{
  int sch_type;
  int n;
  printf("Select Scheduling Type(1.FIFO, 2.RR) *Enter 1 Or 2 : ");
  scanf("%d", &sch_type);
  printf("Enter the number of n (ex: 8): ");
  scanf("%d", &n);
  if(sch_type == 1)
    mythread_init(MYTHREAD_FIFO);
  else
    mythread_init(MYTHREAD_RR);

  int tid1 = mythread_create(f, NULL);

  int tid2 = mythread_create(g, &n);
  int a = mythread_create(f, NULL);
  int b = mythread_create(g, &n);

  mythread_join(tid1);
  mythread_join(tid2);

  mythread_join(a);
  mythread_join(b);

  return 0;
}
