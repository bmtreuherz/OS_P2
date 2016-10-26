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


// PLOG State tracking
typedef enum {NEW, CRTD, RDY, RUN, BLK, TERM} state;

typedef struct{
  pid_t i_pid;
  int timestamp;
  state from_state;
  state to_state;
} plog_state_item;
