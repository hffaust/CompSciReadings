/***********************************************************************************\

VTInject.cpp

VTInject will adjust the privilege of the current process so we can access processes operating as LOCALSYSTEM. Once the privileges are adjusted VTInject will open a handle to the target process id (PID) and load our VulnTrace.DLL into the process. 

\***********************************************************************************/


#include <stdio.h>
#include <windows.h>
#include "detours.h"

#define DLLNAME "\\VulnTrace.dll"

int CDECL inject_dll(DWORD nProcessId, char *szDllPath)
{
     HANDLE token;
     TOKEN_PRIVILEGES tkp;
     HANDLE hProc;
     
     if(OpenProcessToken(     GetCurrentProcess(), 
                     TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, 
                    &token) == FALSE)
     {
          fprintf(stderr, "OpenProcessToken Failed: 0x%X\n", GetLastEr-ror());
          return(-1);
     }
     if(LookupPrivilegeValue(     NULL, 
                         "SeDebugPrivilege", 
                         &tkp.Privileges[0].Luid) == FALSE)
     {
          fprintf(stderr, "LookupPrivilegeValue failed: 0x%X\n", GetLas-tError());
          return(-1);
     }

     tkp.PrivilegeCount = 1;
     tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

     if(AdjustTokenPrivileges(     token, FALSE, &tkp, 0, NULL, NULL) == FALSE)
     {
          fprintf(stderr,
          
               “AdjustTokenPrivileges Failed: 0x%X\n", 

          GetLastError());

          return(-1);
     }

     CloseHandle(token);

     hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, nProcessId);
     if (hProc == NULL)
     {
          fprintf(stderr,

               "[VTInject]: OpenProcess(%d) failed: %d\n",
               
          nProcessId, GetLastError());
          return(-1);
     }
      
     fprintf(stderr, 
          
          "[VTInject]: Loading %s into %d.\n", 

     szDllPath, nProcessId);
     
     fflush(stdout);
    
     if (!DetourContinueProcessWithDllA(hProc, szDllPath))
     {
          fprintf(stderr,
          
               "DetourContinueProcessWithDll(%s) failed: %d",
          
          szDllPath, GetLastError());
          
          return(-1);
     }

     return(0);
}

int main(int argc, char **argv)
{
     char path[1024];
     int plen;

     if(argc!= 2)
     {
          fprintf(stderr,     

               "\n-= VulnTrace =-\n\n"
               "\tUsage: %s <process_id>\n\n"

          ,argv[0]);

          return(-1);
     }


     plen = GetCurrentDirectory(sizeof(path)-1, path);
     strncat(path, DLLNAME, (sizeof(path)-plen)-1);
     if(inject_dll(atoi(argv[1]), path))
     {
          fprintf(stderr, "Injection Failed\n");
          return(-1);
     }

     return(0);
};
