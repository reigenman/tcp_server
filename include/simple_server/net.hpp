#pragma once
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

namespace simple_server {

namespace net = boost::asio;
namespace bs = boost::system;
using tcp = net::ip::tcp;

} //namespace simple_server
