#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void print_buffer(){
  int size = get_plog_size();
  long c_time;
  long t_time;

  printf("Buffer is size %d\n", size);

  for(int i=0; i<size; i++){
    get_plog_byindex(i, &c_time, &t_time);
    printf("INDEX: %d, START: %ld END: %ld\n", i, c_time, t_time);
  }
}

void print_by_pid(int pids[]){
  long c_time;
  long t_time;
  for(int i=0; i<20; i++){
    int result = get_plog_byPID(pids[i], &c_time, &t_time);
    if(result == 2){
      printf("PID: %d not found.\n", pids[i]);
    }else{
      printf("PID: %d, START: %ld END: %ld\n", pids[i], c_time, t_time);
    }
  }
}

void request_plog_out_of_index(){
  int size = get_plog_size();
  long c_time;
  long t_time;

  printf("Buffer is size %d\n", size);
  int result = get_plog_byindex(size, &c_time, &t_time);
  printf("Index out of bound should give result 3. Result is %d\n", result);
  result = get_plog_byindex(-1, &c_time, &t_time);
  printf("Index out of bound should give result 3. Result is %d\n", result);
}

void cleanup(){
  stop_plog();
  reset_plog();
}


void create_processes(int log){

  // Turn logging on or off.
  if(log){
    start_plog();
  }else{
    stop_plog();
  }

  // Create a bunch of processes.
  int pids[20];
  for(int i=0; i<20; i++){
    pids[i] = fork();
    sleep(1);
    // If this is the child process, terminate after 1 second.
    if(pids[i] == 0){
      exit(0);
    }
  }

  // Wait for the child processes to terminate
  sleep(1);

  // Display the buffer
  printf("DISPLAYING BUFFER\n");
  print_buffer();

  printf("GETTING ITEMS BY PID\n");
  print_by_pid(pids);

  printf("SIZE IS %d\n", get_plog_size());

  // clear the BUFFER
  reset_plog();

  printf("SIZE IS NOW: %d\n", get_plog_size());
  print_by_pid(pids);

  // Check the status code given by an index out of bound
  request_plog_out_of_index();
  cleanup();
}

int main(int argc, char **argv) {
  // If any arguemnts are provided, a test will be ran without ever turning on logging. This is just a sanity
  // check to ensure that there are no processes being loaded into the buffer.
  if(argc == 1){
    create_processes(1);
  }else{
    create_processes(0);
  }
}
