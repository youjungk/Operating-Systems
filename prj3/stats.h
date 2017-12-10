#ifndef STATS_H
#define STATS_H

typedef struct {
  int factory_number;
  int num_made;
  int num_eaten;
  double min_delay; 
  double avg_delay; 
  double max_delay; 
} factory_data;

void stats_init(int num_producers);
void stats_cleanup(void);
void stats_record_produced(int factory_number);
void stats_record_consumed(int factory_number, double delay_in_ms);
void stats_display(void);

#endif