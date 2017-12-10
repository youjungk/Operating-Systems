#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

double current_time_in_ms(void){
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  return (double)(now.tv_sec*1000.0 + now.tv_nsec/1000000.0);
}

void minimal_function (void);
void* toggle_shared_num(void* emptyptr);

/* Global Variables */
int shared_num = 0; 
int N = 100000;

pthread_t tid[2]; 
pthread_mutex_t mutex;


int main()
{
  /* Declare Variables */
  int err;
  pid_t pid[2];
  int i = 0;
  int fd1[2];//file descriptors for pipe 1 */
  int fd2[2];//file descriptors for pipe 2 */
  char msg = 'a'; 
  char readBuffer; 
  double avg_func = 0;
  double avg_sys = 0;
  double avg_process = 0;
  double avg_thread = 0;
  double start_time, end_time;


  //1
  start_time = current_time_in_ms();
  
  for (i = 0; i < N; i++){
    minimal_function();
  }

  end_time = current_time_in_ms ();

  avg_func = (end_time-start_time)/N;
  if (avg_func < 0){
    avg_func *= -1;
  }
  printf("Average time of minimal function call: %.10f ms\n", avg_func); 
  // end of 1


  //2
  start_time = current_time_in_ms ();
  
  for (i = 0; i < N; i++){
    pid[0] = getpid();
  }

  end_time = current_time_in_ms ();

  avg_sys = (end_time-start_time)/N;
  if (avg_sys < 0){
    avg_sys *= -1;
  }
  printf("Average time of minimal system call: %.10f ms\n", avg_sys);
  //end of 2

  //3
  pipe(fd1); 
  pipe(fd2); 
  /* 3b. */
  start_time= current_time_in_ms ();
  /* 3c. */
  if ((pid[1] = fork()) == -1){
    perror("could not fork process.\n");
    exit(1);
  }
  /* 3d. */
  if (pid[1] == 0){
    close(fd1[1]);
    close(fd2[0]);

    for (i = 0; i < N; i++){
      read(fd1[0], &readBuffer, 1);
      write(fd2[1], &msg, 1);
    }
    exit(0);
  }
  else{

    close(fd1[0]);
    close(fd2[1]);
    for (i = 0; i < N; i++){
      write(fd1[1], &msg, 1);
      read(fd2[0], &readBuffer, 1);
    }
  }
  //3
  end_time= current_time_in_ms ();
  
  avg_process = (end_time - start_time)/N;
  if (avg_process < 0){
    avg_process *= -1;
  }
  printf("Average time of process switching: %.10f ms\n", avg_process);
  // end of 3
  //4
  start_time= current_time_in_ms ();
  
  err = pthread_create(&(tid[0]), NULL, toggle_shared_num, NULL);
  if (err != 0){
    perror("Error: could not create thread\n");
    exit(1);
  }
  err = pthread_create(&(tid[1]), NULL, toggle_shared_num, NULL);
  if (err != 0){
    perror("Error: could not create thread\n");
    exit(1);
  }
 
  pthread_join(tid[0], NULL);
  pthread_join(tid[1], NULL);
 
  end_time= current_time_in_ms ();
 
  avg_thread = (end_time-start_time)/N;
  if (avg_thread < 0){
    avg_thread *= -1;
  }
  printf("Average time of thread switching: %.10f ms\n", avg_thread);
  
//end of 4
  return 0;
}

void minimal_function(void){return;}

void* toggle_shared_num(void* emptyptr)
{
  int i;

  pthread_t threadID = pthread_self();

  if (threadID == tid[0]){
    for (i = 0; i < N; i++){
      while (shared_num != 1){

      }
      //critical section
      pthread_mutex_lock(&mutex);
      shared_num--;
      pthread_mutex_unlock(&mutex);
    
    }
  }

  if (threadID == tid[1]){
    for (i = 0; i < N; i++){
      while (shared_num != 0){
      }
      //critical section
      pthread_mutex_lock(&mutex);
      shared_num++;
      pthread_mutex_unlock(&mutex);
  
    }
  }
  return emptyptr;
}
