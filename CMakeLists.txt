cmake_minimum_required(VERSION 3.25.1)
project(agmv LANGUAGES C VERSION 1.0.0)

add_subdirectory(extern/agidl)

add_library(agmv STATIC
        src/agmv_decode.c
        src/agmv_encode.c
        src/agmv_playback.c
        src/agmv_utils.c
)
target_include_directories(agmv PUBLIC include)
target_link_libraries(agmv PRIVATE agidl)

add_subdirectory(tools/agmvcli)
