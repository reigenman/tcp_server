set(_target simple_server)

set(HEADER_LIST
    "${SimpleTcpServer_SOURCE_DIR}/include/simple_server/server.hpp"
    "${SimpleTcpServer_SOURCE_DIR}/include/simple_server/acceptor.hpp"
    "${SimpleTcpServer_SOURCE_DIR}/include/simple_server/client.hpp"
    "${SimpleTcpServer_SOURCE_DIR}/include/simple_server/msg_handler.hpp"
    "${SimpleTcpServer_SOURCE_DIR}/include/simple_server/hash_msg_handler.hpp"
    "${SimpleTcpServer_SOURCE_DIR}/include/simple_server/client_registry.hpp"
    "${SimpleTcpServer_SOURCE_DIR}/include/simple_server/client_controller.hpp"
    "${SimpleTcpServer_SOURCE_DIR}/include/simple_server/net.hpp"
    "${SimpleTcpServer_SOURCE_DIR}/include/simple_server/thread_helper.hpp"
)

# Make an automatic library - will be static or dynamic based on user setting
add_library(${_target}
  server.cpp
  acceptor.cpp
  client.cpp
  client_registry.cpp
  client_controller.cpp
  thread_helper.cpp
  ${HEADER_LIST}
)

# We need this directory, and users of our library will need it too
target_include_directories(${_target} PUBLIC ../include)

target_link_libraries(${_target} PUBLIC Boost::boost)

target_compile_features(${_target} PUBLIC cxx_std_17)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
