#include "syslib.h"

int sys_fork(parent, child, child_endpoint, flags, msgaddr, c_pid)
endpoint_t parent;		/* process doing the fork */
endpoint_t child;		/* which proc has been created by the fork */
endpoint_t *child_endpoint;
u32_t flags;
vir_bytes *msgaddr;
pid_t c_pid;
{
/* A process has forked.  Tell the kernel. */

  message m;
  int r;

  printf("LIB_SYS_FORK PID BEFORE: %d\n", c_pid);

  m.PRN_ENDPT = parent;
  m.PRN_SLOT = child;
  m.PRN_FORK_FLAGS = flags;
  m.PRN_CHILD_PID = c_pid;
  printf("LIB_SYS_FORK PID AFTER: %d\n", m.PRN_CHILD_PID);

  r = _kernel_call(SYS_FORK, &m);
  printf("RESULT IS: %d", r); 
  *child_endpoint = m.PRN_ENDPT;
  *msgaddr = (vir_bytes) m.PRN_FORK_MSGADDR;
  return r;
}
