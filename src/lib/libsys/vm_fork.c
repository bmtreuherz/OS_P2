
#include "syslib.h"

#include <minix/vm.h>

/*===========================================================================*
 *                                vm_fork				     *
 *===========================================================================*/
int vm_fork(endpoint_t ep, int slot, endpoint_t *childep, pid_t c_pid)
{
    message m;
    int result;

    m.VMF_ENDPOINT = ep;
    m.VMF_SLOTNO = slot;
    m.VMF_CHILD_PID = c_pid;

    result = _taskcall(VM_PROC_NR, VM_FORK, &m);

    *childep = m.VMF_CHILD_ENDPOINT;

    return(result);
}
