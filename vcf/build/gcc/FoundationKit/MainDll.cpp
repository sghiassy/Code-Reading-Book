#include "D:\code\vcf\build\gcc\FoundationKit\MainDll.h"
#include <windows.h>
#include <stdio.h>

/* Externs */
DllClass DLLIMPORT global_dllclass1 (1);
DllClass DLLIMPORT global_dllclass2 (2);
int DLLIMPORT global_int_variable = 5;

int
DllClassBase::virtual_method () const
{
  return -1;
}

DllClass::DllClass (int i) : i_(i) { ++instances; }
DllClass::~DllClass () { --instances; }

void
DllClass::virtual_method ()
{
    printf("Hello from virtual method :-)\n\n");
}

void
DllClass::non_virtual_method ()
{
    printf("Hello from non-virtual method ;-)\n\n");
}

int DllClass::instances;

BOOL APIENTRY DllMain (HINSTANCE hInst, DWORD reason,
                       LPVOID reserved /* Not used. */ );

/* DllMain Results:
       TRUE on success, FALSE on failure.  */

BOOL APIENTRY
DllMain (
	 HINSTANCE hInst        /* Library instance handle. */ ,
	 DWORD reason           /* Reason this function is being called. */ ,
	 LPVOID reserved        /* Not used. */ )
{

  switch (reason)
    {
    case DLL_PROCESS_ATTACH:
      break;

    case DLL_PROCESS_DETACH:
      break;

    case DLL_THREAD_ATTACH:
      break;

    case DLL_THREAD_DETACH:
      break;
    }
  return TRUE;
}
