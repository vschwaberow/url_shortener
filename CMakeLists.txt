cmake_minimum_required(VERSION 3.15)
project(url_shortener VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CPM_DOWNLOAD_VERSION 0.40.2)

if(CPM_SOURCE_CACHE)
    set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
elseif(DEFINED ENV{CPM_SOURCE_CACHE})
    set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
else()
    set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
endif()

if(NOT(EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    file(DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
        ${CPM_DOWNLOAD_LOCATION}
    )
endif()

include(${CPM_DOWNLOAD_LOCATION})

CPMAddPackage(
    NAME Crow
    GITHUB_REPOSITORY CrowCpp/Crow
    GIT_TAG v1.0+5
)

CPMAddPackage(
    NAME fmt
    GITHUB_REPOSITORY fmtlib/fmt
    GIT_TAG 11.0.2
)

CPMAddPackage(
    NAME blake3
    GITHUB_REPOSITORY BLAKE3-team/BLAKE3
    GIT_TAG 1.5.4
    DOWNLOAD_ONLY YES
)

find_package(OpenSSL REQUIRED)

set(SQLITE_VERSION 3440000)
set(SQLITE_URL https://sqlite.org/2023/sqlite-amalgamation-${SQLITE_VERSION}.zip)
set(SQLITE_DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/sqlite)
set(SQLITE_EXTRACT_DIR ${SQLITE_DOWNLOAD_DIR}/sqlite-amalgamation-${SQLITE_VERSION})
set(SQLITE_TARGET_DIR ${SQLITE_DOWNLOAD_DIR}/sqlite)

if(NOT EXISTS ${SQLITE_TARGET_DIR}/sqlite3.c)
    if(NOT EXISTS ${SQLITE_EXTRACT_DIR}/sqlite3.c)
        file(DOWNLOAD ${SQLITE_URL} ${SQLITE_DOWNLOAD_DIR}/sqlite.zip SHOW_PROGRESS)
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf sqlite.zip
            WORKING_DIRECTORY ${SQLITE_DOWNLOAD_DIR})
    endif()

    if(NOT EXISTS ${SQLITE_TARGET_DIR})
        file(MAKE_DIRECTORY ${SQLITE_TARGET_DIR})
    endif()

    file(COPY ${SQLITE_EXTRACT_DIR}/sqlite3.c ${SQLITE_EXTRACT_DIR}/sqlite3.h
        DESTINATION ${SQLITE_TARGET_DIR})
endif()

set(BLAKE3_SOURCES
    ${blake3_SOURCE_DIR}/c/blake3.c
    ${blake3_SOURCE_DIR}/c/blake3_dispatch.c
    ${blake3_SOURCE_DIR}/c/blake3_portable.c
)

add_library(sqlite3 STATIC ${SQLITE_TARGET_DIR}/sqlite3.c)
target_include_directories(sqlite3 PUBLIC ${SQLITE_TARGET_DIR})

add_library(blake3 STATIC ${BLAKE3_SOURCES})
target_include_directories(blake3 PUBLIC ${blake3_SOURCE_DIR}/c)
target_compile_definitions(blake3 PRIVATE BLAKE3_NO_SSE2 BLAKE3_NO_SSE41 BLAKE3_NO_AVX2 BLAKE3_NO_AVX512)

add_library(url_shortener_lib STATIC
    src/url_shortener.cc
    src/database.cc
    src/config.cc
)

target_link_libraries(url_shortener_lib PUBLIC
    Crow::Crow
    sqlite3
    OpenSSL::SSL
    OpenSSL::Crypto
    fmt::fmt
    blake3
)

target_include_directories(url_shortener_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SQLITE_TARGET_DIR}
    ${blake3_SOURCE_DIR}/c
)

add_executable(url_shortener src/main.cc)
target_link_libraries(url_shortener PRIVATE url_shortener_lib)

enable_testing()

CPMAddPackage(
    NAME Catch2
    GITHUB_REPOSITORY catchorg/Catch2
    VERSION 3.4.0
)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)

file(GLOB TEST_SOURCES "tests/*.cc")
add_executable(url_shortener_tests ${TEST_SOURCES} tests/main.cc)

target_link_libraries(url_shortener_tests PRIVATE
    url_shortener_lib
    Catch2::Catch2WithMain
)

target_include_directories(url_shortener_tests PRIVATE ${Catch2_SOURCE_DIR}/src)

include(CTest)
include(Catch)
catch_discover_tests(url_shortener_tests)