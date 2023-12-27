#include "netlibprotocol.hpp"
#include "netlibheader.hpp"
#include "crc.hpp"
#include <iostream>
#include <connection.pb.h>
#include <information_packet.pb.h>
#include "registrationrequest.hpp"
#include "registrationresponse.hpp"
#include "tunneldata.hpp"

namespace netlib_proto {

struct  NetLibProtocol::Pimpl
{
    using Crc8Table = CRCPP::CRC::Table<std::uint8_t, 8>;
    using Crc32Table = CRCPP::CRC::Table<std::uint32_t, 32>;

    static const Crc32Table& crc32Table();

    static const Crc8Table& crc8Table();

    static uint8_t calculateHeaderCrc(const BinaryPacketHeader &header) noexcept;

    static std::string encryptXorPacketData(std::string &&data)
    {
        const char key = 'S';

        for (std::size_t i = 0; i < data.size(); ++i)
            data[i] = data[i] ^ key;

        return std::move(data);
    }


    std::string decryptPacketData(BinaryPacketHeader::EncriptionMethod method, std::string&& data) const
    {
        switch (method) {
        case BinaryPacketHeader::EncriptionMethod::Xor:
            return encryptXorPacketData(std::move(data));

        case BinaryPacketHeader::EncriptionMethod::None:
            return std::move(data);

        case BinaryPacketHeader::EncriptionMethod::Unknown: {
            std::cerr << "Unknown encription method" << std::endl;
            return {};
        }

        }

        return {};
    }

    std::string packPacketData(std::string &&resStr, const NetLib::InformationPacket_PacketType packetType,
                                                const uint32_t packetId);


    std::string buffer;
};

void NetLibProtocol::addRawData(const std::string &arr)
{
    m_d->buffer.append(arr);

    constexpr auto headerSize = sizeof(BinaryPacketHeader);
    static_assert(headerSize == 27, "headerSize != 27");

    auto bufferSize = m_d->buffer.size();

    std::size_t startHeaderIndex {0};

    while (headerSize < bufferSize) {


        startHeaderIndex = m_d->buffer.find(reinterpret_cast<const char *>(&BINARY_PACKET_HEADER_PREAMBLE), startHeaderIndex, sizeof(BINARY_PACKET_HEADER_PREAMBLE));
        if (startHeaderIndex == std::string::npos)
            return;

        if (bufferSize - startHeaderIndex < headerSize)
            return;

        auto header = reinterpret_cast<const BinaryPacketHeader *>(m_d->buffer.data() + startHeaderIndex);

        if (header->headerCheckSumm != Pimpl::calculateHeaderCrc(*header)) {
            std::cerr << "wrong crc" << std::endl;
            ++startHeaderIndex;
            continue; // попытка найти следующий заголовок
        }

        const auto isValidPacketVersion = ((header->version >= BinaryPacketHeader::PacketVersion::V1)
                                           && (header->version <= BinaryPacketHeader::PacketVersion::MaxSupportedVersion));

        if (!isValidPacketVersion) {
            std::cerr << "Unsupported protocol version" << std::endl;
            ++startHeaderIndex;
            continue; // попытка найти следующий заголовок
        }

        const auto dataSize = headerSize + header->dataLength;

        if (dataSize > bufferSize) {
            std::cout << "dataSize > bufferSize, return" << std::endl;
            return;
        }

        auto partitionSize = [](auto bufLen) -> std::size_t
        {
            if (bufLen > std::numeric_limits<std::size_t>::max())
                return std::numeric_limits<std::size_t>::max();

            if (std::is_same<std::size_t, decltype(bufLen)>::value)
                return bufLen;

            return static_cast<std::size_t>(bufLen);
        };

        std::size_t rdmpSize = partitionSize(header->dataLength);

        std::uint32_t crc32 = CRCPP::CRC::Calculate(m_d->buffer.data() + startHeaderIndex + headerSize, rdmpSize, Pimpl::crc32Table());
        std::uint64_t calculatedCount = rdmpSize;
        while ((calculatedCount < header->dataLength)) {
            rdmpSize = partitionSize(header->dataLength - calculatedCount);
            calculatedCount += rdmpSize;
            crc32 = CRCPP::CRC::Calculate(m_d->buffer.data() + startHeaderIndex + calculatedCount, rdmpSize, Pimpl::crc32Table(), crc32);
        }

        if (crc32 != header->dataCrc32) {
            std::cerr << "wrong crc. Skip packet" << std::endl;
            ++startHeaderIndex;
            continue; // попытка найти следующий заголовок
        }

        std::string netlibData {m_d->decryptPacketData(header->knownEncriptionMethod(),
                                   std::string(m_d->buffer, startHeaderIndex + headerSize,
                                               header->dataLength))
        };

        /// TODO: deserialize ProtoMessage

        m_d->buffer.erase(0, dataSize + startHeaderIndex);

        NetLib::InformationPacket newPacket;
        const auto parseRes = newPacket.ParsePartialFromArray(netlibData.c_str(), netlibData.size());
        if (parseRes) {
            const auto newType = static_cast<NetLib::InformationPacket_PacketType>(newPacket.type());
            if (newType == NetLib::InformationPacket_PacketType_Ticket) {
                std::cout << "New incoming Notification for id = " << newPacket.id() << std::endl;

                //  TODO: обработать квитанциюю
//                m_d->notificationHandler(newPacket);
            } else {

            }
        }

        bufferSize = m_d->buffer.size();
    }

}

std::string NetLibProtocol::packTransporter(std::unique_ptr<const CustomPacket> packet)
{
    if (!packet)
        return {};

    switch (packet->networkPacketType()) {
    case NetworkPacketType::RegistrationRequest: {
        NetLib::RegistrationRequest r;
        auto regPacket = dynamic_cast<const RegistrationRequest *>(packet.get());
        if (regPacket == nullptr)
            return {};

        r.set_node_name(regPacket->nodeName);
        r.set_password(regPacket->password);
        return r.SerializeAsString();
    }

    case NetworkPacketType::RegistrationResponse: {
        NetLib::RegistrationResponse r;
        auto responsePacket = dynamic_cast<const RegistrationResponse *>(packet.get());
        if (responsePacket == nullptr)
            return {};

        r.set_node_name(responsePacket->nodeName);
        r.set_error_code(responsePacket->errorCode);
        return r.SerializeAsString();
    }

    case NetworkPacketType::TunnelData: {
        NetLib::TunnelData r;
        auto tunnelPacket = dynamic_cast<const TunnelData *>(packet.get());
        if (tunnelPacket == nullptr)
            return {};

        r.set_tunneldata(tunnelPacket->data);
        return r.SerializeAsString();
    }

    default:
        break;
    }

    return {};
}

const NetLibProtocol::Pimpl::Crc32Table &NetLibProtocol::Pimpl::crc32Table()
{
    static const Crc32Table table {CRCPP::CRC::CRC_32()};
    return table;
}

const NetLibProtocol::Pimpl::Crc8Table &NetLibProtocol::Pimpl::crc8Table()
{
    static const Crc8Table table {CRCPP::CRC::CRC_8()};
    return table;
}

uint8_t NetLibProtocol::Pimpl::calculateHeaderCrc(const BinaryPacketHeader &header) noexcept
{
    return CRCPP::CRC::Calculate(&header, sizeof(BinaryPacketHeader) - sizeof(header.headerCheckSumm), Pimpl::crc8Table());
}

std::string NetLibProtocol::Pimpl::packPacketData(std::string &&resStr, const NetLib::InformationPacket_PacketType packetType, const uint32_t packetId)
{

    std::cout << "New outgoing packet with id = " <<  packetId << std::endl;

    NetLib::InformationPacket resPack;

    resPack.set_type(packetType);
    resPack.set_id(packetId);

    resPack.set_data(std::move(resStr));
    return resPack.SerializeAsString();
}


}
