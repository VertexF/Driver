add_library(Driver_Lib STATIC
						 olcConsoleGameEngine.h
						 olcConsoleGameEngine.cpp)

add_executable(Driver main.cpp)

target_link_libraries(Driver PUBLIC Driver_Lib)