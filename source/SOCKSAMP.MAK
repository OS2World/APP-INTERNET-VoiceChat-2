#  Compiler options:
#
#   /Ti  : Generate debugger information
#   /Sm  : Allow migration extensions
#   /Ss  : Allow '//' for comments
#   /Q   : Do not display logo
#   /Kb  : Produce basic diagnostic message
#   /W2  : Produce severe errors, errors and warnings
#   /Gm  : Link with the multithread version of library
#   /Gt  : Enavle all variables to be passed to 16-bit function.
#          Static & external variables are mapped into 16-bit segment.
#          Variables larger then 64k will be alligned on, but will still
#          cross, 64k boundary.
#   /C   : Compile only
#
#
#  Linker options:
#
#   /De  : Prepare for Debugging
#   /NOI : Preserve the case sensitivity

ALL : dialin.exe dialout.exe

out.obj: out.c
  icc  /DOS2 /Sm /Ss /Q  /Ti-  /O+ /Op+  /Tx- /W2 /C out.c

in.obj: in.c
  icc  /DOS2 /Sm /Ss /Q  /Ti-  /O+ /Op+  /Tx- /W2 /C in.c

dialin.obj: dialin.c
  icc  /DOS2 /Sm /Ss /Q  /Ti-  /O+ /Op+  /Tx- /W2 /C dialin.c

dialin.exe: dialin.obj in.obj socksamp.mak
  link386 /exepack dialin in ,dialin.exe,NUL,os2386.lib so32dll.lib tcp32dll.lib,sock.def;

dialout.obj: dialout.c
  icc  /DOS2 /Sm /Ss /Q  /Ti-  /O+ /Op+  /Tx- /W2 /C dialout.c

dialout.exe: dialout.obj out.obj socksamp.mak
  link386 /exepack dialout out,dialout.exe,NUL,os2386.lib so32dll.lib tcp32dll.lib,sock.def;

