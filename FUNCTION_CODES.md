# MODBUS Function Codes Documentation

This document provides comprehensive documentation for all MODBUS function codes supported by this implementation, including usage examples for both client and server sides.

---

## Table of Contents

1. [Read Coils (0x01)](#1-read-coils-0x01)
2. [Read Discrete Inputs (0x02)](#2-read-discrete-inputs-0x02)
3. [Read Holding Registers (0x03)](#3-read-holding-registers-0x03)
4. [Read Input Registers (0x04)](#4-read-input-registers-0x04)
5. [Write Single Coil (0x05)](#5-write-single-coil-0x05)
6. [Write Single Register (0x06)](#6-write-single-register-0x06)
7. [Read Exception Status (0x07)](#7-read-exception-status-0x07)
8. [Diagnostics (0x08)](#8-diagnostics-0x08)
9. [Get Comm Event Counter (0x0B)](#9-get-comm-event-counter-0x0b)
10. [Get Comm Event Log (0x0C)](#10-get-comm-event-log-0x0c)
11. [Write Multiple Coils (0x0F)](#11-write-multiple-coils-0x0f)
12. [Write Multiple Registers (0x10)](#12-write-multiple-registers-0x10)
13. [Report Server ID (0x11)](#13-report-server-id-0x11)
14. [Read File Record (0x14)](#14-read-file-record-0x14)
15. [Write File Record (0x15)](#15-write-file-record-0x15)
16. [Mask Write Register (0x16)](#16-mask-write-register-0x16)
17. [Read/Write Multiple Registers (0x17)](#17-readwrite-multiple-registers-0x17)
18. [Read FIFO Queue (0x18)](#18-read-fifo-queue-0x18)
19. [Encapsulated Interface Transport (0x2B)](#19-encapsulated-interface-transport-0x2b)

---

## 1. Read Coils (0x01)

### Description
Reads the ON/OFF status of discrete coils (output bits) in a remote device.

### Client Usage

```cpp
// Read 8 coils starting at address 0 from slave ID 1
std::vector<bool> coils = client.readCoils(1, 0, 8);

for (size_t i = 0; i < coils.size(); i++) {
    std::cout << "Coil[" << i << "]: " << (coils[i] ? "ON" : "OFF") << std::endl;
}
```

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x01 |
| 2-3 | Starting Address | First coil address (0x0000 to 0xFFFF) |
| 4-5 | Quantity of Coils | Number of coils to read (1 to 2000) |
| 6-7 | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x01 |
| 2 | Byte Count | Number of data bytes (N) |
| 3 to 3+N-1 | Coil Status | Packed coil values (1 bit per coil) |
| Last 2 bytes | CRC | Error checking |

### Server Handler: `ReadCoilsHandler`

```cpp
// The server automatically handles this via the data model
// Data is read from: dataModel.ReadCoil(address)
```

---

## 2. Read Discrete Inputs (0x02)

### Description
Reads the ON/OFF status of discrete inputs (input bits) in a remote device. Unlike coils, these are read-only.

### Client Usage

```cpp
// Read 4 discrete inputs starting at address 0 from slave ID 1
std::vector<bool> inputs = client.readDiscreteInputs(1, 0, 4);

for (size_t i = 0; i < inputs.size(); i++) {
    std::cout << "Input[" << i << "]: " << (inputs[i] ? "ON" : "OFF") << std::endl;
}
```

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x02 |
| 2-3 | Starting Address | First input address |
| 4-5 | Quantity of Inputs | Number of inputs to read |
| 6-7 | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x02 |
| 2 | Byte Count | Number of data bytes |
| 3+ | Input Status | Packed input values |
| Last 2 bytes | CRC | Error checking |

### Server Handler: `ReadDiscreteInputsHandler`

```cpp
// Data is read from: dataModel.ReadDiscreteInput(address)
```

---

## 3. Read Holding Registers (0x03)

### Description
Reads the contents of holding registers (16-bit read/write registers) in a remote device.

### Client Usage

```cpp
// Read 5 holding registers starting at address 0 from slave ID 1
std::vector<uint16_t> registers = client.ReadHoldingRegisters(1, 0, 5);

for (size_t i = 0; i < registers.size(); i++) {
    std::cout << "Register[" << i << "]: " << registers[i] << std::endl;
}
```

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x03 |
| 2-3 | Starting Address | First register address |
| 4-5 | Quantity of Registers | Number of registers to read (1 to 125) |
| 6-7 | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x03 |
| 2 | Byte Count | Number of data bytes (2 × N) |
| 3+ | Register Values | Register data (2 bytes per register) |
| Last 2 bytes | CRC | Error checking |

### Server Handler: `ReadHoldingRegistersHandler`

```cpp
// Data is read from: dataModel.ReadHolding(address)
```

---

## 4. Read Input Registers (0x04)

### Description
Reads the contents of input registers (16-bit read-only registers) in a remote device.

### Client Usage

```cpp
// Read 3 input registers starting at address 0 from slave ID 1
std::vector<uint16_t> inputRegs = client.ReadInputRegisters(1, 0, 3);

for (size_t i = 0; i < inputRegs.size(); i++) {
    std::cout << "Input Register[" << i << "]: " << inputRegs[i] << std::endl;
}
```

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x04 |
| 2-3 | Starting Address | First register address |
| 4-5 | Quantity of Registers | Number of registers to read |
| 6-7 | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x04 |
| 2 | Byte Count | Number of data bytes |
| 3+ | Register Values | Register data |
| Last 2 bytes | CRC | Error checking |

### Server Handler: `ReadInputRegistersHandler`

```cpp
// Data is read from: dataModel.ReadInputRegister(address)
```

---

## 5. Write Single Coil (0x05)

### Description
Writes a single coil (output bit) to ON or OFF in a remote device.

### Client Usage

```cpp
// Set coil at address 5 to ON (true)
client.writeSingleCoil(1, 5, true);

// Set coil at address 6 to OFF (false)
client.writeSingleCoil(1, 6, false);
```

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x05 |
| 2-3 | Output Address | Coil address |
| 4-5 | Output Value | 0xFF00 = ON, 0x0000 = OFF |
| 6-7 | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x05 |
| 2-3 | Output Address | Echo of request address |
| 4-5 | Output Value | Echo of request value |
| 6-7 | CRC | Error checking |

### Server Handler: `WriteSingleCoilHandler`

```cpp
// Data is written via: dataModel.WriteCoil(address, value)
// Only accepts 0x0000 (OFF) or 0xFF00 (ON)
```

---

## 6. Write Single Register (0x06)

### Description
Writes a single holding register (16-bit value) in a remote device.

### Client Usage

```cpp
// Write value 1234 to register at address 10
client.writeSingleRegister(1, 10, 1234);

// Write value 0xABCD to register at address 20
client.writeSingleRegister(1, 20, 0xABCD);
```

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x06 |
| 2-3 | Register Address | Target register address |
| 4-5 | Register Value | Value to write |
| 6-7 | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x06 |
| 2-3 | Register Address | Echo of request address |
| 4-5 | Register Value | Echo of request value |
| 6-7 | CRC | Error checking |

### Server Handler: `WriteSingleRegisterHandler`

```cpp
// Data is written via: dataModel.WriteHolding(address, value)
```

---

## 7. Read Exception Status (0x07)

### Description
Reads the contents of eight Exception Status outputs in a remote device. This is a legacy function for serial line diagnostics.

### Client Usage

This function code is not directly exposed in the client API but is supported by the server. You can send a raw request:

```cpp
// Would need to construct frame manually or extend client
// The server responds with status byte
```

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x07 |
| 2-3 | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x07 |
| 2 | Exception Status | Status byte |
| 3-4 | CRC | Error checking |

### Server Handler: `ReadExceptionStatusHandler`

```cpp
// Returns fixed value 0x00
```

---

## 8. Diagnostics (0x08)

### Description
Provides a series of tests for checking the communication system between client and server, or for checking various internal error conditions within the server.

### Client Usage

Currently returns echo of request data. Sub-functions include:
- Return Query Data (0x0000)
- Restart Communications Option (0x0001)
- Return Diagnostic Register (0x0002)
- etc.

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x08 |
| 2-3 | Sub-function | Diagnostic test code |
| 4-5 | Data | Test-specific data |
| 6-7 | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x08 |
| 2-3 | Sub-function | Echo of sub-function |
| 4-5 | Data | Echo or result data |
| 6-7 | CRC | Error checking |

### Server Handler: `DiagnosticsHandler`

```cpp
// Echoes back the request data
```

---

## 9. Get Comm Event Counter (0x0B)

### Description
Gets a status word and an event count from the remote device's communication event counter.

### Client Usage

Supported by server but not directly exposed in client API.

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x0B |
| 2-3 | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x0B |
| 2-3 | Status | 0x0000 = Offline, 0xFFFF = Online |
| 4-5 | Event Count | Total events |
| 6-7 | CRC | Error checking |

### Server Handler: `GetCommEventCounterHandler`

```cpp
// Returns: Status = 0x0000, Event Count = 0x0001
```

---

## 10. Get Comm Event Log (0x0C)

### Description
Gets a status word, event count, message count, and a field of event bytes from the remote device.

### Client Usage

Supported by server but not directly exposed in client API.

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x0C |
| 2-3 | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x0C |
| 2 | Byte Count | Total bytes to follow |
| 3-4 | Status | Status word |
| 5-6 | Event Count | Event count |
| 7-8 | Message Count | Message count |
| 9+ | Events | Event bytes |
| Last 2 bytes | CRC | Error checking |

### Server Handler: `GetCommEventLogHandler`

```cpp
// Returns: ByteCount=8, Status=0, EventCount=0, MessageCount=0
```

---

## 11. Write Multiple Coils (0x0F)

### Description
Writes each coil (output bit) in a sequence of coils to either ON or OFF in a remote device.

### Client Usage

```cpp
// Write multiple coils starting at address 0
std::vector<bool> coilValues = {true, false, true, true, false, false, true, true};
client.writeMultipleCoils(1, 0, coilValues);
```

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x0F |
| 2-3 | Starting Address | First coil address |
| 4-5 | Quantity of Outputs | Number of coils to write |
| 6 | Byte Count | Number of data bytes |
| 7+ | Outputs Value | Packed coil values |
| Last 2 bytes | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x0F |
| 2-3 | Starting Address | First coil address written |
| 4-5 | Quantity of Outputs | Number of coils written |
| 6-7 | CRC | Error checking |

### Server Handler: `WriteMultipleCoilsHandler`

```cpp
// Data is written via: dataModel.WriteCoil(address, bit)
// Validates byte count matches expected
```

---

## 12. Write Multiple Registers (0x10)

### Description
Writes a block of contiguous registers (1 to 123 registers) in a remote device.

### Client Usage

```cpp
// Write multiple holding registers starting at address 0
std::vector<uint16_t> values = {100, 200, 300, 400, 500};
client.writeMultipleRegisters(1, 0, values);
```

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x10 |
| 2-3 | Starting Address | First register address |
| 4-5 | Quantity of Registers | Number of registers to write |
| 6 | Byte Count | Number of data bytes (2 × N) |
| 7+ | Registers Value | Register data |
| Last 2 bytes | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x10 |
| 2-3 | Starting Address | First register address written |
| 4-5 | Quantity of Registers | Number of registers written |
| 6-7 | CRC | Error checking |

### Server Handler: `WriteMultipleRegistersHandler`

```cpp
// Data is written via: dataModel.WriteHolding(address, value)
// Validates byte count = count * 2
```

---

## 13. Report Server ID (0x11)

### Description
Returns a description of the type, the current status, and other information specific to a remote device.

### Client Usage

Supported by server but not directly exposed in client API.

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x11 |
| 2-3 | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x11 |
| 2 | Byte Count | Number of bytes to follow |
| 3+ | Server ID | Device identification string |
| Last bytes | Status | 0xFF = Running |
| Last 2 bytes | CRC | Error checking |

### Server Handler: `ReCOMServerIDHandler`

```cpp
// Returns: "ModbusServerId" + 0xFF status byte
```

---

## 14. Read File Record (0x14)

### Description
Performs a file record read. This function code allows reading of file records from devices that support file-based storage.

### Client Usage

Supported by server but not directly exposed in client API.

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x14 |
| 2 | Byte Count | Request data length |
| 3+ | Sub-Request | Reference type, file number, record number, record length |
| Last 2 bytes | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x14 |
| 2 | Byte Count | Response data length |
| 3+ | File Response | Response data |
| Last 2 bytes | CRC | Error checking |

### Server Handler: `ReadFileRecordHandler`

```cpp
// Returns echo of byte count with zeros
```

---

## 15. Write File Record (0x15)

### Description
Performs a file record write. This function code allows writing of file records to devices that support file-based storage.

### Client Usage

Supported by server but not directly exposed in client API.

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x15 |
| 2 | Byte Count | Request data length |
| 3+ | Sub-Request | Reference type, file number, record number, record data |
| Last 2 bytes | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x15 |
| 2 | Byte Count | Response data length |
| 3+ | File Response | Echo of request |
| Last 2 bytes | CRC | Error checking |

### Server Handler: `WriteFileRecordHandler`

```cpp
// Returns echo of byte count with success indicator
```

---

## 16. Mask Write Register (0x16)

### Description
Masks bits in a holding register. This function code allows setting or clearing individual bits within a register without affecting other bits.

### Client Usage

Supported by server but not directly exposed in client API.

### Algorithm
```
Result = (Current_Value AND And_Mask) OR Or_Mask
```

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x16 |
| 2-3 | Reference Address | Register address |
| 4-5 | AND Mask | Bits to preserve (1) or clear (0) |
| 6-7 | OR Mask | Bits to set (1) or leave (0) |
| 8-9 | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x16 |
| 2-3 | Reference Address | Echo of register address |
| 4-5 | AND Mask | Echo of AND mask |
| 6-7 | OR Mask | Echo of OR mask |
| 8-9 | CRC | Error checking |

### Server Handler: `MaskWriteRegisterHandler`

```cpp
// Reads current value, applies masks, writes back
// uint16_t newValue = (current & andMask) | orMask;
// dataModel.WriteHolding(address, newValue);
```

---

## 17. Read/Write Multiple Registers (0x17)

### Description
Performs a combination of one read operation and one write operation in a single MODBUS transaction. This is useful for optimizing communications when you need to read some values and write others atomically.

### Client Usage

Supported by server but not directly exposed in client API.

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x17 |
| 2-3 | Read Starting Address | First register to read |
| 4-5 | Quantity to Read | Number of registers to read |
| 6-7 | Write Starting Address | First register to write |
| 8-9 | Quantity to Write | Number of registers to write |
| 10 | Write Byte Count | Number of write data bytes |
| 11+ | Write Registers Value | Data to write |
| Last 2 bytes | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x17 |
| 2 | Byte Count | Number of read data bytes |
| 3+ | Read Registers Value | Data read from device |
| Last 2 bytes | CRC | Error checking |

### Server Handler: `ReadWriteMultipleRegistersHandler`

```cpp
// First writes: dataModel.WriteHolding(writeStart + i, value)
// Then reads: dataModel.ReadHolding(readStart + i)
// Returns the read values
```

---

## 18. Read FIFO Queue (0x18)

### Description
Reads the contents of a First-In-First-Out (FIFO) queue of register values from a remote device.

### Client Usage

Supported by server but not directly exposed in client API.

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x18 |
| 2-3 | FIFO Pointer Address | Address of FIFO pointer register |
| 4-5 | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x18 |
| 2-3 | Byte Count | Number of data bytes |
| 4-5 | FIFO Count | Number of FIFO registers |
| 6+ | FIFO Value Register | FIFO data |
| Last 2 bytes | CRC | Error checking |

### Server Handler: `ReadFIFOQueueHandler`

```cpp
// Returns empty FIFO response
```

---

## 19. Encapsulated Interface Transport (0x2B)

### Description
Encapsulates the device specific transport mechanism. This is typically used for MODBUS encapsulated protocols like MODBUS Plus or for device identification.

### Client Usage

Supported by server but not directly exposed in client API.

### Request Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Target device address |
| 1 | Function Code | 0x2B |
| 2 | MEI Type | Encapsulated interface type |
| 3+ | Data | Type-specific data |
| Last 2 bytes | CRC | Error checking |

### Response Frame Structure
| Byte | Field | Description |
|------|-------|-------------|
| 0 | Slave Address | Responding device address |
| 1 | Function Code | 0x2B |
| 2 | MEI Type | Echo of MEI type |
| 3+ | Data | Type-specific response |
| Last 2 bytes | CRC | Error checking |

### Server Handler: `EncapsulatedInterfaceHandler`

```cpp
// Echoes back the request data
```

---

## Exception Responses

When a server cannot process a request, it returns an exception response with the function code OR'd with 0x80.

### Exception Codes

| Code | Name | Description |
|------|------|-------------|
| 0x01 | ILLEGAL FUNCTION | Function code not supported |
| 0x02 | ILLEGAL DATA ADDRESS | Address out of range |
| 0x03 | ILLEGAL DATA VALUE | Value not acceptable |
| 0x04 | SERVER DEVICE FAILURE | Server internal error |
| 0x05 | ACKNOWLEDGE | Request accepted, processing |
| 0x06 | SERVER DEVICE BUSY | Server busy, retry later |
| 0x08 | MEMORY PARITY ERROR | Memory parity error |
| 0x0A | GATEWAY PATH UNAVAILABLE | Gateway path unavailable |
| 0x0B | GATEWAY TARGET FAILED | Gateway target failed to respond |

### Client Exception Handling

```cpp
try {
    auto regs = client.ReadHoldingRegisters(1, 0, 1);
} catch (const std::runtime_error& e) {
    std::cerr << "MODBUS Exception: " << e.what() << std::endl;
    // Exception message will contain the exception name
}
```

---

## Summary Table

| FC | Name | Client Method | Server Handler |
|----|------|---------------|----------------|
| 0x01 | Read Coils | `readCoils()` | `ReadCoilsHandler` |
| 0x02 | Read Discrete Inputs | `readDiscreteInputs()` | `ReadDiscreteInputsHandler` |
| 0x03 | Read Holding Registers | `ReadHoldingRegisters()` | `ReadHoldingRegistersHandler` |
| 0x04 | Read Input Registers | `ReadInputRegisters()` | `ReadInputRegistersHandler` |
| 0x05 | Write Single Coil | `writeSingleCoil()` | `WriteSingleCoilHandler` |
| 0x06 | Write Single Register | `writeSingleRegister()` | `WriteSingleRegisterHandler` |
| 0x07 | Read Exception Status | - | `ReadExceptionStatusHandler` |
| 0x08 | Diagnostics | - | `DiagnosticsHandler` |
| 0x0B | Get Comm Event Counter | - | `GetCommEventCounterHandler` |
| 0x0C | Get Comm Event Log | - | `GetCommEventLogHandler` |
| 0x0F | Write Multiple Coils | `writeMultipleCoils()` | `WriteMultipleCoilsHandler` |
| 0x10 | Write Multiple Registers | `writeMultipleRegisters()` | `WriteMultipleRegistersHandler` |
| 0x11 | Report Server ID | - | `ReCOMServerIDHandler` |
| 0x14 | Read File Record | - | `ReadFileRecordHandler` |
| 0x15 | Write File Record | - | `WriteFileRecordHandler` |
| 0x16 | Mask Write Register | - | `MaskWriteRegisterHandler` |
| 0x17 | Read/Write Multiple Registers | - | `ReadWriteMultipleRegistersHandler` |
| 0x18 | Read FIFO Queue | - | `ReadFIFOQueueHandler` |
| 0x2B | Encapsulated Interface | - | `EncapsulatedInterfaceHandler` |