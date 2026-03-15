#pragma once

#include "VitualTransportInterfaceing.hpp"
#include <windows.h>
#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include "MODBUSFRAME.hpp"

// Simple serial transport implementation for Windows.
// This talks directly to a COM port and pushes raw Modbus frames.
class CSerialTransCOMing : public CInterfacing_TransCOMing
{
private:

HANDLE m_serialHandle;   // handle returned by CreateFile

public:

CSerialTransCOMing(const std::string& comPort)
{
    // Open COM port
    m_serialHandle = CreateFileA(
        comPort.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (m_serialHandle == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Failed to open COM port");
    }

    // Configure serial parameters
    DCB serialCfg = {0};
    serialCfg.DCBlength = sizeof(serialCfg);

    // get current settings first (safer than writing blind)
    GetCommState(m_serialHandle, &serialCfg);

    serialCfg.BaudRate = CBR_9600;
    serialCfg.ByteSize = 8;
    serialCfg.StopBits = ONESTOPBIT;
    serialCfg.Parity   = NOPARITY;

    // apply config
    SetCommState(m_serialHandle, &serialCfg);

    // Setup timeout values
    COMMTIMEOUTS timeouts = {0};

    timeouts.ReadIntervalTimeout         = 50;
    timeouts.ReadTotalTimeoutConstant    = 1000;
    timeouts.ReadTotalTimeoutMultiplier  = 0;

    if (!SetCommTimeouts(m_serialHandle, &timeouts))
    {
        throw std::runtime_error("Failed to configure COM timeouts");
    }

    // Clear buffers just in case
    PurgeComm(m_serialHandle,
              PURGE_RXCLEAR |
              PURGE_TXCLEAR |
              PURGE_RXABORT |
              PURGE_TXABORT);

    std::cout << "COM " << comPort << " opened successfully." << std::endl;
}


~CSerialTransCOMing()
{
    // cleanup handle
    if (m_serialHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_serialHandle);
    }
}


bool SendData(const std::vector<uint8_t>& data) override
{
    DWORD bytesWritten = 0;

    BOOL ok = WriteFile(
        m_serialHandle,
        data.data(),
        static_cast<DWORD>(data.size()),
        &bytesWritten,
        NULL
    );

    

    return ok;
}


// Small debug helper
void logHex(const std::string& header, const std::vector<uint8_t>& bytes)
{
    if (bytes.empty())
        return;

    std::cout << header << ": ";

    for (auto b : bytes)
    {
        printf("%02X ", b);
    }

    std::cout << std::endl;
}


std::vector<uint8_t> RecivedData() override
{
    std::vector<uint8_t> result;

    uint8_t buffer[256];
    DWORD readCount = 0;

    // read first byte (blocking)
    if (ReadFile(m_serialHandle, buffer, 1, &readCount, NULL) && readCount > 0)
    {
        result.push_back(buffer[0]);

        // try reading remaining bytes
        if (ReadFile(m_serialHandle,
                     buffer + 1,
                     sizeof(buffer) - 1,
                     &readCount,
                     NULL))
        {
            if (readCount > 0)
            {
                result.insert(result.end(), buffer + 1, buffer + 1 + readCount);
            }
        }
    }

    // checking
    if (!result.empty())
    {
        std::cout << "[RAW RX]: ";

        for (auto b : result)
        {
            printf("%02X ", b);
        }

        std::cout << std::endl;
    }

    return result;
}

};