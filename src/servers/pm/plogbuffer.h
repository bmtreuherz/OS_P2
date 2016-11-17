#include "const.h"

typedef struct{
  pid_t i_pid;
  long c_time;
  long t_time;
} plog_item;

typedef struct{
  int tracking_on;
  int index;
  int full;
  plog_item items[PLOG_BUFFER_SIZE];
} plog_buffer;
