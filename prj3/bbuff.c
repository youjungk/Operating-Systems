#include "bbuff.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

pthread_mutex_t mutex;
sem_t empty_sem/*empty_sem*/, full_sem /*full_sem*/;

void* buffer[BUFFER_SIZE];
int counter, front_flag = 0;

void bbuff_init(void){
	if (sem_init(&full_sem, 0, 0) != 0){
		printf("Unable to initialize haveData semaphore \n");
		exit(1);
	}
	if (sem_init(&empty_sem, 0, BUFFER_SIZE) != 0){
		printf("Unable to initialize haveSpace semaphore \n");
		exit(1);
	}
	counter = 0;
	front_flag = 0;
}

void bbuff_blocking_insert(void* item){
	if (item == NULL){
    	printf("Warning: cannot insert NULL item into buffer\n");
    	return;
  	}
	if (sem_wait(&empty_sem) != 0){
		printf("sem_wait failed. \n");
		exit(1);
	}
	//critical section
	pthread_mutex_lock(&mutex);
	if (counter >= BUFFER_SIZE){
		exit(1);
	}
	buffer[(front_flag+counter)%BUFFER_SIZE] = item;
	counter++;
	pthread_mutex_unlock(&mutex);

	if (sem_post(&full_sem)!= 0){
		printf("sem_post failed. \n");
		exit(1);
	}
}

void* bbuff_blocking_extract(void){
	if (sem_wait(&full_sem) !=0){
		printf("sem_wait failed. \n");
		exit(1);
	}
	void *item;
	pthread_mutex_lock(&mutex);
	if (counter ==0){
		printf ("buffer is empty\n");
		exit(1);
	}
	item = buffer[front_flag];
	front_flag = (front_flag+1) % BUFFER_SIZE;
	counter --;

	pthread_mutex_unlock(&mutex);
	
	if (sem_post(&empty_sem) != 0){
		printf("sem_wait failed. \n");
		exit(1);
	}
	
	return item;	
}

_Bool bbuff_is_empty(void) {
	if (counter == 0){
		return true;
	}
	else{return false;}
}