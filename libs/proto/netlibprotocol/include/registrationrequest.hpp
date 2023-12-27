#pragma once

#include <string>
#include "custompacket.hpp"

namespace netlib_proto {

///
/// \brief The RegistrationRequest class ОБъект запроса на регистрацию
///
struct RegistrationRequest: public CustomPacket
{
public:
    RegistrationRequest(std::string password, std::string nodeName): CustomPacket(NetworkPacketType::RegistrationRequest),
        password {std::move(password)},
        nodeName {std::move(nodeName)}
    {}

    ~RegistrationRequest() override = default;

    std::string password;
    std::string nodeName;
};

}
