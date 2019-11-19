cmake_minimum_required(VERSION 3.10)
project(CliCfg)
if(sysroot)
set(CMAKE_SYSROOT ${sysroot})
endif()

if(crosscompilegcc)
set(CMAKE_C_COMPILER ${crosscompilegcc})
endif()

add_executable(CliCfg cli.c)
