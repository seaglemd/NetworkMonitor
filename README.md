# NetworkMonitor
Author: Matthew Seagle

Network monitoring program written in C++ using FLTK for the gui and curl libraries for blacklisted ip lookup.
The code and project files here are for Microsoft Visual Studio 2013 professional. Instructions for compiling under
this parameter is given. I am not responsible for correct compilation under another IDE or Compiler.

FLTK version 1.3.3

curlLib version 1.371

These are not instructions on installing the curl library or the FLTK libraries. (Although there is useful information
here regarding usage/installation). These are instructions on how to use FLTK and Curl in a MSVS Pro 2013 project.

To setup a MSVS Pro 2013 project using FLTK (and curl if desired although required to run this program) do the following.

1. File->New->Project...
2. Under Visual C++ create a Win32 Application.
3. On the Win32 Application wizard Hit Next.
4. Check the radio buttion for Windows Application.
5. Under Additional options: click Empty Project and unselect Security Development Lifecycle.
6. Hit finish.
7. Under your Solution Explorer right click on your project (Default name might be something like Win32Project2)
   and hit properties.
8. In the C/C++ tab go to Code Generation and under Runtime Library Multi-threaded Debug DLL make sure it has the /MD       option selected.
9. Under the Linker tab go to Input and open up the Additional Dependencies with edit and copy in 
   fltk.lib
   fltkimages.lib
   fltkjpeg.lib
   fltkpng.lib
   fltkzlib.lib (FLTK requirements end here)
   curllib.lib
   curllib_static.lib
   libcurl_imp.lib
   libeay32.lib
   openldap.lib
   ssleay32.lib
10. Under Ignore Specific Default libraries add libcd.lib (FLTK doesn't want it)
12. Curl in order to work even in Debug and run from the compiler needs some .dlls in the project folder itself. In 
    the project folder (and possibly any release/debug folders) place curllib.dll, libeay32.dll, libsasl.dll, 
    openldap.dll, and ssleay32.dll
13. These are the steps I follow every time to setup my project and hopefully will work for you as well. Apply changes
    and get to work!
