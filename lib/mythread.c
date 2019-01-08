#include <stdio.h>
#include <ucontext.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <setjmp.h>

#include "mythread.h"

static enum mythread_scheduling_policy sched_policy = MYTHREAD_RR;

sigjmp_buf jmp;
sigjmp_buf jmp2;

// Thread control block
typedef struct tcb {
  // TODO: Fill your own code
  void (*stub)(void(*a)(void *),void* args,int n_tcbs);
  ucontext_t *context;
  int thread_id;
  int finished; // 0이면 끝나지 않음, 1이면 끝남.
}Job;

static struct tcb* tcbs[1024];
static int n_tcbs = 0;
static int current_thread_id = 0;
static struct itimerval time_quantum;
static struct sigaction ticker;

int a = 0;

int tim = 2;

void fun(void (*stub)(void *), void* args, int n_tcbs){
   stub(args);
   tcbs[n_tcbs]->finished = 1;
}
static int nextTcb() {
  if (sched_policy == MYTHREAD_RR) {
    // TODO: Fill your own code
    sigsetjmp(jmp,1);
    if(current_thread_id == n_tcbs){
       current_thread_id = 0;

       alarm(tim);

       swapcontext(tcbs[n_tcbs]->context,tcbs[0]->context);
    }
    else{
       current_thread_id++;
       if((tcbs[current_thread_id]->finished) == 1){
          siglongjmp(jmp,1);
       }
       alarm(tim);
       swapcontext(tcbs[current_thread_id-1]->context,tcbs[current_thread_id]->context);
    }
  } else if (sched_policy == MYTHREAD_FIFO) {
    // TODO: Fill your own code
  }
}

static void tick(int sig) {
  // TODO: Implement your own code
  nextTcb();
  // HINT: 이 함수에서 nextTcb() 함수를 통해 scheduling 하고, 해당 schedule 함수를 실행.
}

void mythread_init(enum mythread_scheduling_policy policy)
{
  sched_policy = policy;

  // TODO: Implement your own code
  
  // HINT: 이 함수에서 tick 에 대해서 timer signal를...
  ticker.sa_handler = tick;
  sigemptyset(&ticker.sa_mask);
  ticker.sa_flags = 0;
  sigaction(SIGALRM, &ticker, 0);

  tcbs[0] = (Job*)malloc(sizeof(Job));
  tcbs[0] -> context = (ucontext_t *)malloc(sizeof(ucontext_t));
  getcontext(tcbs[0]->context);

  if(sched_policy == 0)
    printf("Scheduling : Round Robin\n");
  else
    printf("Scheduling : FIFO\n");
}

int mythread_create(void (*stub)(void*), void* args)
{
  int tid = -1; // Thread ID

  // TODO: Implement your own code
  n_tcbs++;
  tcbs[n_tcbs] = (Job*)malloc(sizeof(Job));
  tid = n_tcbs;
  tcbs[n_tcbs]->thread_id = n_tcbs;
  tcbs[n_tcbs]->stub = fun;
  tcbs[n_tcbs]->context = (ucontext_t *)malloc(sizeof(ucontext_t));
  getcontext(tcbs[n_tcbs]->context);
  tcbs[n_tcbs]->context->uc_stack.ss_sp = malloc(10000);
  tcbs[n_tcbs]->context->uc_stack.ss_size = 10000;
  tcbs[n_tcbs]->context->uc_link = tcbs[n_tcbs-1]->context;
  makecontext(tcbs[n_tcbs]->context,(void(*)(void))(tcbs[n_tcbs]->stub), 3, stub, args, n_tcbs);

  if(sched_policy == MYTHREAD_RR){
    current_thread_id = n_tcbs;
    alarm(tim);
    swapcontext(tcbs[0]->context, tcbs[current_thread_id]->context);
  }
  else{
    current_thread_id = n_tcbs;
    (stub)(args);
    tcbs[n_tcbs]->finished = 1;
  }

  return tid;
}

void mythread_join(int tid)
{
  // TODO: Implement your own code
  int b = 1;
  printf("Waiting thread %d\n",tid);

  while(tcbs[tid]->finished==0){
  }
  printf("Thread %d Finished\n", tid);
}

