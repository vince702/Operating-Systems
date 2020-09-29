//Vincent Chi 304576879 vincentchi9702@gmail.com
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include <sched.h>

int exit_status = 0;
int c;
int option_index;


int num_threads = 1;
int num_iters = 1;

int opt_yield;

char sync_arg;

int sync_type = 0;
long long total_run_time = 0;
long long average_run_time = 0;

long long num_ops ;

struct timespec starting_time;
struct timespec end_time;

/*
SYNC_TYPE:
0 = none
1 = mutex
2 = spinlock
3 = compare and swap
*/


long long counter  = 0;


pthread_mutex_t lock;
int spinlock_flag = 0;


void add(long long *pointer, long long value) {
  long long sum = *pointer + value;
  if (opt_yield)
    sched_yield();
  *pointer = sum;
}

void mutex_add(long long *pointer, long long value) {

  pthread_mutex_lock(&lock);

  long long sum = *pointer + value;

  if (opt_yield)
    sched_yield();


  *pointer = sum;

  pthread_mutex_unlock(&lock);

}

void spinlock_add(long long *pointer, long long value) {

  while (__sync_lock_test_and_set(&spinlock_flag, 1)){ 
    continue; 
  }
         
  long long sum = *pointer + value;

  if (opt_yield)
    sched_yield();


  *pointer = sum;

  __sync_lock_release(&spinlock_flag);

}


void compare_swap_add(long long *pointer, long long value) {
  int new;
  int old;

  old = *pointer;
  new = old + value; 

  while(__sync_val_compare_and_swap(pointer, old, new) != old){
    old = *pointer;

    if (opt_yield)
      sched_yield();

    new = old + value; 

  }  

}




void multithread_add(long long value ){

  for (int i = 0; i < num_iters; i++) {
    //fprintf(stdout, "adding 1  %i \n",num_iters);

    if (sync_type == 0) add(&counter, value);

    else if (sync_type == 1) mutex_add(&counter, value);

    else if (sync_type == 2) spinlock_add(&counter, value);

    else if (sync_type == 3) compare_swap_add(&counter, value);

    else exit(1);

  }

}

void* function(){

  multithread_add(1);
  multithread_add(-1);

  return 0;
}


int main(int argc, char *argv[]) {
    
  //  long options
  struct option long_options[] = {
    {"threads",     required_argument, 0, 't'},
    {"iterations",  required_argument, 0, 'i'},
    {"sync",        required_argument, 0, 's'},
    {"yield",       no_argument,       0, 'y'},
    {0,             0,                 0, 0}
  };
    
  while( (c = getopt_long(argc, argv, "", long_options, &option_index)) != -1){

    switch(c){
    case 't':
      num_threads = atoi(optarg);
      break;
    case 'i':
      num_iters = atoi(optarg);
      break;
    case 's':

      sync_arg = optarg[0];

      if(sync_arg == 'm'){

	if (pthread_mutex_init(&lock, NULL)) {
	  fprintf(stderr,"mutex initialization error \n");
	  exit(1);
	}

	sync_type = 1;
      }

      else if(sync_arg == 's'){
	sync_type = 2;
      }

      else if(sync_arg == 'c'){
	sync_type = 3;
      }

      else{
	fprintf(stderr, "invalid arguement for --sync: %s\n", optarg);
	exit(1);
      }

      break;

    case 'y':
      opt_yield = 1;
      break;
    default:
      exit(1);
      break;

    }

  }

  /*collect hires starting time */


  if (clock_gettime(CLOCK_MONOTONIC, &starting_time) != 0){
    fprintf(stderr, "error getting start time \n");
    exit(1);
  }



  pthread_t* tid;

  if (!(   tid = (pthread_t*) malloc(sizeof(pthread_t) * num_threads) )) {
    fprintf(stderr,"error in allocating memory for pthread \n");
    exit(1);
  }

  for ( int i = 0; i < num_threads; i++) {

    if (pthread_create(&tid[i] , NULL, function , NULL)) {

      fprintf(stderr, "error creating thread \n");
      free(tid);
      exit(1);

    }

  }

  for ( int i = 0;  i < num_threads; i++) {

    if (pthread_join(tid[i], NULL)) {

      fprintf(stderr, "error joining thread \n");
      exit(1);
    }

  }


  if (clock_gettime(CLOCK_MONOTONIC, &end_time) != 0){
    fprintf(stderr, "error getting end time \n");
    exit(1);
  }


  if (sync_type == 0){
    if (opt_yield == 1) fprintf(stdout,"add-yield-none,");
    else
      fprintf(stdout,"add-none,");

  }
  if (sync_type == 1){
    if (opt_yield == 1) fprintf(stdout,"add-yield-m,");
    else
      fprintf(stdout,"add-m,");
  }
  if (sync_type == 2){
    if (opt_yield == 1) fprintf(stdout,"add-yield-s,");
    else
      fprintf(stdout,"add-s,");
  }
  if (sync_type == 3){
    if (opt_yield == 1) fprintf(stdout,"add-yield-c,");
    else
      fprintf(stdout,"add-c,");
  }


  total_run_time = ( (long long) end_time.tv_sec - (long long) starting_time.tv_sec) * 1000000000 ; //secs to nanosecs
  total_run_time += ( (long long) end_time.tv_nsec - (long long) starting_time.tv_nsec); //rest of nanosecs

  num_ops =  num_threads * num_iters * 2;

  average_run_time = total_run_time / num_ops;

  fprintf(stdout,"%i,%i,%lld,%lld,%lld,%lld\n", num_threads, num_iters, num_ops, total_run_time, average_run_time, *&counter);

  return 0;

}
