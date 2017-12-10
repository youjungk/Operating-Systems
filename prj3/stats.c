#include "stats.h"
#include <stdbool.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

//global variables
factory_data* factories;
int numfact = 0;

void stats_init(int num_producers){
	if (num_producers<=0){
		return;
	}
	numfact = num_producers;

	factories = (factory_data*)malloc(sizeof(factory_data)*num_producers);

	for (int i=0; i< num_producers; i++){
		factories[i].factory_number = i;
		factories[i].num_made = 0;
		factories[i].num_eaten = 0;
		factories[i].min_delay = DBL_MAX;
		factories[i].avg_delay = 0;
		factories[i].max_delay = 0;
	}
}

void stats_cleanup(void){
	free(factories);
	factories = NULL;
	numfact =0;
}

void stats_record_produced(int factory_number){
	if (factory_number < 0 || factory_number >= numfact || factories == NULL){
		printf("no such factory \n");
		return;
	}
	for (int i=0; i < numfact; i++){
		if (factories[i].factory_number == factory_number){
			factories[i].num_made++;
			return;
		}
	}
}

void stats_record_consumed(int factory_number, double delay_in_ms){
	if (factory_number < 0 || factory_number >= numfact|| factories == NULL){
		printf("no such factory \n");
		return;
	}
	if (delay_in_ms < 0){
		printf("delay_in_ms cannot be negative\n");
    	return;
	}
	for (int i=0; i < numfact; i++){
		if (factories[i].factory_number == factory_number){
			if (delay_in_ms < factories[i].min_delay){
				factories[i].min_delay = delay_in_ms;
			}
			if (delay_in_ms > factories[i].max_delay){
				factories[i].max_delay = delay_in_ms;
			}
			factories[i].avg_delay *= factories[i].num_eaten;
			factories[i].avg_delay += delay_in_ms;
			factories[i].num_eaten++;
			factories[i].avg_delay /=factories[i].num_eaten;
			return;
		}
	}
}

void stats_display(void){
	if (numfact < 1 || factories == NULL){
    return;
	}

	int error_factories[numfact];
	int num_error =0;

 	printf("Statistics:\n\n");
 	printf("%-8s %-6s %-6s %-13s %-13s %-13s\n", "Factory#", "#Made", "#Eaten", "Min_Delay[ms]", "Avg_Delay[ms]", "Max_Delay[ms]");	

 	for (int i=0;i<numfact;i++){
		printf("%-8d %-6d %-6d ", factories[i].factory_number, factories[i].num_made, factories[i].num_eaten);
		printf("%-13.5f %-13.5f %-13.5f\n", factories[i].min_delay, factories[i].avg_delay, factories[i].max_delay);
 	

	 	if (factories[i].num_made != factories[i].num_eaten){
	 		error_factories[num_error] = factories[i].factory_number;
	 		num_error++;
	 	}
	}
 	for(int j=0; j< num_error;j++){
 		printf("ERROR: Mismatch between number made and eaten. %d\n", error_factories[j]);
 	}

}