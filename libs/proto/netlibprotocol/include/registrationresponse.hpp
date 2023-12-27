#pragma once

#include <string>
#include "custompacket.hpp"

namespace netlib_proto {

///
/// \brief The RegistrationResponse class Объект результата регистрации
///
struct RegistrationResponse: public CustomPacket
{
public:
    RegistrationResponse(std::string nodeName, bool errorCode): CustomPacket(NetworkPacketType::RegistrationResponse),
        nodeName {std::move(nodeName)},
        errorCode {errorCode}
    {}

    ~RegistrationResponse() override = default;

    std::string nodeName;
    std::uint32_t errorCode {0};
};

}
