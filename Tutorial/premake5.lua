solution "IOCP_01"
  configurations { "Debug", "Release" }
  platforms { "Win64" }
  
-- A project defines one build target
project "IOCP_01"
kind "ConsoleApp"
language "C++"
files { "**.h", "**.cpp" }

configuration "Debug"
  defines { "DEBUG" }
  flags { "Symbols" }
  platforms { "Win64" }

configuration "Release"
  defines { "NDEBUG" }
  flags { "Optimize" }
    
filter { "platforms:Win64" }
   architecture "x64"
   targetdir("../bin")