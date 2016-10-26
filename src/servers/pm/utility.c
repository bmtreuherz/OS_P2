/* This file contains some utility routines for PM.
 *
 * The entry points are:
 *   get_free_pid:	get a free process or group id
 *   no_sys:		called for invalid system call numbers
 *   find_param:	look up a boot monitor parameter
 *   find_proc:		return process pointer from pid number
 *   nice_to_priority	convert nice level to priority queue
 *   pm_isokendpt:	check the validity of an endpoint
 *   tell_vfs:		send a request to VFS on behalf of a process
 *   log_process_creation: logs the creation of a new process in a buffer
 *   log_process_termination:  Logs the creation of a process in a buffer
 *   find_process_by_pid: searches the pbuffer for a process with that id
 *   get_plog_size: returns the size of the plog
 */


#include "pm.h"
#include <sys/resource.h>
#include <sys/stat.h>
#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/endpoint.h>
#include <fcntl.h>
#include <signal.h>		/* needed only because mproc.h needs it */
#include "mproc.h"
#include "param.h"

#include <minix/config.h>
#include <timers.h>
#include <string.h>
#include <machine/archtypes.h>
#include "kernel/const.h"
#include "kernel/config.h"
#include "kernel/type.h"
#include "kernel/proc.h"

/*===========================================================================*
 *				get_free_pid				     *
 *===========================================================================*/
pid_t get_free_pid()
{
  static pid_t next_pid = INIT_PID + 1;		/* next pid to be assigned */
  register struct mproc *rmp;			/* check process table */
  int t;					/* zero if pid still free */

  /* Find a free pid for the child and put it in the table. */
  do {
	t = 0;
	next_pid = (next_pid < NR_PIDS ? next_pid + 1 : INIT_PID + 1);
	for (rmp = &mproc[0]; rmp < &mproc[NR_PROCS]; rmp++)
		if (rmp->mp_pid == next_pid || rmp->mp_procgrp == next_pid) {
			t = 1;
			break;
		}
  } while (t);					/* 't' = 0 means pid free */
  return(next_pid);
}


/*===========================================================================*
 *				no_sys					     *
 *===========================================================================*/
int no_sys()
{
/* A system call number not implemented by PM has been requested. */
  return(ENOSYS);
}

/*===========================================================================*
 *				find_param				     *
 *===========================================================================*/
char *find_param(name)
const char *name;
{
  register const char *namep;
  register char *envp;

  for (envp = (char *) monitor_params; *envp != 0;) {
	for (namep = name; *namep != 0 && *namep == *envp; namep++, envp++)
		;
	if (*namep == '\0' && *envp == '=')
		return(envp + 1);
	while (*envp++ != 0)
		;
  }
  return(NULL);
}

/*===========================================================================*
 *				find_proc  				     *
 *===========================================================================*/
struct mproc *find_proc(lpid)
pid_t lpid;
{
  register struct mproc *rmp;

  for (rmp = &mproc[0]; rmp < &mproc[NR_PROCS]; rmp++)
	if ((rmp->mp_flags & IN_USE) && rmp->mp_pid == lpid)
		return(rmp);

  return(NULL);
}

/*===========================================================================*
 *				nice_to_priority			     *
 *===========================================================================*/
int nice_to_priority(int nice, unsigned* new_q)
{
	if (nice < PRIO_MIN || nice > PRIO_MAX) return(EINVAL);

	*new_q = MAX_USER_Q + (nice-PRIO_MIN) * (MIN_USER_Q-MAX_USER_Q+1) /
	    (PRIO_MAX-PRIO_MIN+1);

	/* Neither of these should ever happen. */
	if ((signed) *new_q < MAX_USER_Q) *new_q = MAX_USER_Q;
	if (*new_q > MIN_USER_Q) *new_q = MIN_USER_Q;

	return (OK);
}

/*===========================================================================*
 *				pm_isokendpt			 	     *
 *===========================================================================*/
int pm_isokendpt(int endpoint, int *proc)
{
	*proc = _ENDPOINT_P(endpoint);
	if(*proc < -NR_TASKS || *proc >= NR_PROCS)
		return EINVAL;
	if(*proc >= 0 && endpoint != mproc[*proc].mp_endpoint)
		return EDEADEPT;
	if(*proc >= 0 && !(mproc[*proc].mp_flags & IN_USE))
		return EDEADEPT;
	return OK;
}

/*===========================================================================*
 *				tell_vfs			 	     *
 *===========================================================================*/
void tell_vfs(rmp, m_ptr)
struct mproc *rmp;
message *m_ptr;
{
/* Send a request to VFS, without blocking.
 */
  int r;

  if (rmp->mp_flags & VFS_CALL)
	panic("tell_vfs: not idle: %d", m_ptr->m_type);

  r = asynsend3(VFS_PROC_NR, m_ptr, AMF_NOREPLY);
  if (r != OK)
  	panic("unable to send to VFS: %d", r);

  rmp->mp_flags |= VFS_CALL;
}

/*===========================================================================*
 *				log the creation of a new process			 	     *
 *===========================================================================*/
void log_process_creation(pid_t new_pid, long timestamp){

  // Create a new plog_item
  plog_item new_item = {.i_pid = new_pid, .c_time = timestamp, .t_time = -1};

  // Add this item to the buffer.
  p_buffer.items[p_buffer.index] = new_item;

  // Increment the index.
  p_buffer.index++;

  // Wrap the index around if the buffer is full.
  if(p_buffer.index >= PLOG_BUFFER_SIZE){
    p_buffer.index = 0;
    p_buffer.full = 1;
  }
}

/*===========================================================================*
 *				log the termination time of a process			 	     *
 *===========================================================================*/
void log_process_termination(pid_t mpid, long timestamp){

  // Find the item with this pid.
  int index;
  plog_item item = find_process_by_pid(mpid, &index);

  // Exit if the item is not in the buffer.
  if(item.i_pid == -1){
    return;
  }

  // Add the termination time to this item and update the buffer.
  item.t_time = timestamp;
  p_buffer.items[index] = item;
}

/*===========================================================================*
 *				Performs a linear search for a process in the plog with a given pid*
 *===========================================================================*/
plog_item find_process_by_pid(pid_t mpid, int *index){

  // Get the buffer size.
  int buffer_size = get_pbuffer_size();

  // Do a linear search for an item with the given pid.
  for(int i=0; i<buffer_size; i++){
    if(p_buffer.items[i].i_pid == mpid){
      *index = i;
      return p_buffer.items[i];
    }
  }

  // Create an empty plog_item
  plog_item fake_item = {.i_pid = -1, .c_time = -1, .t_time = -1};
  return fake_item;
}


/*===========================================================================*
 *				Returns the size of the plog buffer     *
 *===========================================================================*/
int get_pbuffer_size(){
  return p_buffer.full ? PLOG_BUFFER_SIZE : p_buffer.index;
}

int start_tracking(pid_t pid){
  
}

int stop_tracking(pid_t pid){

}
