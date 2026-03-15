#pragma once

#include <cstdint>
#include <string>


// Generic error types used across the Modbus stack.

enum class ErrorKind
{
None,
Timeout,
CrcError,
InvalidFrame,

ExceptionResponse,

IllegalDataAddress,
IllegalDataValue,

IOError,
BufferOverflow,
NotImplemented

};


// Standard Modbus exception codes

enum class ExceptionCode : uint8_t
{
IllegalFunction = 0x01,
IllegalDataAddress = 0x02,
IllegalDataValue = 0x03,
ServerDeviceFailure = 0x04,

Acknowledge            = 0x05,
ServerDeviceBusy       = 0x06,

MemoryParityError      = 0x08,

GatewayPathUnavailable = 0x0A,
GatewayTargetFailed    = 0x0B

};

// Convert exception code --> readable text
inline std::string exceptionName(ExceptionCode exc)
{
switch (exc)
{
case ExceptionCode::IllegalFunction:
return "Illegal Function";

    case ExceptionCode::IllegalDataAddress:
        return "Illegal Data Address";

    case ExceptionCode::IllegalDataValue:
        return "Illegal Data Value";

    case ExceptionCode::ServerDeviceFailure:
        return "Server Device Failure";

    case ExceptionCode::Acknowledge:
        return "Acknowledge";

    case ExceptionCode::ServerDeviceBusy:
        return "Server Device Busy";

    case ExceptionCode::MemoryParityError:
        return "Memory Parity Error";

    case ExceptionCode::GatewayPathUnavailable:
        return "Gateway Path Unavailable";

    case ExceptionCode::GatewayTargetFailed:
        return "Gateway Target Device Failed";

    default:
        // fallback just in case something unexpected appears
        return "Unknown";
}

}


// Modbus Function Codes

enum class CFunctionCode : uint8_t
{
ReadCoils = 0x01,
ReadDiscreteInputs = 0x02,
ReadHoldingRegisters = 0x03,
ReadInputRegisters = 0x04,

WriteSingleCoil            = 0x05,
WriteSingleRegister        = 0x06,

ReadExceptionStatus        = 0x07,
Diagnostics                = 0x08,

GetCommEventCounter        = 0x0B,
GetCommEventLog            = 0x0C,

WriteMultipleCoils         = 0x0F,
WriteMultipleRegisters     = 0x10,

COMServerID                = 0x11,  

ReadFileRecord             = 0x14,
WriteFileRecord            = 0x15,

MaskWriteRegister          = 0x16,
ReadWriteMultipleRegisters = 0x17,

ReadFIFOQueue              = 0x18,

EncapsulatedInterface      = 0x2B

};