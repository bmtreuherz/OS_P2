#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
  // All this tests is that the library funcitons can be called.
  // Should see some console logging.
  start_plog();
  start_plog();
  stop_plog();
  stop_plog();
  reset_plog();

  int size = get_plog_size();
  printf("This should be 25: %d\n", size);

  int pid = 3;
  long c_time;
  long t_time;

  get_plog_bypid(pid, &c_time, &t_time);
  printf("This should be 100: %ld, and this should be 200: %ld\n", c_time, t_time);

  get_plog_byindex(pid, &c_time, &t_time);
  printf("This should be 78: %ld, and this should be 189: %ld\n", c_time, t_time);
}
