#pragma once

namespace netlib_proto
{
    enum class NetworkPacketType {
        Undefined,
        RegistrationRequest,
        RegistrationResponse,
        TunnelData
    };

    class CustomPacket
    {
    public:
        CustomPacket(NetworkPacketType value):
            m_networkPacketType {value} {}
        virtual ~CustomPacket() = default;

        NetworkPacketType networkPacketType() const noexcept;

    private:
        NetworkPacketType m_networkPacketType {NetworkPacketType::Undefined};
    };

    inline NetworkPacketType CustomPacket::networkPacketType() const noexcept
    {
        return m_networkPacketType;
    }

}
