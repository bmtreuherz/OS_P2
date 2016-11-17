/* This file contains a collection of miscellaneous procedures:
 *   panic:    abort MINIX due to a fatal error
 *   kputc:          buffered putc used by kernel printf
 */

#include "kernel/kernel.h"
#include "arch_proto.h"

#include <minix/syslib.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>

#include <minix/sys_config.h>

#include <minix/sysutil.h>
#include <stdio.h>

#define ARE_PANICING 0xDEADC0FF

/*===========================================================================*
 *			panic                                          *
 *===========================================================================*/
void panic(const char *fmt, ...)
{
  va_list arg;
  /* The system has run aground of a fatal kernel error. Terminate execution. */
  if (kinfo.minix_panicing == ARE_PANICING) {
  	reset();
  }
  kinfo.minix_panicing = ARE_PANICING;
  if (fmt != NULL) {
	printf("kernel panic: ");
  	va_start(arg, fmt);
	vprintf(fmt, arg);
	printf("\n");
  }

  printf("kernel on CPU %d: ", cpuid);
  util_stacktrace();

#if 0
  if(get_cpulocal_var(proc_ptr)) {
	  printf("current process : ");
	  proc_stacktrace(get_cpulocal_var(proc_ptr));
  }
#endif

  /* Abort MINIX. */
  minix_shutdown(NULL);
}

/*===========================================================================*
 *				kputc				     	     *
 *===========================================================================*/
void kputc(c)
int c;					/* character to append */
{
/* Accumulate a single character for a kernel message. Send a notification
 * to the output driver if an END_OF_KMESS is encountered.
 */
  if (c != END_OF_KMESS) {
      int maxblpos = sizeof(kmess.kmess_buf) - 2;
#ifdef DEBUG_SERIAL
      if (kinfo.do_serial_debug) {
	if(c == '\n')
      		ser_putc('\r');
      	ser_putc(c);
      }
#endif
      kmess.km_buf[kmess.km_next] = c;	/* put normal char in buffer */
      kmess.kmess_buf[kmess.blpos] = c;
      if (kmess.km_size < sizeof(kmess.km_buf))
          kmess.km_size += 1;
      kmess.km_next = (kmess.km_next + 1) % _KMESS_BUF_SIZE;
      if(kmess.blpos == maxblpos) {
      	memmove(kmess.kmess_buf,
		kmess.kmess_buf+1, sizeof(kmess.kmess_buf)-1);
      } else kmess.blpos++;
  } else {
      int p;
      endpoint_t outprocs[] = OUTPUT_PROCS_ARRAY;
      if(!(kinfo.minix_panicing || kinfo.do_serial_debug)) {
	      for(p = 0; outprocs[p] != NONE; p++) {
		 if(isokprocn(outprocs[p])) {
       	    send_sig(outprocs[p], SIGKMESS);
		 }
      	}
     }
  }
  return;
}

/*===========================================================================*
 *				_exit				     	     *
 *===========================================================================*/
void _exit(e)
int e;					/* error code */
{
  panic("_exit called from within the kernel, should not happen. (err %i)", e);
}

char* get_state_string(int state){
  switch(state){
    case 0:
      return "NEW";
    case 1:
      return "RDY";
    case 2:
      return "RUN";
    case 3:
      return "BLK";
    default:
      return "TRM";
  }
}


// TODO: Clean this up
typedef struct{
  pid_t p_id;
  clock_t timestamp;
  char *from_state;
  char *to_state;
} buffer_entry;

#define buffer_size  5

buffer_entry log_buffer[buffer_size];
int buffer_index = 0;
int buffer_full = 0;

void log_state_change(pid_t p_id, clock_t timestamp, int from, int to){
  char *fromState = get_state_string(from);
  char *toState = get_state_string(to);

  if(buffer_index >= buffer_size){
    buffer_full = 1;
    buffer_index = 0;
  }

  if(buffer_full && to != 2){
    // print the buffer
    for(int i=0; i<buffer_size; i++){
      buffer_entry current = log_buffer[i];
      printf("%d\t%d\t%s\t%s\n", current.p_id, current.timestamp, current.from_state, current.to_state);
    }
    buffer_full = 0;
    buffer_index = 0;
  }

  buffer_entry new_item = {.p_id = p_id, .timestamp = timestamp, .from_state = fromState, .to_state = toState};
  log_buffer[buffer_index++] = new_item;

  // If its a terminate just print this no matter what.
  if(to == 4){
    for(int i=0; i<buffer_index; i++){
      buffer_entry current = log_buffer[i];
      printf("%d\t%d\t%s\t%s\n", current.p_id, current.timestamp, current.from_state, current.to_state);
    }
    buffer_full = 0;
    buffer_index = 0;
  }
}
