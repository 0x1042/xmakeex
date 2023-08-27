#include "server.h"

#include <array>
#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>

#include <_types/_uint8_t.h>
#include <asio/awaitable.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/signal_set.hpp>
#include <spdlog/spdlog.h>

#include <asio.hpp>

#include "asio/buffer.hpp"
#include "asio/use_awaitable.hpp"

namespace socks {

constexpr uint8_t ver = 0x05;
constexpr uint8_t suc = 0x00;
constexpr uint8_t rsv = 0x00;
constexpr uint8_t atyp = 0x01;

auto serve(uint16_t port) -> int {
    try {
        asio::io_context context(4);

        asio::signal_set signals(context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) { context.stop(); });

        auto listen = listener(context, port);
        asio::co_spawn(context, std::move(listen), asio::detached);

        context.run();
        std::cerr << "Server done \n";
        spdlog::info("server done");
    } catch (std::exception & e) {
        spdlog::info("server error: {}",e.what());
    }
    return 0;
}

auto listener(asio::io_context & context, uint16_t port) -> asio::awaitable<void> {
    Endpoint endpoint(asio::ip::tcp::v4(), port);

    std::cout << "listen at " << endpoint.address().to_string() << ":" << endpoint.port() << '\n';

    Acceptor acceptor(context, endpoint);

    try {
        for (;;) {
            Socket socket = co_await acceptor.async_accept(asio::use_awaitable);
            std::cout << "incoming request: " << socket.remote_endpoint().address().to_string() << ":"
                      << socket.remote_endpoint().port() << '\n';

            asio::co_spawn(context, session(context, std::move(socket)), asio::detached);
        }
    } catch (std::exception & e) {
        std::cerr << "listener failure: " << e.what() << "\n";
    }
    co_return;
}

auto session(asio::io_context & context, Socket socket) -> asio::awaitable<void> {
    try {
        // read header
        std::array<uint8_t, 2> data{};
        std::size_t len = co_await socket.async_read_some(asio::buffer(data), asio::use_awaitable);
        for (int i = 0; i < len; i++) {
            spdlog::trace("read {} : {} ", i, data.at(i));
        }

        uint8_t ver = data.at(0);
        uint8_t nmethod = data.at(1);

        for (int i = 0; i < nmethod; i++) {
            std::array<uint8_t, 1> methods{};
            std::size_t len = co_await socket.async_read_some(asio::buffer(methods, 1), asio::use_awaitable);
            spdlog::trace("read method {} : {} ", i, methods.at(0));
        }

        // resp
        std::array<uint8_t, 2> resp{ver, suc};
        co_await async_write(socket, asio::buffer(resp, resp.size()), asio::use_awaitable);

        spdlog::trace("write success.");

        std::array<uint8_t, 4> req{};
        std::size_t read_size = co_await socket.async_read_some(asio::buffer(req, 4), asio::use_awaitable);

        spdlog::trace("read request. read_size: {} ver:{} cmd:{} atyp:{}", read_size, req.at(0), req.at(1), req.at(3));

        // read addr
        std::array<uint8_t, 4> addr{};
        co_await socket.async_read_some(asio::buffer(addr, 4), asio::use_awaitable);

        std::ostringstream ips;
        ips << static_cast<uint16_t>(addr[0]) << '.' << static_cast<uint16_t>(addr[1]) << '.'
            << static_cast<uint16_t>(addr[2]) << '.' << static_cast<uint16_t>(addr[3]);

        std::string ip_addr = ips.str();
        spdlog::trace("ip:{}", ip_addr);

        std::array<uint8_t, 2> port_buf{};
        co_await socket.async_read_some(asio::buffer(port_buf, 2), asio::use_awaitable);

        uint16_t port = static_cast<uint16_t>(port_buf[0]) << 8 | static_cast<uint16_t>(port_buf[1]);
        spdlog::trace("port:{}", port);

        // Socket remote_socket(context);
        Socket remote_socket(socket.get_executor());
        co_await connect(remote_socket, ip_addr, port);

        std::array<uint8_t, 10> resp_buf{ver, suc, rsv, atyp, 0, 0, 0, 0, 0, 0};

        co_await async_write(socket, asio::buffer(resp_buf, resp_buf.size()), asio::use_awaitable);
        spdlog::trace("write response success, proxy start");

        // asio::co_spawn(context, proxy(socket, remote_socket), asio::detached);
        asio::co_spawn(socket.get_executor(), proxy(socket, remote_socket), asio::detached);
        // co_await proxy(socket, remote_socket);
        co_await proxy(remote_socket, socket);

        remote_socket.close();
        socket.close();

    } catch (std::exception & e) {
        spdlog::error("session failure: {}", e.what());
    }
    co_return;
}

auto connect(Socket & socket, const std::string & host, uint16_t port) -> asio::awaitable<void> {
    Endpoint endpoint(asio::ip::address::from_string(host), port);
    co_await socket.async_connect(endpoint, asio::use_awaitable);
    spdlog::trace("Connected to the server success.");
    co_return;
}

auto proxy(Socket & from, Socket & to) -> asio::awaitable<void> {
    spdlog::trace("====================proxy=======================.");
    try {
        std::array<char, 4096> buffer;
        for (;;) {
            size_t bytesRead = co_await from.async_read_some(asio::buffer(buffer), asio::use_awaitable);
            spdlog::trace("bytesRead {}", bytesRead);

            if (bytesRead == 0) {
                break;
            }
            co_await async_write(to, asio::buffer(buffer, bytesRead), asio::use_awaitable);
            spdlog::trace("async_write");
        }
    } catch (std::exception & e) {
        spdlog::error("proxy error: {}", e.what());
    }

    co_return;
}

} // namespace socks
