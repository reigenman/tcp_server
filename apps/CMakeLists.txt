set(_target simple-server)

add_executable(${_target} main.cpp)

target_link_libraries(${_target} PRIVATE simple_server Boost::program_options)

install(TARGETS ${_target} DESTINATION bin)
