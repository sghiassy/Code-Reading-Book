/*
 * as Windows does not support signal, OnServe use event to emulate them.
 * The supported signal is SIGTERM.
 * The kills.c contains the kill logic.
 */
#ifdef OS_CYGWIN
#include <windows.h>
#include <stdio.h>
static void (*HandleTerm)()=NULL; // address of the handler routine.

/*
 * Event handling routine
 */
void v_difthf(LPVOID par)
{
HANDLE hevint; /* make a local copy because the parameter is shared! */

  hevint = (HANDLE) par;

  for (;;) {
    if (WaitForSingleObject(hevint,INFINITE) == WAIT_FAILED) {
      // something have gone wrong.
      return; // may be something more is needed.
      }

    // call the interrupt handler.
    if (HandleTerm==NULL) return;
    HandleTerm();
    }
}

/*
 * set a routine handler for the signal
 * note that it cannot be used to change the signal handler
 */
int SetTerm(void (*func)())
{
char Name[256];
HANDLE hevint, hthread;
DWORD ThreadId; 
SECURITY_ATTRIBUTES sa;
SECURITY_DESCRIPTOR sd;

  sprintf(Name,"TERM%ld",GetCurrentProcessId());

  /*
   * event cannot be inherited.
   * the event is reseted to nonsignaled after the waiting thread is released.
   * the start state is resetted.
   */

  // Initialize the new security descriptor.
  InitializeSecurityDescriptor (&sd, SECURITY_DESCRIPTOR_REVISION);

  // Add a NULL descriptor ACL to the security descriptor.
  SetSecurityDescriptorDacl (&sd, TRUE, (PACL)NULL, FALSE);

  sa.nLength = sizeof(sa);
  sa.lpSecurityDescriptor = &sd;
  sa.bInheritHandle = TRUE;


  //  It works also with NULL instead &sa!!
  hevint = CreateEvent(&sa,FALSE, FALSE,Name);

  HandleTerm = (int (*)()) func;

  if (hevint == NULL) return(-1); // failed

  /* create the thread to wait for event */
  hthread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) v_difthf,
                         (LPVOID) hevint, 0, &ThreadId);
  if (hthread == NULL) {
    // failed remove the event
    CloseHandle(hevint); // windows will remove it.
    return(-1);
    }

  CloseHandle(hthread); // not needed
  return(0);
}
#endif
