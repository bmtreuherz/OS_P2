#include <lib.h>
#include <unistd.h>

void start_plog(){
  message m;
  _syscall(PM_PROC_NR, STARTPLOG, &m);
}

void stop_plog(){
  message m;
  _syscall(PM_PROC_NR, STOPPLOG, &m);
}

void reset_plog(){
  message m;
  _syscall(PM_PROC_NR, RESETPLOG, &m);
}

int get_plog_size(){
  message m;
  _syscall(PM_PROC_NR, GETPLOGSIZE, &m);

  return m.m1_i1;
}

int get_plog_byPID(int pid, long *c_time, long *t_time){
  message m;
  m.m2_i1 = pid;

  int status = _syscall(PM_PROC_NR, GETPLOGBYPID, &m);

  // Set the values for c_time and t_time (should be set in the syscall)
  *c_time = m.m2_l1;
  *t_time = m.m2_l2;

  return status;
}

int get_plog_byindex(int index, long *c_time, long *t_time){
  message m;
  m.m2_i1 = index;

  int status = _syscall(PM_PROC_NR, GETPLOGBYINDEX, &m);

  // Set the values for c_time and t_time (should be set in the syscall)
  *c_time = m.m2_l1;
  *t_time = m.m2_l2;

  return status;
}

int plog_state_start(int pid){
  message m;
  m.m2_i1 = pid;

  return _syscall(PM_PROC_NR, PLOGSTATESTART, &m);
}

int plog_state_stop(int pid){
  message m;
  m.m2_i1 = pid;

  return _syscall(PM_PROC_NR, PLOGSTATESTOP, &m);
}
