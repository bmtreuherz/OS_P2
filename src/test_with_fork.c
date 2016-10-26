#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
  // When forking:
  // 0 is returned to child
  // A pid is returned to the parent

  stop_plog();
  // Plog off, should not get called
  int pid = fork();

  if (!pid) {
    printf("%s\n", "exiting first child process" );
    exit(0);
  }

  // Verify log is still empty
  printf("%s%d\n\n", "- Process log should be empty\n- Process log size is: ", get_plog_size());

  // Make a bunch of processes
  int pids[42] = { 0 };
  printf("%s\n", "- Creating 50 processes. Please hold.");
  start_plog();
  for (int i = 0; i < 50; i++) {
    // Only let parent make more kids.
    pid = fork();
    sleep(2);
    if (!pid) {
      // printf("im a child preocess exiting \n");
      exit(0);
    }
    pids[i] = pid;

  }

  printf("%s%d\n\n", "- Process log size is: ", get_plog_size());
  long start;
  long end;


  sleep(3);
  // Print out info for the processes i created
  for (int i = 0; i < 50; i ++) {
    get_plog_byPID(pids[i], &start, &end);
    //get_plog_byindex(2, &start, &end);
    printf("%s%d%s%ld%s%ld\n\n", "- Process w/ PID:", pids[i] , ", start time: ", start, " End time: ", end);
  }

  // Try to get info from buffer with invalid pid
  printf("%s%d\n", "Geting plog by PID with invalid pid. Return code:  ", get_plog_byPID(19902, &start, &end) );

  printf("%s%d\n", "Geting plog by index with negative index. Return code:  ", get_plog_byPID(-12, &start, &end) );

  printf("%s%d\n", "Geting plog by index with out of bounds index. Return code:  ", get_plog_byPID(100, &start, &end));

  // Reset and verify
  printf("- Reset the plog!\n\n");
  reset_plog();
  printf("%s%d\n\n", "- Process log size is: ", get_plog_size());

  // Turn off logging
  stop_plog();

  return 0;
}
