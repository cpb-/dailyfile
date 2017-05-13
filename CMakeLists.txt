#######################################################################
# CMakeLists.txt for the dailyfile project.
#
# Copyright 2017  Christophe BLAESS (https://www.blaess.fr/christophe).
#
# License  GPLv3+:  This is free software:  you are free to  change and
# redistribute it.There is NO WARRANTY, to the extent permitted by law.
#
#######################################################################

cmake_minimum_required(VERSION 2.6)
project(DAILYFILE)

execute_process(COMMAND git describe OUTPUT_VARIABLE GIT_REPO_VERSION)

add_definitions( -DPROGRAM_VERSION=${GIT_REPO_VERSION} )

add_executable(dailyfile dailyfile.c)

install(TARGETS dailyfile DESTINATION /usr/local/bin)
