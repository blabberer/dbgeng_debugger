# dbgeng_debugger 
test code for event callbacks   
a full blown standalone dbgeng based debugger    
compiled and tested using vs2019 x64 devcmdprompt as x64 with cl /Zi /W4 /Od /analyze /nologo /EHsc dbxxxx.cpp /link /release    

usage dxxxx.exe someexe.exe    

should spit    

Microsoft (R) Windows Debugger Version 10.0.19041.1503 AMD64   
Copyright (c) Microsoft Corporation. All rights reserved.   

ChangeEngineState Flags = 800   
ChangeEngineState Argument  = 0   
CommandLine: testdump.exe   
ChangeEngineState Flags = 10   
ChangeEngineState Argument  = 1   

if you edit virtual memory EventCallbacks::ChangeDebuggeeState should show   

testdump!__scrt_common_main_seh:   
00007ff7`a4d111f0 48895c2408      mov     qword ptr [rsp+8],rbx ss:000000b3`815afd70=0000000000000000   
0:000>ed [rsp+8]   <<<<<<<<<<<< changing virtual memory contents   
000000b3`815afd70 00000000 1

ChangeDebugeeState Flags = 2

Flags = DEBUG_CDS_DATA

virtual Data Change 

ChangeDebugeeState Argument  = 0

000000b3`815afd74 00000000 


