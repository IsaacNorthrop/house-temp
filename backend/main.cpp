#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <mutex>

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

struct Device
{
    float temp;
    float humidity;
    std::string time;
};

class State : public std::enable_shared_from_this<State>
{

    Device out_;
    Device room_;
    Device kitchen_;

public:
    void update_out(float temp, float humidity, std::string time)
    {
        out_.temp = temp;
        out_.humidity = humidity;
        out_.time = time;
    }
    void update_room(float temp, float humidity, std::string time)
    {
        room_.temp = temp;
        room_.humidity = humidity;
        room_.time = time;
    }
    void update_kitchen(float temp, float humidity, std::string time)
    {
        kitchen_.temp = temp;
        kitchen_.humidity = humidity;
        kitchen_.time = time;
    }

    nlohmann::json get()
    {
        nlohmann::json response = {
            {"out", {{"temperature", out_.temp}, {"humidity", out_.humidity}, {"time", out_.time}}},
            {"room", {{"temperature", room_.temp}, {"humidity", room_.humidity}, {"time", room_.time}}},
            {"kitchen", {{"temperature", kitchen_.temp}, {"humidity", kitchen_.humidity}, {"time", kitchen_.time}}}};
        return response;
    }
};

// This function produces an HTTP response for the given request.
http::response<http::string_body>
handle_request(http::request<http::string_body> const &req,
               std::shared_ptr<State> const &state)
{
    if (req.method() == http::verb::get && req.target() == "/api/data")
    {
        // Handle GET request
        nlohmann::json json_response = {{"response", state->get()}};
        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::server, "Beast");
        res.set(http::field::content_type, "application/json");
        res.keep_alive(req.keep_alive());
        res.body() = json_response.dump();
        res.prepare_payload();
        return res;
    }
    else if (req.method() == http::verb::post && req.target() == "/api/data")
    {
        // Handle POST request
        auto json_request = nlohmann::json::parse(req.body());
        std::string response_message = "Received: " + json_request.dump();
        nlohmann::json json_response = {{"message", response_message}};
        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::server, "Beast");
        res.set(http::field::content_type, "application/json");
        res.keep_alive(req.keep_alive());
        res.body() = json_response.dump();
        if (json_request["id"] == 1)
        {
            state->update_out(json_request["temperature"], json_request["humidity"],
                             json_request["time"]);
        }
        if (json_request["id"] == 2)
        {
            state->update_room(json_request["temperature"], json_request["humidity"],
                             json_request["time"]);
        }
        if (json_request["id"] == 3)
        {
            state->update_kitchen(json_request["temperature"], json_request["humidity"],
                             json_request["time"]);
        }
        res.prepare_payload();
        return res;
    }
    // else if (req.method() == http::verb::put && req.target() == "/api/data")
    // {
    //     // Handle PUT request
    //     auto json_request = nlohmann::json::parse(req.body());
    //     std::string response_message = "Updated: " + json_request.dump();
    //     nlohmann::json json_response = {{"message", response_message}};
    //     http::response<http::string_body> res{http::status::ok, req.version()};
    //     res.set(http::field::server, "Beast");
    //     res.set(http::field::content_type, "application/json");
    //     res.keep_alive(req.keep_alive());
    //     res.body() = json_response.dump();
    //     res.prepare_payload();
    //     return res;
    // }

    // Default response for unsupported methods
    return http::response<http::string_body>{http::status::bad_request, req.version()};
}

// This class handles an HTTP server connection.
class Session : public std::enable_shared_from_this<Session>
{
    tcp::socket socket_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    std::shared_ptr<State> state_;

public:
    explicit Session(tcp::socket socket, std::shared_ptr<State> const &state) : socket_(std::move(socket)), state_(state) {}

    void run()
    {
        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        http::async_read(socket_, buffer_, req_, [this, self](beast::error_code ec, std::size_t)
                         {
            if (!ec) {
                do_write(handle_request(req_, state_));
            } });
    }

    void do_write(http::response<http::string_body> res)
    {
        auto self(shared_from_this());
        auto sp = std::make_shared<http::response<http::string_body>>(std::move(res));
        http::async_write(socket_, *sp, [this, self, sp](beast::error_code ec, std::size_t)
                          { socket_.shutdown(tcp::socket::shutdown_send, ec); });
    }
};

// This class accepts incoming connections and launches the sessions.
class Listener : public std::enable_shared_from_this<Listener>
{
    net::io_context &ioc_;
    tcp::acceptor acceptor_;
    std::shared_ptr<State> state_;

public:
    Listener(net::io_context &ioc, tcp::endpoint endpoint, std::shared_ptr<State> state)
        : ioc_(ioc), acceptor_(net::make_strand(ioc)), state_(std::move(state))
    {
        beast::error_code ec;

        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if (ec)
        {
            std::cerr << "Open error: " << ec.message() << std::endl;
            return;
        }

        // Allow address reuse
        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        if (ec)
        {
            std::cerr << "Set option error: " << ec.message() << std::endl;
            return;
        }

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if (ec)
        {
            std::cerr << "Bind error: " << ec.message() << std::endl;
            return;
        }

        // Start listening for connections
        acceptor_.listen(net::socket_base::max_listen_connections, ec);
        if (ec)
        {
            std::cerr << "Listen error: " << ec.message() << std::endl;
            return;
        }

        do_accept();
    }

    void run()
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(net::make_strand(ioc_), [this](beast::error_code ec, tcp::socket socket)
                               {
            if (!ec) {
                std::make_shared<Session>(std::move(socket), state_)->run();
            }
            do_accept(); });
    }
};

int main()
{
    try
    {
        auto const address = net::ip::make_address("0.0.0.0");
        unsigned short port = 8080;

        net::io_context ioc{1};

        auto state = std::make_shared<State>();

        auto listener = std::make_shared<Listener>(ioc, tcp::endpoint{address, port}, state);

        listener->run();

        ioc.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}