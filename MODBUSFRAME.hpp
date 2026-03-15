#pragma once

#include <vector>
#include <cstdint>
#include <stdexcept>

// Basic CRC16 implementation used by Modbus RTU


inline uint16_t calculateCRC(const uint8_t* buffer, size_t length)
{
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < length; i++)
    {
        uint8_t byte = buffer[i];
        crc ^= static_cast<uint16_t>(byte);

        for (int bit = 0; bit < 8; bit++)
        {
            bool lsbSet = (crc & 0x0001) != 0;

            crc >>= 1;

            if (lsbSet)
            {
                crc ^= 0xA001;
            }
        }
    }

return crc;

}

// Basic Modbus RTU frame container
// Holds slave id + function code + payload data

class ModbusFrame
{
    public:

    uint8_t m_slaveId;
    uint8_t m_functionCode;

    std::vector<uint8_t> m_data;

    static ModbusFrame FrameExtraction(const std::vector<uint8_t>& raw);

    std::vector<uint8_t> FrameCreation() const;

};

// Convert raw RTU bytes to ModbusFrame

inline ModbusFrame ModbusFrame::FrameExtraction(const std::vector<uint8_t>& raw)
{
ModbusFrame frame;

    if (raw.size() < 4)
    {
        // minimal frame = addr + fc + crc
        throw std::invalid_argument("Frame too short");
    }

    frame.m_slaveId     = raw[0];
    frame.m_functionCode = raw[1];

    // everything between FC and CRC is payload
    frame.m_data.assign(raw.begin() + 2, raw.end() - 2);

    // CRC is little-endian in Modbus RTU
    uint16_t recvCrc =
        (static_cast<uint16_t>(raw[raw.size() - 1]) << 8) |
        static_cast<uint16_t>(raw[raw.size() - 2]);

    uint16_t calcCrc = calculateCRC(raw.data(), raw.size() - 2);

    if (recvCrc != calcCrc)
    {
        throw std::invalid_argument("CRC mismatch");
    }

    return frame;

}

// Build raw RTU frame from internal representation

inline  std::vector<uint8_t> ModbusFrame::FrameCreation() const
{
        std::vector<uint8_t> raw;

        // reserve a little space to avoid some reallocations
        raw.reserve(2 + m_data.size() + 2);

        raw.push_back(m_slaveId);
        raw.push_back(m_functionCode);

        // append payload
        raw.insert(raw.end(), m_data.begin(), m_data.end());

        // compute CRC
        uint16_t crc = calculateCRC(raw.data(), raw.size());

        uint8_t crcLow  = static_cast<uint8_t>(crc & 0xFF);
        uint8_t crcHigh = static_cast<uint8_t>((crc >> 8) & 0xFF);

        // Modbus RTU sends low byte first
        raw.push_back(crcLow);
        raw.push_back(crcHigh);

        return raw;

}