#pragma once

#include <string>
#include "custompacket.hpp"

namespace netlib_proto {

///
/// \brief The RegistrationRequest class ОБъект запроса на регистрацию
///
struct TunnelData: public CustomPacket
{
public:
    TunnelData(std::string senderId, std::string correspondentId): CustomPacket(NetworkPacketType::TunnelData),
        senderId {std::move(senderId)},
        correspondentId {std::move(correspondentId)}
    {}

    ~TunnelData() override = default;

    std::string senderId;
    std::string correspondentId;
    std::string data;
};

}
