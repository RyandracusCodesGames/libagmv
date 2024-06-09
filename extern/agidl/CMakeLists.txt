cmake_minimum_required(VERSION 3.25.1)
project(agidl LANGUAGES C VERSION 1.0.0)

file(GLOB sources CONFIGURE_DEPENDS src/agidl_*.c)

add_library(agidl STATIC ${sources})
target_include_directories(agidl PUBLIC include)
if(UNIX AND NOT APPLE) # Linux
    target_link_libraries(agidl PUBLIC m)
endif()
