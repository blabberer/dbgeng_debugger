#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <dbgeng.h>
#pragma comment(lib, "dbgeng.lib")
// globals
IDebugClient *g_Client = NULL;
IDebugControl *g_Control = NULL;
IDebugBreakpoint *g_Breakpoint = NULL;
bool State = NULL;
// end of globals
// release the interfaces on last in first out  basis
void Exit(int Code, PCSTR Format, ...)
{
       if (g_Control != NULL) // print message if given
       {
              g_Control->Release(); // and exit the program
              g_Control = NULL;
       }
       if (g_Client != NULL)
       {
              g_Client->EndSession(DEBUG_END_PASSIVE);
              g_Client->Release();
              g_Client = NULL;
       }
       if (Format != NULL)
       {
              va_list Args;
              va_start(Args, Format);
              vfprintf(stderr, Format, Args);
              va_end(Args);
       }
       exit(Code);
};
// event callback class has 16 methods
class EventCallbacks : public DebugBaseEventCallbacks
{
public:
       STDMETHOD_(ULONG, AddRef)
       (THIS);
       STDMETHOD_(ULONG, Release)
       (THIS);
       STDMETHOD(Breakpoint)
       (THIS_ IN PDEBUG_BREAKPOINT Bp);
       STDMETHOD(ChangeDebuggeeState)
       (THIS_ IN ULONG Flags, IN ULONG64 Argument);
       STDMETHOD(ChangeEngineState)
       (THIS_ IN ULONG Flags, IN ULONG64 Argument);
       STDMETHOD(ChangeSymbolState)
       (THIS_ IN ULONG Flags, IN ULONG64 Argument);
       STDMETHOD(CreateThread)
       (THIS_ IN ULONG64 Handle, IN ULONG64 DataOffset, IN ULONG64 StartOffset);
       STDMETHOD(Exception)
       (THIS_ IN PEXCEPTION_RECORD64 Exception, IN ULONG FirstChance);
       STDMETHOD(ExitProcess)
       (THIS_ IN ULONG ExitCode);
       STDMETHOD(ExitThread)
       (THIS_ IN ULONG ExitCode);
       STDMETHOD(GetInterestMask)
       (THIS_ OUT PULONG Mask);
       STDMETHOD(SessionStatus)
       (THIS_ IN ULONG Status);
       STDMETHOD(SystemError)
       (THIS_ IN ULONG Error, IN ULONG Level);
       STDMETHOD(UnloadModule)
       (THIS_ IN PCSTR ImageBaseName, IN ULONG64 BaseOffset);
       STDMETHOD(LoadModule)
       (THIS_ IN ULONG64 ImageFileHandle, IN ULONG64 BaseOffset,
        IN ULONG ModuleSize, IN PCSTR ModuleName, IN PCSTR ImageName, IN ULONG CheckSum,
        IN ULONG TimeDateStamp);
       STDMETHOD(CreateProcess)
       (THIS_ IN ULONG64 ImageFileHandle, IN ULONG64 Handle,
        IN ULONG64 BaseOffset, IN ULONG ModuleSize, IN PCSTR ModuleName, IN PCSTR ImageName, IN ULONG CheckSum,
        IN ULONG TimeDateStamp, IN ULONG64 InitialThreadHandle, IN ULONG64 ThreadDataOffset, IN ULONG64 StartOffset);
};
// output callback class has 4 methods
class StdioOutputCallbacks : public IDebugOutputCallbacks
{
public:
       STDMETHOD(QueryInterface)
       (THIS_ IN REFIID InterfaceId, OUT PVOID *Interface);
       STDMETHOD_(ULONG, AddRef)
       (THIS);
       STDMETHOD_(ULONG, Release)
       (THIS);
       STDMETHOD(Output)
       (THIS_ IN ULONG Mask, IN PCSTR Text);
};
// input callback class has 5 methods
class StdioInputCallbacks : public IDebugInputCallbacks
{
public:
       STDMETHOD(QueryInterface)
       (THIS_ IN REFIID InterfaceId, OUT PVOID *Interface);
       STDMETHOD_(ULONG, AddRef)
       (THIS);
       STDMETHOD_(ULONG, Release)
       (THIS);
       STDMETHOD(StartInput)
       (THIS_ IN ULONG BufferSize);
       STDMETHOD(EndInput)
       (THIS_ void);
};
// implementation of 16 Event callbacks
// event callback method 1
STDMETHODIMP_(ULONG)
EventCallbacks::AddRef(THIS)
{
       return 1;
}
// event callback method 2
STDMETHODIMP_(ULONG)
EventCallbacks::Release(THIS)
{
       return 0;
}
// event callback method 3
STDMETHODIMP EventCallbacks::Breakpoint(THIS_ IN PDEBUG_BREAKPOINT Bp)
{
       printf("Bpcallback bp %p\n", Bp);
       return DEBUG_STATUS_BREAK;
};
// event callback method 4
STDMETHODIMP EventCallbacks::ChangeDebuggeeState(THIS_ IN ULONG Flags, IN ULONG64 Argument)
{
       printf("ChangeDebugeeState Flags = %x\n", Flags);
       if ((Flags & DEBUG_CDS_ALL) == DEBUG_CDS_ALL)
       {
              printf("Flags = DEBUG_CDS_ALL\n");
       }
       else if ((Flags & DEBUG_CDS_REGISTERS) == DEBUG_CDS_REGISTERS)
       {
              printf("Flags = DEBUG_CDS_REGISTERS\n");
       }
       else if ((Flags & DEBUG_CDS_DATA) == DEBUG_CDS_DATA)
       {
              printf("Flags = DEBUG_CDS_DATA\n");
              if ((Argument & DEBUG_DATA_SPACE_VIRTUAL) == DEBUG_DATA_SPACE_VIRTUAL)
              {
                     printf("virtual Data Change\n");
              }
              else
              {
                     printf("some other data Change\n");
              }
       }
       else if ((Flags & DEBUG_CDS_REFRESH) == DEBUG_CDS_REFRESH)
       {
              {
                     printf("Flags = DEBUG_CDS_REFRESH\n");
              }
       }
       else
       {
              printf("unknown flags\n");
       }
       printf("ChangeDebugeeState Argument  = %I64x\n", Argument);
       State = 1;
       return DEBUG_STATUS_NO_CHANGE;
}
// event callback method 5
STDMETHODIMP EventCallbacks::ChangeEngineState(THIS_ IN ULONG Flags, IN ULONG64 Argument)
{
       printf("ChangeEngineState Flags = %x\n", Flags);
       printf("ChangeEngineState Argument  = %I64x\n", Argument);
       return DEBUG_STATUS_NO_CHANGE;
}
// event callback method 6
STDMETHODIMP EventCallbacks::ChangeSymbolState(THIS_ IN ULONG Flags, IN ULONG64 Argument) // method 16
{
       printf("ChangeSymbolState Flags = %x\n", Flags);
       printf("ChangeSymbolState Argument  = %I64x\n", Argument);
       return DEBUG_STATUS_NO_CHANGE;
}
// event callback method 7
STDMETHODIMP EventCallbacks::CreateThread(THIS_ IN ULONG64 Handle, IN ULONG64 DataOffset, IN ULONG64 StartOffset)
{
       printf("CreateThread Handle = %I64x\n", Handle);
       printf("Create Thread Data Offset = %I64x\n", DataOffset);
       printf("Create Thread Start Offset = %I64x\n", StartOffset);
       return DEBUG_STATUS_NO_CHANGE;
}
// event callback method 8
STDMETHODIMP EventCallbacks::Exception(THIS_ IN PEXCEPTION_RECORD64 Exception, IN ULONG FirstChance)
{
       printf("ExRecPtr = %p\n", Exception);
       printf("ExCode = %x\n", Exception->ExceptionCode);
       printf("fchance = %x\n", FirstChance);
       return DEBUG_STATUS_BREAK;
}
// event callback method 9
STDMETHODIMP EventCallbacks::ExitProcess(THIS_ IN ULONG ExitCode)
{
       printf("ExitProc ExitCode = %x\n", ExitCode);
       return DEBUG_STATUS_NO_CHANGE;
}
// event callback method 10
STDMETHODIMP EventCallbacks::ExitThread(THIS_ IN ULONG ExitCode)
{
       printf("ExitThread ExitCode = %x\n", ExitCode);
       return DEBUG_STATUS_NO_CHANGE;
}
// event callback method 11
STDMETHODIMP EventCallbacks::GetInterestMask(THIS_ OUT PULONG Mask)
{
       *Mask =
           DEBUG_EVENT_BREAKPOINT |
           DEBUG_EVENT_EXCEPTION |
           DEBUG_EVENT_CREATE_THREAD |
           DEBUG_EVENT_EXIT_THREAD |
           DEBUG_EVENT_CREATE_PROCESS |
           DEBUG_EVENT_EXIT_PROCESS |
           DEBUG_EVENT_LOAD_MODULE |
           DEBUG_EVENT_UNLOAD_MODULE |
           DEBUG_EVENT_SYSTEM_ERROR |
           DEBUG_EVENT_SESSION_STATUS |
           DEBUG_EVENT_CHANGE_DEBUGGEE_STATE |
           DEBUG_EVENT_CHANGE_ENGINE_STATE |
           DEBUG_EVENT_CHANGE_SYMBOL_STATE;
       return S_OK;
}
// event callback method 12
STDMETHODIMP EventCallbacks::SessionStatus(THIS_ IN ULONG SessionStatus)
{
       printf("SessionStatus = %x\n", SessionStatus);
       return DEBUG_STATUS_NO_CHANGE;
}
// event callback method 13
STDMETHODIMP EventCallbacks::SystemError(THIS_ IN ULONG Error, IN ULONG Level)
{
       printf("SystemError Error  = %x\n", Error);
       printf("SystemError Level  = %x\n", Level);
       return DEBUG_STATUS_BREAK;
}
// event callback method 14
STDMETHODIMP EventCallbacks::UnloadModule(THIS_ IN PCSTR ImageBaseName, IN ULONG64 BaseOffset)
{
       printf("Unload module ImageBaseName = %s\n", ImageBaseName);
       printf("unload Module BaseOffset = %I64x\n", BaseOffset);
       return DEBUG_STATUS_NO_CHANGE;
}
// event callback method 15
STDMETHODIMP EventCallbacks::LoadModule(THIS_ IN ULONG64 ImageFileHandle, IN ULONG64 BaseOffset,
                                        IN ULONG ModuleSize, IN PCSTR ModuleName, IN PCSTR ImageName,
                                        IN ULONG CheckSum, IN ULONG TimeDateStamp)
{
       printf("Load Module ImageFileHandle = %I64x\n", ImageFileHandle);
       printf("Load Module BaseOffset = %I64x\n", BaseOffset);
       printf("Load Module ModuleSize = %x\n", ModuleSize);
       printf("Load Module ModuleName = %s\n", ModuleName);
       printf("Load Module ImageName = %s\n", ImageName);
       printf("Load Module Checksum = %x\n", CheckSum);
       printf("Load Module TimeDateStamp = %x\n", TimeDateStamp);
       return DEBUG_STATUS_NO_CHANGE;
}
// event callback method 16
STDMETHODIMP EventCallbacks::CreateProcess(THIS_ IN ULONG64 ImageFileHandle, IN ULONG64 Handle,
                                           IN ULONG64 BaseOffset, IN ULONG ModuleSize, IN PCSTR ModuleName, IN PCSTR ImageName,
                                           IN ULONG CheckSum, IN ULONG TimeDateStamp, IN ULONG64 InitialThreadHandle,
                                           IN ULONG64 ThreadDataOffset, IN ULONG64 StartOffset)
// event callback method 4  and so on till method 16
{
       printf("CreateProcess Image File Handle = %I64x\n", ImageFileHandle);
       printf("CreateProcess Handle = %I64x\n", Handle);
       printf("CreateProcess base offset = %I64x\n", BaseOffset);
       printf("CreateProcess ModuleSize = %x\n", ModuleSize);
       printf("CreateProcess Module Name = %s\n", ModuleName);
       printf("CreateProcess Image Name = %s\n", ImageName);
       printf("CreateProcess CheckSum = %x\n", CheckSum);
       printf("CreateProcess TimeDateStamp = %x\n", TimeDateStamp);
       printf("CreateProcess InitialThreadHandle = %I64x\n", InitialThreadHandle);
       printf("CreateProcess Thread Data Offset  = %I64x\n", ThreadDataOffset);
       HRESULT Status = 0; // we are setting a break point in Address Of Entrypoint of the debuggee
       if ((Status = g_Control->AddBreakpoint(DEBUG_BREAKPOINT_CODE, DEBUG_ANY_ID, &g_Breakpoint)) == S_OK)
       {
              if ((Status = g_Breakpoint->SetOffset(StartOffset)) == S_OK)
              {
                     if ((Status = g_Breakpoint->SetFlags(DEBUG_BREAKPOINT_ENABLED)) == S_OK)
                     {
                            if ((Status = g_Breakpoint->SetCommand(
                                     ".echo This is a Bp From DBGENGDBG on Address of EntryPoint")) == S_OK)
                            {
                                   return DEBUG_STATUS_NO_CHANGE;
                            }
                            printf("SetFlags failed %x\n", Status);
                     }
                     printf("Setoffset failed %x\n", Status);
              }
              printf("AddBp failed %x\n", Status);
       }
       return DEBUG_STATUS_NO_CHANGE;
}
// end of event call back method implementation
// start of OutputCallback Method Implementation
// 4 methods
// event callback method 1
STDMETHODIMP StdioOutputCallbacks::QueryInterface(THIS_ IN REFIID InterfaceId, OUT PVOID *Interface)
{
       // outputevent callbacks 3 methods
       *Interface = NULL;
       if (IsEqualIID(InterfaceId, __uuidof(IUnknown)) ||
           IsEqualIID(InterfaceId, __uuidof(IDebugOutputCallbacks)))
       {
              *Interface = (IDebugOutputCallbacks *)this;
              AddRef();
              return S_OK;
       }
       else
       {
              return E_NOINTERFACE;
       }
}
// event callback method 2
STDMETHODIMP_(ULONG)
StdioOutputCallbacks::AddRef(THIS) // method 1
{
       return 1;
}
// event callback method 3
STDMETHODIMP_(ULONG)
StdioOutputCallbacks::Release(THIS) // method 2
{
       return 0;
}
// event callback method 4
STDMETHODIMP StdioOutputCallbacks::Output(THIS_ IN ULONG Mask, IN PCSTR Text) // method 3
{
       UNREFERENCED_PARAMETER(Mask);
       fputs(Text, stdout);
       return S_OK;
}
// end of output call back method implementation
// start of inputCallback Method Implementation
// 5 methods
// event callback method 1
STDMETHODIMP StdioInputCallbacks::QueryInterface(THIS_ IN REFIID InterfaceId, OUT PVOID *Interface)
{
       // inputevent callbacks 4 methods
       *Interface = NULL;
       if (IsEqualIID(InterfaceId, __uuidof(IUnknown)) ||
           IsEqualIID(InterfaceId, __uuidof(IDebugInputCallbacks)))
       {
              *Interface = (IDebugInputCallbacks *)this;
              AddRef();
              return S_OK;
       }
       else
       {
              return E_NOINTERFACE;
       }
}
// event callback method 2
STDMETHODIMP_(ULONG)
StdioInputCallbacks::AddRef(THIS)
{
       return 1;
}
// event callback method 3
STDMETHODIMP_(ULONG)
StdioInputCallbacks::Release(THIS)
{
       return 0;
}
// event callback method 4
STDMETHODIMP StdioInputCallbacks::StartInput(THIS_ IN ULONG BufferSize)
{
       char *Inbuff = (char *)malloc(BufferSize + 10);
       if (Inbuff)
       {
              memset(Inbuff, 0, BufferSize + 10);
              fgets(Inbuff, BufferSize, stdin);
              g_Control->ReturnInput(Inbuff);
              free(Inbuff);
       }
       return S_OK;
}
// event callback method 5
STDMETHODIMP StdioInputCallbacks::EndInput(THIS_ void) // method 4
{
       return S_OK;
}
// instances of classes
StdioOutputCallbacks g_OutputCb;
StdioInputCallbacks g_InputCb;
EventCallbacks g_EventCb;
// functions used in main
BOOL WINAPI HandlerRoutine(DWORD dwCtrlType) // control + c handler
{
       switch (dwCtrlType)
       {
       case CTRL_C_EVENT:
              g_Control->SetInterrupt(DEBUG_INTERRUPT_ACTIVE);
              printf(
                  "You Pressed Ctrl-C\n"
                  "if you did not intend to break enter g to continue execution");
              return TRUE;
       default:
              return FALSE;
       }
}
// main program
void __cdecl main(int Argc, char *Argv[])
{
       HRESULT Status = 0;
       ULONG InputSize = 0;
       ULONG ExecStatus = 0;
       char Buff[0x105];
       memset(Buff, 0, sizeof(Buff));
       if (Argc != 2) // looks for exe name
       {
              Exit(0, "provide path of exe\n");
       }
       if ((Status = DebugCreate(__uuidof(IDebugClient), (void **)&g_Client)) != S_OK) // Createinterfaces
       {
              Exit(1, "DebugCreate failed, 0x%X\n", Status);
       }
       if ((Status = g_Client->QueryInterface(__uuidof(IDebugControl), (void **)&g_Control)) != S_OK) // Createinterfaces
       {
              Exit(1, "QueryInterface failed, 0x%X\n", Status);
       }
       if (
           ((Status = g_Client->SetEventCallbacks(&g_EventCb)) != S_OK) || // Register Callbacks
           ((Status = g_Client->SetOutputCallbacks(&g_OutputCb)) != S_OK) ||
           ((Status = g_Client->SetInputCallbacks(&g_InputCb)) != S_OK))
       {
              Exit(1, "Setting of Callbacks failed\n");
       }
       SetConsoleCtrlHandler(&HandlerRoutine, TRUE);                                        // Set control +c handler
       if ((Status = g_Client->CreateProcess(0, Argv[1], DEBUG_ONLY_THIS_PROCESS)) != S_OK) // create a process
       {
              Exit(1, "CreateProcess failed, 0x%X\n", Status);
       }
       Status = g_Control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE); // wait for event
       while (TRUE)                                                    // spin till debuggee exits for processing events
       {
              if ((g_Control->GetExecutionStatus(&ExecStatus)) != S_OK)
              {
                     printf("GetExecutionStatus failed\n");
                     break; // quit while loop if GetExecutionStatus failed
              }
              if (ExecStatus == DEBUG_STATUS_NO_DEBUGGEE)
              {
                     printf("no Debuggee\n");
                     break; // quit while loop if no debugee
              }
              if (
                  (ExecStatus == DEBUG_STATUS_GO) ||
                  (ExecStatus == DEBUG_STATUS_STEP_OVER) ||
                  (ExecStatus == DEBUG_STATUS_STEP_INTO) ||
                  (ExecStatus == DEBUG_STATUS_STEP_BRANCH))
              {
                     if ((Status = g_Control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE)) == S_OK)
                     {
                            continue; // do not prompt do not stop on the above execution status
                     }
                     if (Status != E_UNEXPECTED)
                     {
                            printf("Wait For Event in mainloop failed\n");
                            break; // quit while loop on a fatal error in waitforevent
                     }
              }
              if (State == 1) // print register disassembly for 1 line , effective address and symbols
              {
                     State = 0;
                     g_Control->OutputCurrentState(DEBUG_OUTCTL_THIS_CLIENT, DEBUG_CURRENT_DEFAULT);
              }
              g_Control->OutputPrompt(DEBUG_OUTCTL_THIS_CLIENT, NULL);              // set the prompt
              if ((g_Control->Input(Buff, (sizeof(Buff) - 5), &InputSize)) != S_OK) // get input when broken
              {
                     Exit(0, "g_Control->Input Failed\n");
              }
              else if (InputSize)
              {
                     g_Control->Execute(DEBUG_OUTCTL_THIS_CLIENT, Buff, DEBUG_EXECUTE_DEFAULT); // execute the input
              }
       }
       Exit(0, "Finished Debugging Quitting\n"); // print this msg and exit program
}
