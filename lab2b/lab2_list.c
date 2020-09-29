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
pthread_mutex_t *mutex_locklist;
int *spinlock_locklist;

SortedList_t *LIST; 
SortedListElement_t **ELEMENTS;
SortedList_t **SUBLIST;


int num_elements;
int num_lists = 1;

int *thread_numbers;

int INSERT = 0;
int DELETE = 0;
int LOOKUP = 0;

long long total_wait_time = 0;
long num_waits = 0;

void segmentationFaultHandler(){
  fprintf(stderr, "segmentation fault caught \n");
  exit(2);

}


//**************************************************************************************************************
//https://piazza.com/class/jqmoqdgs2ch1ae?cid=386 
//djb2hash http://www.cse.yorku.ca/~oz/hash.html
    unsigned long
    hash(const  char *str)
    {

      unsigned long hash = 5381;
      int c;

      while ((c = *str++))
	hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

      return hash;
    }
//**************************************************************************************************************

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

int length = 0;
void* function(void *tid){

  int thread_num = * ((int *) tid); 
  num_elements = num_iters * num_threads;
  int sublist_num; 

  long long total_wait_thread = 0;
  long num_waits_thread = 0;

  struct timespec start_wait;
  struct timespec end_wait;

  //INSERTIONS  //****** ************ ************ ************ ******
  for ( int i = thread_num; i < num_elements; i += num_threads) {

    //Determine which sublist to insert to
   
    sublist_num = hash(ELEMENTS[i]->key);
    sublist_num = abs(sublist_num % num_lists);
    //fprintf(stdout, "%i \n", sublist_num);

    if (sync_type == 2) {

      if (clock_gettime(CLOCK_MONOTONIC, &start_wait) != 0){
	fprintf(stderr, "error getting start time \n");
	exit(1);
      }
      while (__sync_lock_test_and_set(&spinlock_locklist[sublist_num], 1)){ 
	continue; 
      }
      if (clock_gettime(CLOCK_MONOTONIC, &end_wait) != 0){
	fprintf(stderr, "error getting start time \n");
	exit(1);
      }


      //get wait time for lock
      total_wait_thread += ( (long long) end_wait.tv_sec - (long long)  start_wait.tv_sec) * 1000000000 ; //secs to nanosecs
      total_wait_thread += ( (long long) end_wait.tv_nsec - (long long)  start_wait.tv_nsec);
      num_waits_thread += 1;
    }

    if (sync_type == 1) {
      if (clock_gettime(CLOCK_MONOTONIC, &start_wait) != 0){
	fprintf(stderr, "error getting start time \n");
	exit(1);
      }
      pthread_mutex_lock(&mutex_locklist[sublist_num]);
      if (clock_gettime(CLOCK_MONOTONIC, &end_wait) != 0){
	fprintf(stderr, "error getting start time \n");
	exit(1);
      }

      //get wait time for lock
      total_wait_thread += ( (long long) end_wait.tv_sec - (long long)  start_wait.tv_sec) * 1000000000 ; //secs to nanosecs
      total_wait_thread += ( (long long) end_wait.tv_nsec - (long long)  start_wait.tv_nsec);
      num_waits_thread += 1;
    }

    //each thread will insert numelements/numthreads elements into list

    SortedList_insert( SUBLIST[sublist_num], ELEMENTS[i] );


    
    if (sync_type == 1) {
      //pthread_mutex_unlock(&lock);
      pthread_mutex_unlock(&mutex_locklist[sublist_num]);
    }



    if (sync_type == 2) {
      __sync_lock_release(&spinlock_locklist[sublist_num]);
    }

  }
  //****** ************ ************ ************ ******



  //************ ****** get length ************ ******
  if (sync_type == 2) {
    if (clock_gettime(CLOCK_MONOTONIC, &start_wait) != 0){
      fprintf(stderr, "error getting start time \n");
      exit(1);
    }

    while (__sync_lock_test_and_set(&spinlock_locklist[sublist_num], 1)){
      continue;
    }
    if (clock_gettime(CLOCK_MONOTONIC, &end_wait) != 0){
      fprintf(stderr, "error getting start time \n");
      exit(1);
    }
    //get wait time for lock
    total_wait_thread += ( (long long) end_wait.tv_sec - (long long)  start_wait.tv_sec) * 1000000000 ; //secs to nanosecs
    total_wait_thread += ( (long long) end_wait.tv_nsec - (long long)  start_wait.tv_nsec);
    num_waits_thread += 1;

  }

  if (sync_type == 1) {
    //pthread_mutex_lock(&lock);
    if (clock_gettime(CLOCK_MONOTONIC, &start_wait) != 0){
      fprintf(stderr, "error getting start time \n");
      exit(1);
    }
    pthread_mutex_lock(&mutex_locklist[sublist_num]);
    if (clock_gettime(CLOCK_MONOTONIC, &end_wait) != 0){
      fprintf(stderr, "error getting start time \n");
      exit(1);
    }
    //get wait time for lock
    total_wait_thread += ( (long long) end_wait.tv_sec - (long long)  start_wait.tv_sec) * 1000000000 ; //secs to nanosecs
    total_wait_thread += ( (long long) end_wait.tv_nsec - (long long)  start_wait.tv_nsec);
    num_waits_thread += 1;
  }

  length = SortedList_length(SUBLIST[sublist_num]);
  if (length < 0){
    fprintf(stderr, "error list corrupted \n");
    exit(2);
  }

  if (sync_type == 1) {
    //pthread_mutex_unlock(&lock);
    pthread_mutex_unlock(&mutex_locklist[sublist_num]);

  }

  if (sync_type == 2) {
    __sync_lock_release(&spinlock_locklist[sublist_num]);
  }


  //****** ************ ************ ************ ******



  

  //DELETIONS  //****** ************ ************ ************ ******
  for ( int i = thread_num; i < num_elements; i += num_threads) {

    sublist_num = hash(ELEMENTS[i]->key);
    sublist_num = abs(sublist_num % num_lists);

    if (sync_type == 2) {
      if (clock_gettime(CLOCK_MONOTONIC, &start_wait) != 0){
	fprintf(stderr, "error getting start time \n");
	exit(1);
      }
      while (__sync_lock_test_and_set(&spinlock_locklist[sublist_num], 1)){
	continue;
      }
      if (clock_gettime(CLOCK_MONOTONIC, &end_wait) != 0){
	fprintf(stderr, "error getting start time \n");
	exit(1);
      }
      //get wait time for lock
      total_wait_thread += ( (long long) end_wait.tv_sec - (long long)  start_wait.tv_sec) * 1000000000 ; //secs to nanosecs
      total_wait_thread += ( (long long) end_wait.tv_nsec - (long long)  start_wait.tv_nsec);
      num_waits_thread += 1;
    }

    if (sync_type == 1) {
      //pthread_mutex_lock(&lock);
      if (clock_gettime(CLOCK_MONOTONIC, &start_wait) != 0){
	fprintf(stderr, "error getting start time \n");
	exit(1);
      }
      pthread_mutex_lock(&mutex_locklist[sublist_num]);
      if (clock_gettime(CLOCK_MONOTONIC, &end_wait) != 0){
	fprintf(stderr, "error getting start time \n");
	exit(1);
      }
      //get wait time for lock
      total_wait_thread += ( (long long) end_wait.tv_sec - (long long)  start_wait.tv_sec) * 1000000000 ; //secs to nanosecs
      total_wait_thread += ( (long long) end_wait.tv_nsec - (long long)  start_wait.tv_nsec);
      num_waits_thread += 1;
    }

    //lookup and remove each previously inserted element by this thread


    SortedListElement_t* to_delete;

    to_delete = SortedList_lookup(SUBLIST[sublist_num], ELEMENTS[i]->key);

    if (to_delete == NULL) {
      fprintf(stderr, "deleting element that does not exist \n");
      exit(2);   
    }

    if (SortedList_delete(to_delete) == 1) {  
      fprintf(stderr,"cannot delete due to corrupted list \n");
      exit(2);
    }

  

  
    if (sync_type == 1) {
      //pthread_mutex_unlock(&lock);
      pthread_mutex_unlock(&mutex_locklist[sublist_num]);

    }

    if (sync_type == 2) {
      __sync_lock_release(&spinlock_locklist[sublist_num]);
    }

  }


  pthread_mutex_lock(&lock);
  total_wait_time += total_wait_thread;
  num_waits += num_waits_thread;
  pthread_mutex_unlock(&lock);

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
    {"lists",       required_argument, 0, 'l'},
    {0,             0,                 0, 0}
  };
    
  while( (c = getopt_long(argc, argv, "", long_options, &option_index)) != -1){

    switch(c){
    case 'l':
      num_lists = atoi(optarg);
      break;
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



  //**************************** create sublists and corresponding lock *******************************************
  SUBLIST = malloc( num_lists * sizeof(SortedList_t) );
  mutex_locklist = malloc( num_lists * sizeof(pthread_mutex_t) );
  spinlock_locklist = malloc( num_lists * sizeof(int) );


  for (int i = 0; i < num_lists; i++){
    
    if (sync_type == 1){
      if (pthread_mutex_init(&mutex_locklist[i], NULL)) {
	fprintf(stderr,"mutex initialization error \n");
	exit(1);
      }
      //fprintf(stdout, "creating a mutex for sublist \n");
    }

    if (sync_type == 2){
      spinlock_locklist[i] = 0;
      //fprintf(stdout, "creating a spinlock for sublist \n");
    }

    SUBLIST[i] = malloc(sizeof(SortedList_t*));
    SUBLIST[i]->prev = SUBLIST[i];
    SUBLIST[i]->next = SUBLIST[i];
    //fprintf(stdout, "creating sublist \n");

  }
  //**************************************************************************************************************



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


  //iterate thru sublists to get lengths

  for ( int i = 0; i < num_lists; i++){

    if (SortedList_length( SUBLIST[i]) != 0) {
      fprintf(stdout,"length of list is nonzero \n");
      exit(2);
    }

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

  fprintf(stdout,"%i,%i,%i,%lld,%lld,%lld,", num_threads, num_iters,num_lists, num_ops, total_run_time, average_run_time);

  if (num_waits == 0) num_waits = 1;
  fprintf(stdout, "%lld \n", total_wait_time/num_waits);
  exit(0);


}
