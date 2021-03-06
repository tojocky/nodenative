#include "native/http.hpp"
#include "native/Loop.hpp"

namespace native {
namespace http {

std::shared_ptr<Server> Server::Create()
{
    std::shared_ptr<Loop> loop = Loop::GetInstanceOrCreateDefault();
    std::shared_ptr<Server> instance(new Server(loop));
    instance->_instance = instance;
    return instance;
}

std::shared_ptr<Server> Server::Create(std::shared_ptr<Loop> iLoop)
{
    std::shared_ptr<Server> instance(new Server(iLoop));
    instance->_instance = instance;
    return instance;
}

Server::Server(std::shared_ptr<Loop> iLoop) : _socket(native::net::Tcp::Create(iLoop))
{
    NNATIVE_FCALL();
}

Server::~Server()
{
    NNATIVE_FCALL();
    if(_socket)
    {
        _socket->close();
    }
}

std::shared_ptr<Server> Server::getInstance() {
    return shared_from_this();
}

bool Server::listen(const std::string& ip, int port, std::function<void(std::shared_ptr<Transaction>)> callback)
{
    NNATIVE_FCALL();
    if(!_socket->bind(ip, port)) {
        NNATIVE_DEBUG("failed to bind to ip " << ip << " port " << port);
        return false;
    }

    std::weak_ptr<Server> instanceWeak = getInstance();

    if(!_socket->listen([instanceWeak, callback](native::Error e) {
        NNATIVE_FCALL();
        NNATIVE_DEBUG("start listen");
        if(e)
        {
            // TODO: handle client connection Error
        }
        else
        {
            std::shared_ptr<Transaction> transaction = Transaction::Create(instanceWeak.lock());
            transaction->parse(callback);
        }
        NNATIVE_DEBUG("end listen");
    })) {
        NNATIVE_DEBUG("failed to listen socket");
        return false;
    }

    return true;
}

Future<std::shared_ptr<Server>> Server::close() {
    std::weak_ptr<Server> instanceWeak = getInstance();

    return _socket->close().then([instanceWeak]() -> std::shared_ptr<Server>{
            std::shared_ptr<Server> instance = instanceWeak.lock();
            instance->_instance.reset();
            return instance;
        });
}

} /* namespace http */
} /* namespace native */
