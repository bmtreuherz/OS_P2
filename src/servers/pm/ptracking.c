#include <stdio.h>
#include <minix/callnr.h>
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

int internal_plog_state_start(){
  pid_t mpid = m_in.m2_i1;
  return start_tracking(mpid);
}

int internal_plog_state_stop(){
  pid_t mpid = m_in.m2_i1;
  return stop_tracking(mpid);
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
      result = internal_plog_state_start();
      break;
    case PLOGSTATESTOP:
      result = internal_plog_state_stop();
      break;
    default:
      result = -1;
      break;
  }

  return result;
}
