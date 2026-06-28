# To compile this application, use CMAKE.

<!-- SDL3 needs to be static and the libSDL3.dll.a needs to be in **thirdparty/SDL3/build** -->
**Know that this method only works for windows (MINGW64)/(MSVC) or linux (GCC) compiler!**

Before you compile make sure youre at **thirdparty/SDL3**, then run the following:  
    
    cmake -S . -B build
    cmake build
    cmake --build build

With the compiled library, you can now build code editor, for that you need to be located at your root dir,  
and insert the commands for the second time, the same as what you did for SDL3.

After that you should be able to run the executable in **root/build**.