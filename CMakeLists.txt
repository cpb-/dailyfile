cmake_minimum_required(VERSION 2.6)
project(DAILYFILE)

set(CMAKE_CXX_FLAGS "-g -Wall")

add_executable(dailyfile dailyfile.c)

install(TARGETS dailyfile DESTINATION /usr/local/bin)
