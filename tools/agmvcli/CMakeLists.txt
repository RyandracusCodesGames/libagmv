cmake_minimum_required(VERSION 3.25.1)
project(agmvcli LANGUAGES C VERSION 1.0.0)

add_executable(agmvcli agmvcli.c)
target_link_libraries(agmvcli PRIVATE agmv)
