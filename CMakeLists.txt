cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(Boost_USE_STATIC_LIBS ON)

option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(BUILD_COVERAGE "Build code coverage" OFF)

set(
  HUNTER_CACHE_SERVERS
  "https://github.com/bmstu-iu8-cpp-sem-3/hunter-cache"
  CACHE STRING "Default cache server"
)

include("cmake/HunterGate.cmake")

HunterGate(
  URL "https://github.com/cpp-pm/hunter/archive/v0.23.314.tar.gz"
  SHA1 "95c47c92f68edb091b5d6d18924baabe02a6962a"
)

# TODO: rename project and delete this comment
project(Lab06 VERSION 0.1.0)
string(APPEND CMAKE_CXX_FLAGS " -pedantic -Werror -Wall -Wextra")
string(APPEND CMAKE_CXX_FLAGS " -Wno-unused-command-line-argument")
string(APPEND CMAKE_CXX_FLAGS " -Wshadow -Wnon-virtual-dtor")

# new versions of cmake contain gtest search files
# commented out to increase the build speed.

hunter_add_package(Boost COMPONENTS filesystem log system thread log)
hunter_add_package(Boost COMPONENTS program_options)
hunter_add_package(nlohmann_json)
hunter_add_package(GTest)

find_package(Boost CONFIG REQUIRED filesystem log log_setup system thread)
find_package(Boost CONFIG REQUIRED program_options)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_library(${PROJECT_NAME} STATIC
  # enum your files and delete this comment
        ${CMAKE_CURRENT_SOURCE_DIR}/sources/BruteForce.cpp)

add_executable(demo
  ${CMAKE_CURRENT_SOURCE_DIR}/demo/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sources/BruteForce.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>"
)

target_include_directories(${PROJECT_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third-party/PicoSHA2>"
        "$<INSTALL_INTERFACE:third-party/PicoSHA2>"
)

target_link_libraries(demo ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}
        Boost::log
        Boost::log_setup
        Boost::program_options
        Boost::filesystem
        Boost::system
        Boost::thread
)

if(BUILD_TESTS)
  add_executable(tests
    # TODO: enum your files and delete this comment
          ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.cpp
  )
  target_link_libraries(tests ${PROJECT_NAME} GTest::gtest_main)
  enable_testing()
  add_test(NAME unit_tests COMMAND tests)
endif()

if(BUILD_COVERAGE)
  set(ENABLE_COVERAGE ON CACHE BOOL "Enable coverage build." FORCE)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/coverage/cmake")
  find_package(codecov)
  string(APPEND CMAKE_CXX_FLAGS " --coverage")
  add_coverage(${PROJECT_NAME})
  add_coverage(tests)
  coverage_evaluate()
endif()

if(BUILD_DOCS)
  set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/_docs)
  set(DOXYGEN_EXAMPLE_PATH examples)
  set(DOXYGEN_GENERATE_HTML NO)
  set(DOXYGEN_GENERATE_MAN YES)
  find_package(Doxygen)
  doxygen_add_docs(docs include)
  install(DIRECTORY ${DOXYGEN_OUTPUT_DIRECTORY}/man DESTINATION share)
  # add_dependencies(install docs) need to call target docs before install or package
endif()

include(CPackConfig.cmake)

install(TARGETS ${PROJECT_NAME} demo
  EXPORT "${PROJECT_NAME}-targets"
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  INCLUDES DESTINATION include
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)

install(EXPORT "${PROJECT_NAME}-targets"
  NAMESPACE "${PROJECT_NAME}::"
  DESTINATION "lib/cmake"
)
