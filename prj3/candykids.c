#include "bbuff.h"
#include "stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <inttypes.h>

//Global variables/mutex/semas
pthread_mutex_t mutex;
sem_t haveData, haveSpace;
_Bool thread_flag = false;

//function declaration for main
double current_time_in_ms(void);
void* factory_thr_function(void* fac_num);
void* kid_thr_function(void* kid_num);

/*
• // 1. Extract arguments
• // 2. Initialize modules
• // 3. Launch candy-factory threads
• // 4. Launch kid threads
• // 5. Wait for requested time
• // 6. Stop candy-factory threads
• // 7. Wait until no more candy
• // 8. Stop kid threads
• // 9. Print statistics
• // 10. Cleanup any allocated memory
• }
*/
int main (int argc, char* argv[]){
	/*1. Extract arguments*/
	if (argc <= 3){
		printf("Please enter 3 arguments, <#Factories>, <#Kids>, <#Seconds> (e.g. './candykids 3 1 10')\n");
        exit(1);
	}

	int factories_num, kids_num, seconds_num; 

	for (int i=1; i < argc; i++){
		int temp = atoi(argv[i]);
		if (temp <= 0) {
			printf("The arguments have to be greater than 0\n");
			exit (1);
		}
		else{
			if (i==1){factories_num = temp;}
			else if (i==2){kids_num = temp;}
			else {seconds_num = temp;}
		}
	}

	/*2. Initialize modules
	- bounded buffer, and statistics
	*/
	bbuff_init();
	stats_init(factories_num);

	/*3. Launch candy-factory threads
	--store the thread ids in an array
	*/
	pthread_t factory_tid[factories_num];
	int factory_arr[factories_num];
	for (int j=0; j < factories_num; j++){
		factory_arr[j] = j;
		pthread_create(&factory_tid[j], NULL, factory_thr_function, &factory_arr[j]);
	}

	/*4. Launch kid threads*/
	pthread_t kid_tid[kids_num];
	for (int k=0; k < kids_num; k++){
		pthread_create(&kid_tid[k], NULL, kid_thr_function, NULL);
	}

	/*5. Wait for request time*/
	for (int l=0; l < seconds_num; l++){
		sleep(1);
		printf("\n");
		printf("        Time %ds\n", l+1);
	}
	  printf("\n");

	/*6. Stop factory threads*/
	printf("Signalling candy factory threads to stop\n\n");

	thread_flag = true;

	for (int m=0; m < factories_num; m++){
		pthread_cancel(factory_tid[m]);
		pthread_join(factory_tid[m], NULL);
	}

	/*7. Wait until no more candy*/
	while (!bbuff_is_empty()){
    	printf("Waiting for all candy to be consumed\n");
    	sleep(1);
  	}

	/*8. Stop kids thread*/
  	for (int n=0; n < kids_num; n++){
  		pthread_cancel(kid_tid[n]);
  		pthread_join(kid_tid[n], NULL);
  	}

  	/*9. Display statistics */
  	stats_display();
 
  	/*10. Clean up any allocated */
  	stats_cleanup();
  	exit(1);
}

//Time functinos and thread functions
double current_time_in_ms(void){
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	return (double)(now.tv_sec*1000.0 + now.tv_nsec/1000000.0);
}

/*
1. Loop until flag is true
2. Pick a number of seconds which it will (later) wait. Number randomly
selected between 0 and 3 inclusive.
3. Print a message such as: “\tFactory 0 ships candy & waits 2s”
4. Dynamically allocate a new candy item and populate its fields.
5. Add the candy item to the bounded buffer.
6. Sleep for number of seconds identified in #1
7. When the thread finishes, print that it is done
*/
void* factory_thr_function(void* fac_num){
	int factory_number = *((int*)fac_num);
	// 1. Loop until flag is true
	while (!thread_flag){
		//2. Pick a number of seconds which it will (later) wait. Number randomly
		int wait_time = (rand() % 4);



		//3. Print a message such as: “\tFactory 0 ships candy & waits 2s”
		printf("\tFactory %d ships candy & waits %ds\n", factory_number, wait_time);

		//4. Dynamically allocate a new candy item and populate its fields.
		candy_t* candy = (candy_t*) malloc(sizeof(candy_t));
		candy->factory_number = factory_number;
		candy->time_stamp_in_ms = current_time_in_ms();

		//5.
		bbuff_blocking_insert(candy);
		stats_record_produced(factory_number);
		//6. Sleep for number of seconds identified in #1

		sleep(wait_time); 
	}
	//7. When the thread finishes, print that it is done
	printf("Candy-factory %d done\n", factory_number);
    return fac_num;
}

/*
1. Loop forever
2. Extract a candy item from the bounded buffer.
▪ This will block until there is a candy item to extract.
3. Process the item. Initially you may just want to printf() it to the screen;
in the next section you must add a statistics module that will track what
candies have been eaten.
4. Sleep for either 0 or 1 seconds (randomly selected).
*/
void* kid_thr_function(void* kid_num){
	candy_t* candy = NULL;
	double time_flag = 0;
	//1.
	while (true){
		int wait_time = (rand()%2);
		//2-4
		candy = bbuff_blocking_extract();
		time_flag = current_time_in_ms () - candy->time_stamp_in_ms;
		if (time_flag < 0){
			time_flag *= -1;
		}
		stats_record_consumed(candy->factory_number, time_flag);
		free(candy);
		candy = NULL;
		//4
		sleep (wait_time);
	}
	pthread_exit(NULL);
	return kid_num;
}