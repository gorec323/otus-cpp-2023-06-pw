#pragma once

#include <stdint.h>

#pragma pack(push, 1)

namespace netlib_proto
{

constexpr uint32_t BINARY_PACKET_HEADER_PREAMBLE { 0x78788787 };

/// Транспортный заголовок
struct BinaryPacketHeader
{
    /// Существующие версии протокола
    enum class PacketVersion: uint8_t
    {
        Unknown = 0x00,
        V1 = 0x01,
        MaxSupportedVersion = V1
    };

    enum class EncriptionMethod: uint8_t
    {
        Unknown = 0x00,
        None = 0x01,
        Xor = 0x02,
        LastEncriptionMethod = Xor
    };

    uint32_t preamble {BINARY_PACKET_HEADER_PREAMBLE};

    /// Версия заголовка
    PacketVersion version {PacketVersion::V1};

    /// 8 байт - циклический идентификатор пакета
    uint64_t packetIndex;

    /// 8 байт - размер следующего за заголовком бинарного вложения
    uint64_t dataLength {0};

    /// 1 байт метод маскировки данных
    EncriptionMethod encription {EncriptionMethod::None};

    ///
    /// \brief knownEncriptionMethod - возвращает значение в диапазоне значений EncriptionMethod
    /// \return EncriptionMethod
    ///
    EncriptionMethod knownEncriptionMethod() const noexcept;

    /// 4 байта - контрольная сумма данных crc32
    uint32_t dataCrc32 {0};

    /// 1 байт - контрольная сумма заголовка без этого поля
    uint8_t headerCheckSumm {0};

};

BinaryPacketHeader::EncriptionMethod BinaryPacketHeader::knownEncriptionMethod() const noexcept
{
    if ((encription > EncriptionMethod::Unknown) && (encription <= EncriptionMethod::LastEncriptionMethod))
        return encription;

    return EncriptionMethod::Unknown;
}

}

#pragma pack (pop)
