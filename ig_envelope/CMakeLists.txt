cmake_minimum_required(VERSION 3.15)
project(ig_envelope)

set(CMAKE_CXX_STANDARD 14)

add_executable(ig_envelope main.cpp)

FIND_PACKAGE(OpenSSL 1.0.2 EXACT REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(ig_envelope  ${OPENSSL_CRYPTO_LIBRARY})

target_compile_options(ig_envelope PUBLIC -fsanitize=undefined)
target_link_options(ig_envelope PUBLIC -fsanitize=undefined)

