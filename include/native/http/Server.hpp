#ifndef __NATIVE_HTTP_SERVER_HPP__
#define __NATIVE_HTTP_SERVER_HPP__

#include "../net.hpp"

namespace native {
namespace http {

class request;
class response;

class Server
{
protected:
    Server();
    Server(std::shared_ptr<Loop> iLoop);

public:
    virtual ~Server();

    bool listen(const std::string& ip, int port, std::function<void(request&, response&)> callback);

    static std::shared_ptr<Server> Create();
    static std::shared_ptr<Server> Create(std::shared_ptr<Loop> iLoop);

protected:
    std::shared_ptr<native::net::Tcp> _socket;
    std::weak_ptr<Server> _instance;
};

} // namespace http
} // namespace native

#endif /* __NATIVE_HTTP_SERVER_HPP__ */