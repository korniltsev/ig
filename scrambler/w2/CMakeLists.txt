cmake_minimum_required(VERSION 3.4.1)



add_library(
             native-lib


             SHARED


             src/main/cpp/scrambler.c
             src/main/cpp/main.c
             src/main/cpp/native-lib.c
)

target_include_directories(native-lib PRIVATE src/main/cpp/)




target_link_libraries( native-lib dl)


add_executable(ig_scrambler
    src/main/cpp/scrambler.c
    src/main/cpp/main.c
)
target_link_libraries( ig_scrambler dl)