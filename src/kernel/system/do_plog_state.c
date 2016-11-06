#include "kernel/system.h"
#include <minix/endpoint.h>

/*===========================================================================*
  *                                do_sample                                  *
  *===========================================================================*/
int do_plog_state(struct proc *caller_ptr, message *m_ptr)
{
  pid_t mpid = m2_i1;
  int state = m2_i2;
  int result = 1;
  register struct proc *rmp;

  if(mpid == 0){
    // If the PID is 0, set the tracking state for all processes in the table
    for (rmp = &proc[0]; rmp < &proc[NR_TASKS + NR_PROCS]; rmp++){
      rmp->is_tracked = state;
    }
    result = 0;
  }else{
    for (rmp = &proc[0]; rmp < &proc[NR_TASKS + NR_PROCS]; rmp++){
      if(rmp->p_id == mpid){
        if(isuserp(rmp)){
          rmp->is_tracked = state;
          result = 0;
        }else{
          result = 2;
        }
        break;
      }
    }
  }

  // TODO: REMOVE AFTER DEBUGGING
  printf("KERNAL TABLE\n")
  for (rmp = &proc[0]; rmp < &proc[NR_TASKS + NR_PROCS]; rmp++){
    prinf("PID: %d\tTRACKED: %d\n", rmp->p_id, rmp->is_tracked);
  }

  m_ptr->m2_i3 = result;
  return(OK);
}
