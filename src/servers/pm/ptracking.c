#include <stdio.h>
#include <minix/callnr.h>
#include <minix/syslib.h>
#include "pm.h"
#include "mproc.h"
#include "param.h"

int internal_start_plog(){
  p_buffer.tracking_on = 1;
  return 0;
}

int internal_stop_plog(){
  p_buffer.tracking_on = 0;
  return 0;
}

int internal_reset_plog(){
  p_buffer.index = 0;
  p_buffer.full = 0;
  return 0;
}

int internal_get_plog_size(){
  m_in.m1_i1 = get_pbuffer_size();
  mp->mp_reply = m_in;
  return 0;
}

int internal_get_plog_by_pid(){
  // Get the item with this pid.
  pid_t mpid = m_in.m2_i1;
  int index;
  plog_item item = find_process_by_pid(mpid, &index);

  // If its not null, extract the start and end time from it.
  if(item.i_pid == -1){
    // not found
    return 2;
  }

  m_in.m2_l1 = item.c_time;
  m_in.m2_l2 = item.t_time;
  mp->mp_reply = m_in;

  return 0;
}

int internal_get_plog_by_index(){
  // Get the index from the message and ensure it is within range.
  int index = m_in.m2_i1;
  if(index < 0 || index >= get_pbuffer_size()){
    // index out of bounds
    return 3;
  }

  // Get the item here and extrat the information from it.
  plog_item item = p_buffer.items[index];
  m_in.m2_l1 = item.c_time;
  m_in.m2_l2 = item.t_time;
  mp->mp_reply = m_in;

  return 0;
}

int internal_plog_state(int state){
  pid_t mpid = m_in.m2_i1;
  int pm_result, k_result;

  // Modify the is_tracking for the process(es) in the PM table.
  if(mpid == 0){
    // If the PID is 0, set the tracking state for all processes in the table
    internal_set_all_state(state);
    pm_result = 0;
  }else{
    register struct mproc *rmp = find_proc(mpid);
    if(rmp != NULL){
      rmp->is_tracked = state;
      pm_result = 0;
    }else{
      pm_result = 1;
    }
  }

  // TODO: REMOVE AFTER DEBUGGING
  printf("PM PROCESS TABLE\n");
  for (rmp = &mproc[0]; rmp < &mproc[NR_PROCS]; rmp++){
    printf("PID: %d\tTRACKED: %d\n", rmp->mp_pid, rmp->is_tracked);
  }

  k_result = sys_plog_state(mpid, state);

  printf("PTRACKING: PID:%d STATE:%d PM_RES:%d K_RES:%d\n", mpid, state, pm_result, k_result);
  if(pm_result == k_result){
    return pm_result;
  }else if(pm_result == 0 && k_result != 0){
    return k_result;
  }else if(pm_result != 0 && k_result == 0){
    return pm_result;
  }else{
    // This last case will occur when the process was not found in the pm table,
    // but was found to be a non user proccess in the kernal table.
    return k_result;
  }
}

int internal_set_all_state(int state){
  register struct mproc *rmp;

  for (rmp = &mproc[0]; rmp < &mproc[NR_PROCS]; rmp++){
    rmp->is_tracked = state;
  }
}

int do_plog(){

  int result;

  switch(call_nr){
    case STARTPLOG:
      result = internal_start_plog();
      break;
    case STOPPLOG:
      result = internal_stop_plog();
      break;
    case RESETPLOG:
      result = internal_reset_plog();
      break;
    case GETPLOGSIZE:
      result = internal_get_plog_size();
      break;
    case GETPLOGBYPID:
      result = internal_get_plog_by_pid();
      break;
    case GETPLOGBYINDEX:
      result = internal_get_plog_by_index();
      break;
    case PLOGSTATESTART:
      result = internal_plog_state(1);
      break;
    case PLOGSTATESTOP:
      result = internal_plog_state(0);
      break;
    default:
      result = -1;
      break;
  }

  return result;
}
