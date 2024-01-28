cmake_minimum_required(VERSION 3.14...3.28)

project(
  SimpleTcpServer
  VERSION 0.1
  DESCRIPTION "An example project with CMake"
  LANGUAGES CXX)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

  # Let's nicely support folders in IDEs
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Testing only available if this is the main app
  # Note this needs to be done in the main CMakeLists
  # since it calls enable_testing, which must be in the
  # main CMakeLists.
  include(CTest)

endif()

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)

# The compiled library code is here
add_subdirectory(src)

# The executable code is here
add_subdirectory(apps)

# Testing only available if this is the main app
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
   AND BUILD_TESTING)
  add_subdirectory(tests)
endif()
