cmake_minimum_required(VERSION 3.10)
project(CodecaveDumper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(codecave_dumper
    main.cpp
    codecave_dumper.cpp
    codecave_dumper_cli.cpp
)