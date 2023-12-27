#pragma once

#include <memory>
#include <string>
#include <functional>
#include "custompacket.hpp"

namespace netlib_proto {

class NetLibProtocol
{
public:
    NetLibProtocol() noexcept;
    ~NetLibProtocol();

    using NotifyEvent = std::function<void()>;
    void subscribe();

    void addRawData(const std::string &arr);
    std::string packTransporter(std::unique_ptr<const CustomPacket> packet);

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> m_d;

};

}
