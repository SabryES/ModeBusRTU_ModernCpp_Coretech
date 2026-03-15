#pragma once

#include <vector>
#include <cstdint>

// Basic transport interface for communication layer.
// The idea here is that ModbusClient (or any protocol layer)
// shouldn't care if we're using Serial, TCP
// As long as the implementation sends/receives raw bytes.
class CInterfacing_TransCOMing
{
public:

// Sends raw frame bytes to the transport medium.
// Returns true if the data was accepted for sending.
// (Not necessarily delivered yet depending on implementation.)
virtual bool SendData(const std::vector<uint8_t>& data) = 0;

// Reads incoming data from the transport layer.
//now returning a vector directly

virtual std::vector<uint8_t> RecivedData() = 0;

// Virtual destructor is important here since we'll likely delete derived transport objects via this base pointer.

virtual ~CInterfacing_TransCOMing() = default;

};