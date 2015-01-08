ILaunch		- Injection launcher
Injection	- Core DLL
RemCrypt	- Plugin which removes encryption from client


ILaunch only starts client and loads injection.dll into it.
All patching is done inside injection.dll after loading. Communication
between ILaunch & DLL is done via registry. This allows DLL to be loaded 
in other ways.

All strings except which are used in debugging are moved to resources.
Use LoadString(IDS_blablabla) to get them.

all callbacks are called in the context of GUI thread! So they should be as 
short as possible.

All installable commands should write the following text when executed without
parameters or with invalid params:
"Usage: command_name [parameters]
Command description.
   parameter - parameter description"
There should be 3 spaces before parameter description.
