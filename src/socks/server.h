#pragma once

#include <memory>
#include <string>

#include <asio/awaitable.hpp>

#include <asio.hpp>

namespace socks {

using Socket = asio::ip::tcp::socket;
using Endpoint = asio::ip::tcp::endpoint;
using Acceptor = asio::ip::tcp::acceptor;

using SocketPtr = std::shared_ptr<Socket>;

auto serve(uint16_t port) -> int;

auto listener(asio::io_context & context, uint16_t port) -> asio::awaitable<void>;

auto session(asio::io_context & context, Socket socket) -> asio::awaitable<void>;

auto connect(Socket & socket, const std::string & host, uint16_t port) -> asio::awaitable<void>;

auto proxy(Socket & from, Socket & to) -> asio::awaitable<void>;

} // namespace socks
