#include "syslib.h"

int sys_plog_state(mpid, state)
pid_t mpid;
int state;
{
  message m;
  m.m2_i1 = mpid;
  m.m2_i2 = state;

  _kernel_call(SYS_PLOG_STATE, &m);
  return m.m2_i3; // Result is stored here.
}
