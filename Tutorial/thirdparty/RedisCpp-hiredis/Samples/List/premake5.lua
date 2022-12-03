solution "List"
  configurations { "Debug", "Release" }
  platforms { "Win64" }
  
-- A project defines one build target
project "List"
kind "ConsoleApp"
language "C++"
files { "**.h", "**.hpp", "**.cpp" }

includedirs { 
  "D:/Dev/c++/thirdparty/hiredis"
}

configuration "Debug"
  defines { "DEBUG" }
  platforms { "Win64" }
  libdirs { "D:/Dev/c++/thirdparty/hiredis/vc_soluration/hiredis.dir/Debug" }

configuration "Release"
  defines { "NDEBUG" }
    
filter { "platforms:Win64" }
   architecture "x64"
   targetdir("../bins")