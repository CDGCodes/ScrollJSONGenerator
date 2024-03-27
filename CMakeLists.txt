cmake_minimum_required(VERSION 3.0)

project(scrolljsongenerator LANGUAGES C)

add_executable(scrolljsongenerator main.c)

target_link_libraries(scrolljsongenerator curl)

target_link_libraries(scrolljsongenerator cjson)

install(TARGETS scrolljsongenerator RUNTIME DESTINATION bin)
