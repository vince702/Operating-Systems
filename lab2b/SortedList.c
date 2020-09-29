//Vincent Chi 304576879 vincentchi9702@gmail.com   
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include <sched.h>
#include "SortedList.h"

int opt_yield;

void SortedList_insert(SortedList_t *list, SortedListElement_t *element){

  if (element == NULL || list == NULL){
    return;
  }

  SortedListElement_t *iterator = list->next;

  while(iterator != list){

    if ( strcmp( iterator->key, element->key) < 0){ 
      break; 
    }

    iterator = iterator->next;
  }
  //critical section when inserting, yield the cpu to move to end of run queue
  if ( opt_yield & INSERT_YIELD) {
    sched_yield();
  }

  element->prev = iterator->prev;
  element->next = iterator;
  iterator->prev->next = element;
  iterator->prev = element;


}


int SortedList_delete( SortedListElement_t *element){
  //list corruption means return 1
  if ( element == NULL || element->prev->next != element || element->next->prev != element){
    return 1; 
  }

  else{
    //critical section when deleting, yield the cpu to move to end of run queue
    if ( opt_yield & INSERT_YIELD) {
      sched_yield();
    }

    element->prev->next = element->next;
    element->next->prev = element->prev;
    free(element);

    return 0;
  }

}


SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){
  if (list == NULL ){
    return NULL;
  }

  SortedListElement_t *iterator = list->next;

  while (iterator != list) {

    if (  strcmp(key, iterator->key) == 0 ){
      return iterator; 
    }
    //yield so we're not reporting an element that has already been deleted, or reporting null when it's been inserted
    if ( opt_yield & LOOKUP_YIELD ){
      sched_yield();
    }

    iterator = iterator->next;

  }

  return NULL;

}


int SortedList_length(SortedList_t *list) {

  int length = 0;

  SortedListElement_t *iterator = list->next;  

  //critical section when counting list, don't want it to change as we're counting 

  if ( opt_yield & LOOKUP_YIELD) {
    sched_yield();
  }


  while (iterator != list) {
    // check if list corrupted
    if (    ( iterator->prev->next != iterator) || ( iterator->next->prev != iterator)   ) {
      return -1;  
    }

    iterator = iterator->next;
    length += 1;

  }

  return length;

}


