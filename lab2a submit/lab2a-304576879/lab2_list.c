//Vincent Chi 304576879 vincentchi9702@gmail.com   
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include <sched.h>
#include "SortedList.h"
#include <string.h>

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

long long counter  = 0;

pthread_mutex_t lock;
int spinlock_flag = 0;

SortedList_t *LIST; 
SortedListElement_t **ELEMENTS;
int num_elements;

int *thread_numbers;

int INSERT = 0;
int DELETE = 0;
int LOOKUP = 0;


void segmentationFaultHandler(){
  fprintf(stderr, "segmentation fault caught \n");
  exit(2);

}
//random key generator
char *randkey(size_t length) {

  static char alphanum[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!123456789";  

  char *key = NULL;

    
  key = malloc(sizeof(char) * (length +1));

            
  for (int n = 0;n < (int)(length) ;n++) {            
    int index = rand() % (int)( sizeof(alphanum) -1);

    key[n] = alphanum[index];
  }

  key[length] = '\0';
    
  return key;

}


void* function(void *tid){

  int thread_num = * ((int *) tid); 
  num_elements = num_iters * num_threads;

  for ( int i = thread_num; i < num_elements; i += num_threads) {

  if (sync_type == 2) {
    while (__sync_lock_test_and_set(&spinlock_flag, 1)){ 
      continue; 
    }
  }

  if (sync_type == 1) {
    pthread_mutex_lock(&lock);
  }

  //each thread will insert numelements/numthreads elements into list




    SortedList_insert( LIST, ELEMENTS[i] );

  
  if (sync_type == 1) {
    pthread_mutex_unlock(&lock);
  }

  if (sync_type == 2) {
    __sync_lock_release(&spinlock_flag);
  }

  }

  if (sync_type == 2) {
    while (__sync_lock_test_and_set(&spinlock_flag, 1)){
      continue;
    }
  }

  if (sync_type == 1) {
    pthread_mutex_lock(&lock);
  }

  int length = SortedList_length(LIST);

  if (sync_type == 1) {
    pthread_mutex_unlock(&lock);
  }

  if (sync_type == 2) {
    __sync_lock_release(&spinlock_flag);
  }


  if (length < 0){
    fprintf(stderr, "error list corrupted \n");
    exit(2);
  }


  for ( int i = thread_num; i < num_elements; i += num_threads) {
  if (sync_type == 2) {
    while (__sync_lock_test_and_set(&spinlock_flag, 1)){
      continue;
    }
  }

  if (sync_type == 1) {
    pthread_mutex_lock(&lock);
  }

  //lookup and remove each previously inserted element by this thread


    SortedListElement_t* to_delete;

    to_delete = SortedList_lookup(LIST, ELEMENTS[i]->key);

    if (to_delete == NULL) {
      fprintf(stderr, "deleting element that does not exist \n");
      exit(2);   
    }

    if (SortedList_delete(to_delete) == 1) {  
      fprintf(stderr,"cannot delete due to corrupted list \n");
      exit(2);
    }

  

  
  if (sync_type == 1) {
    pthread_mutex_unlock(&lock);
  }

  if (sync_type == 2) {
    __sync_lock_release(&spinlock_flag);
  }

  }

  return NULL;
}






int main(int argc, char *argv[]) {
  signal(SIGSEGV, segmentationFaultHandler);
    
  //  long options
  struct option long_options[] = {
    {"threads",     required_argument, 0, 't'},
    {"iterations",  required_argument, 0, 'i'},
    {"sync",        required_argument, 0, 's'},
    {"yield",       required_argument, 0, 'y'},
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
      /*
	else if(sync_arg == 'c'){
	sync_type = 3;
	}
      */

      else{
	fprintf(stderr, "invalid arguement for --sync: %s\n", optarg);
	exit(1);
      }

      break;

    case 'y':

      for(int i = 0; i < (int)(strlen(optarg)); i++){

	if (optarg[i] == 'i'){
	  opt_yield |= INSERT_YIELD;
	  INSERT = 1;
	}
	else if (optarg[i] == 'd'){
	  opt_yield |= DELETE_YIELD;
	  DELETE = 1;
	}
	else if (optarg[i] == 'l'){
	  opt_yield |= LOOKUP_YIELD;
	  LOOKUP = 1;
	}
	else{
	  fprintf(stderr, "invalid arguement for --yield");
	  exit(1);
	}

      }

      break;


    default:
      exit(1);
      break;

    }

  }


  num_elements = num_threads * num_iters;

  LIST = malloc(sizeof(SortedList_t));
  LIST->prev = LIST;
  LIST->next = LIST;

  ELEMENTS = malloc( num_elements * sizeof(SortedListElement_t*) );

  for ( int i = 0; i < num_elements; i++ ) {

    ELEMENTS[i] = malloc(sizeof(SortedListElement_t));
    ELEMENTS[i]->key = randkey(25);
    ELEMENTS[i]->prev = NULL;
    ELEMENTS[i]->next = NULL;
  }

  if (clock_gettime(CLOCK_MONOTONIC, &starting_time) != 0){
    fprintf(stderr, "error getting start time \n");
    exit(1);
  }



  pthread_t* tid;
  thread_numbers = malloc(sizeof(int) * num_threads);

  if (!(   tid = (pthread_t*) malloc(sizeof(pthread_t) * num_threads) )) {
    fprintf(stderr,"error in allocating memory for pthread \n");
    exit(1);
  }

  for ( int i = 0; i < num_threads; i++) {

    thread_numbers[i] = i;

    if (pthread_create(&tid[i] , NULL, function , (void *) &thread_numbers[i])) {

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

  if (SortedList_length(LIST) != 0) {
    fprintf(stdout,"length of list is nonzero \n");
    exit(2);
  }

  free(ELEMENTS);
  free(LIST);

  fprintf(stdout,"list-");

  if (INSERT==0 && DELETE == 0 && LOOKUP == 0){
    fprintf(stdout, "none");
  }

  if (INSERT == 1){
    fprintf(stdout,"i");
  }
  if (DELETE == 1){
    fprintf(stdout,"d");
  }
  if (LOOKUP == 1){
    fprintf(stdout,"l");
  }

  if (sync_type == 0){
    fprintf(stdout, "-none,");
  }
  if (sync_type == 1){
    fprintf(stdout, "-m,");
  }
  if (sync_type == 2){
    fprintf(stdout, "-s,");
  }




  total_run_time = ( (long long) end_time.tv_sec - (long long) starting_time.tv_sec) * 1000000000 ; //secs to nanosecs
  total_run_time += ( (long long) end_time.tv_nsec - (long long) starting_time.tv_nsec); //rest of nanosecs

  num_ops =  num_threads * num_iters * 3;

  average_run_time = total_run_time / num_ops;

  fprintf(stdout,"%i,%i,%i,%lld,%lld,%lld\n", num_threads, num_iters,1, num_ops, total_run_time, average_run_time);

  exit(0);


}
