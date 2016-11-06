#include "syslib.h"

int sys_plog_state(mpid, state)
pid_t mpid;
int state;
{
  message m;
  m.m2_i1 = mpid;
  m.m2_i2 = state;

  // TODO: Make a kernal call here brah.
  printf("Kernal call would be made here bruh!\n");
  return 0;
}
