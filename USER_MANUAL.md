# MODBUS RTU User Manual

This comprehensive user manual explains how to use each file in the MODBUS RTU C++ implementation.

---

## Table of Contents

1. [class.hpp - Common Headers](#1-classhpp---common-headers)
2. [MODBUSTypes.hpp - Types and Enumerations](#2-modbustypeshpp---types-and-enumerations)
3. [MODBUSFRAME.hpp - Frame Handling](#3-modbusframehpp---frame-handling)
4. [DataModel.hpp - Data Storage](#4-datamodelhpp---data-storage)
5. [VitualTransportInterfaceing.hpp - Transport Abstraction](#5-vitualtransportinterfaceinghpp---transport-abstraction)
6. [Serialnterface.hpp - Serial Communication](#6-serialnterfacehpp---serial-communication)
7. [ModBusClient.hpp - Client Implementation](#7-modbusclienthpp---client-implementation)
8. [ModBusServer.hpp - Server Implementation](#8-modbusserverhpp---server-implementation)
9. [Client_App.cpp - Client Application](#9-client_appcpp---client-application)
10. [Server_App.cpp - Server Application](#10-server_appcpp---server-application)

---

## 1. class.hpp - Common Headers

### Purpose
This file provides a centralized include file that imports all standard C++ libraries and Windows API headers needed by the MODBUS implementation.

### Usage
Include this file at the top of your source files to get access to commonly used libraries:

```cpp
#include "class.hpp"
```

### Included Libraries
| Library | Purpose |
|---------|---------|
| `<iostream>` | Console input/output |
| `<string>` | String handling |
| `<vector>` | Dynamic arrays |
| `<map>` | Key-value containers |
| `<memory>` | Smart pointers |
| `<stdexcept>` | Exception handling |
| `<windows.h>` | Windows API |

### When to Use
- When you want a quick include for common functionality
- When building Windows-specific applications

### Best Practices
- Include this file first before other project headers
- For production code, consider including only the specific headers you need to reduce compilation time

---

## 2. MODBUSTypes.hpp - Types and Enumerations

### Purpose
Defines all MODBUS-specific types, function codes, exception codes, and error handling enumerations.

### Usage

#### Error Types

```cpp
#include "MODBUSTypes.hpp"

// Check error type
ErrorKind err = ErrorKind::None;

switch (err) {
    case ErrorKind::None:
        // No error
        break;
    case ErrorKind::Timeout:
        // Communication timeout
        break;
    case ErrorKind::CrcError:
        // CRC validation failed
        break;
    case ErrorKind::InvalidFrame:
        // Malformed frame received
        break;
    case ErrorKind::IllegalDataAddress:
        // Address out of range
        break;
    // ... other cases
}
```

#### Function Codes

```cpp
// Get function code value
CFunctionCode fc = CFunctionCode::ReadHoldingRegisters;
uint8_t fcValue = static_cast<uint8_t>(fc); // 0x03

// Available function codes:
// ReadCoils = 0x01
// ReadDiscreteInputs = 0x02
// ReadHoldingRegisters = 0x03
// ReadInputRegisters = 0x04
// WriteSingleCoil = 0x05
// WriteSingleRegister = 0x06
// ... and more
```

#### Exception Handling

```cpp
// Convert exception code to human-readable string
ExceptionCode exc = ExceptionCode::IllegalDataAddress;
std::string name = exceptionName(exc); // Returns "Illegal Data Address"
```

### All Enumerations

#### ErrorKind
| Value | Description |
|-------|-------------|
| `None` | No error |
| `Timeout` | Communication timeout |
| `CrcError` | CRC validation failed |
| `InvalidFrame` | Malformed frame |
| `ExceptionResponse` | MODBUS exception received |
| `IllegalDataAddress` | Address out of range |
| `IllegalDataValue` | Invalid data value |
| `IOError` | Input/output error |
| `BufferOverflow` | Buffer size exceeded |
| `NotImplemented` | Feature not implemented |

#### ExceptionCode
| Value | Code | Description |
|-------|------|-------------|
| `IllegalFunction` | 0x01 | Function not supported |
| `IllegalDataAddress` | 0x02 | Invalid address |
| `IllegalDataValue` | 0x03 | Invalid value |
| `ServerDeviceFailure` | 0x04 | Server error |
| `Acknowledge` | 0x05 | Request accepted |
| `ServerDeviceBusy` | 0x06 | Server busy |
| `MemoryParityError` | 0x08 | Memory error |
| `GatewayPathUnavailable` | 0x0A | Gateway error |
| `GatewayTargetFailed` | 0x0B | Gateway target error |

### Best Practices
- Always use the enum types instead of raw hex values for code clarity
- Use `exceptionName()` for logging and debugging

---

## 3. MODBUSFRAME.hpp - Frame Handling

### Purpose
Provides MODBUS RTU frame construction, parsing, and CRC16 calculation.

### Key Components

#### CRC16 Calculation

```cpp
#include "MODBUSFRAME.hpp"

// Calculate CRC for a buffer
uint8_t buffer[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x0A};
uint16_t crc = calculateCRC(buffer, 6);
```

#### ModbusFrame Class

```cpp
// Create a frame manually
ModbusFrame frame;
frame.m_slaveId = 1;           // Slave address
frame.m_functionCode = 0x03;    // Function code
frame.m_data = {0x00, 0x00, 0x00, 0x0A};  // Payload

// Convert to raw bytes (with CRC)
std::vector<uint8_t> rawFrame = frame.FrameCreation();
// Result: {0x01, 0x03, 0x00, 0x00, 0x00, 0x0A, CRC_LOW, CRC_HIGH}
```

#### Frame Extraction

```cpp
// Parse received raw bytes into a frame
std::vector<uint8_t> receivedData = {/* raw bytes from serial */};

try {
    ModbusFrame frame = ModbusFrame::FrameExtraction(receivedData);
    
    // Access parsed data
    uint8_t slaveId = frame.m_slaveId;
    uint8_t functionCode = frame.m_functionCode;
    std::vector<uint8_t> payload = frame.m_data;
    
} catch (const std::invalid_argument& e) {
    // Handle CRC mismatch or frame too short
    std::cerr << "Frame error: " << e.what() << std::endl;
}
```

### Frame Structure

#### Request Frame (Client to Server)
```
| Slave ID (1 byte) | Function Code (1 byte) | Data (N bytes) | CRC (2 bytes) |
```

#### Response Frame (Server to Client)
```
| Slave ID (1 byte) | Function Code (1 byte) | Data (N bytes) | CRC (2 bytes) |
```

#### Exception Response Frame
```
| Slave ID (1 byte) | Function Code | 0x80 | Exception Code (1 byte) | CRC (2 bytes) |
```

### Example: Building a Read Holding Registers Request

```cpp
// Build request for FC 0x03: Read 10 holding registers starting at address 0
ModbusFrame request;
request.m_slaveId = 1;
request.m_functionCode = static_cast<uint8_t>(CFunctionCode::ReadHoldingRegisters);

// Payload: Start Address (2 bytes) + Quantity (2 bytes)
request.m_data = {
    0x00, 0x00,  // Start address = 0
    0x00, 0x0A   // Quantity = 10
};

// Convert to raw bytes for transmission
std::vector<uint8_t> rawRequest = request.FrameCreation();
```

### Best Practices
- Always use `FrameCreation()` to add CRC automatically
- Always use `FrameExtraction()` to validate CRC when receiving
- Catch `std::invalid_argument` for frame parsing errors

---

## 4. DataModel.hpp - Data Storage

### Purpose
Provides an abstract interface and concrete implementation for MODBUS data storage (coils, discrete inputs, holding registers, input registers).

### Abstract Interface: ModbusDataModel

```cpp
class ModbusDataModel {
public:
    // Coils (read/write bits)
    virtual bool ReadCoil(uint16_t addr) = 0;
    virtual void WriteCoil(uint16_t addr, bool value) = 0;
    
    // Discrete Inputs (read-only bits)
    virtual bool ReadDiscreteInput(uint16_t addr) = 0;
    
    // Input Registers (read-only 16-bit)
    virtual uint16_t ReadInputRegister(uint16_t addr) = 0;
    
    // Holding Registers (read/write 16-bit)
    virtual uint16_t ReadHolding(uint16_t addr) = 0;
    virtual void WriteHolding(uint16_t addr, uint16_t value) = 0;
    
    virtual ~ModbusDataModel() = default;
};
```

### Concrete Implementation: CcompilerDataModel

This template-based implementation uses fixed-size arrays for embedded systems:

```cpp
#include "DataModel.hpp"

// Define a data model with:
// - 100 holding registers
// - 100 coils
// - 100 discrete inputs
// - 100 input registers
using MyDataModel = CcompilerDataModel<100, 100, 100, 100>;

// Create instance
MyDataModel dataModel;

// Write to holding register
dataModel.WriteHolding(0, 1234);

// Read from holding register
uint16_t value = dataModel.ReadHolding(0);  // Returns 1234

// Write to coil
dataModel.WriteCoil(5, true);

// Read from coil
bool coilState = dataModel.ReadCoil(5);  // Returns true

// Read from discrete input (would typically be set by hardware)
bool inputState = dataModel.ReadDiscreteInput(0);

// Read from input register (would typically be set by hardware)
uint16_t inputReg = dataModel.ReadInputRegister(0);
```

### Custom Data Model Implementation

You can create your own data model by inheriting from `ModbusDataModel`:

```cpp
class MyCustomDataModel : public ModbusDataModel {
private:
    // Your internal storage
    std::map<uint16_t, uint16_t> holdingRegisters;
    std::map<uint16_t, bool> coils;
    
public:
    uint16_t ReadHolding(uint16_t addr) override {
        auto it = holdingRegisters.find(addr);
        if (it != holdingRegisters.end())
            return it->second;
        throw std::out_of_range("Invalid address");
    }
    
    void WriteHolding(uint16_t addr, uint16_t value) override {
        holdingRegisters[addr] = value;
    }
    
    // Implement other methods...
};
```

### Data Types Summary

| Type | Access | Size | Description |
|------|--------|------|-------------|
| Coils | Read/Write | 1 bit | Output bits that can be controlled |
| Discrete Inputs | Read Only | 1 bit | Input bits from sensors |
| Holding Registers | Read/Write | 16 bits | Output registers for configuration |
| Input Registers | Read Only | 16 bits | Input registers from sensors |

### Best Practices
- Use `CcompilerDataModel` for fixed-size embedded applications
- Create custom implementations for dynamic or persistent storage
- Always handle `std::out_of_range` exceptions when accessing data

---

## 5. VitualTransportInterfaceing.hpp - Transport Abstraction

### Purpose
Defines the abstract interface for transport layer communication, allowing the MODBUS protocol layer to be independent of the physical transport (serial, TCP, etc.).

### Interface Definition

```cpp
class CInterfacing_TransCOMing {
public:
    // Send raw bytes to transport
    virtual bool SendData(const std::vector<uint8_t>& data) = 0;
    
    // Receive raw bytes from transport
    virtual std::vector<uint8_t> RecivedData() = 0;
    
    virtual ~CInterfacing_TransCOMing() = default;
};
```

### Usage Example

```cpp
#include "VitualTransportInterfaceing.hpp"

// The client/server takes a reference to this interface
CInterfacing_TransCOMing& transport;

// Send data
std::vector<uint8_t> data = {0x01, 0x03, 0x00, 0x00};
transport.SendData(data);

// Receive data
std::vector<uint8_t> received = transport.RecivedData();
```

### Creating Custom Transport

Implement this interface for different transport types:

```cpp
// Example: TCP Transport
class TcpTransport : public CInterfacing_TransCOMing {
private:
    int socketFd;
    
public:
    bool SendData(const std::vector<uint8_t>& data) override {
        // Send via TCP socket
        return send(socketFd, data.data(), data.size(), 0) > 0;
    }
    
    std::vector<uint8_t> RecivedData() override {
        // Receive from TCP socket
        std::vector<uint8_t> buffer(256);
        int n = recv(socketFd, buffer.data(), buffer.size(), 0);
        if (n > 0) {
            buffer.resize(n);
            return buffer;
        }
        return {};
    }
};
```

### Best Practices
- Always pass by reference to avoid slicing
- Ensure thread safety if used in multi-threaded applications
- Handle timeouts appropriately in `RecivedData()`

---

## 6. Serialnterface.hpp - Serial Communication

### Purpose
Implements the Windows serial COM port transport for MODBUS RTU communication.

### Constructor

```cpp
#include "Serialnterface.hpp"

// Open COM port (throws on failure)
CSerialTransCOMing serial("COM1");

// The serial port is automatically configured with:
// - Baud rate: 9600
// - Data bits: 8
// - Stop bits: 1
// - Parity: None
```

### Sending Data

```cpp
std::vector<uint8_t> frame = {/* MODBUS frame */};
bool success = serial.SendData(frame);
```

### Receiving Data

```cpp
std::vector<uint8_t> received = serial.RecivedData();
if (!received.empty()) {
    // Process received data
}
```

### Configuration Details

The serial port is configured with these default settings:

| Parameter | Value |
|-----------|-------|
| Baud Rate | 9600 |
| Data Bits | 8 |
| Stop Bits | 1 |
| Parity | None |
| Read Timeout | 1000ms |
| Read Interval Timeout | 50ms |

### Error Handling

```cpp
try {
    CSerialTransCOMing serial("COM1");
    // Use serial port...
    
} catch (const std::runtime_error& e) {
    std::cerr << "Serial port error: " << e.what() << std::endl;
}
```

### Complete Example

```cpp
#include "Serialnterface.hpp"
#include <iostream>

int main() {
    try {
        // Open COM1
        CSerialTransCOMing transport("COM1");
        
        // Send a MODBUS request
        std::vector<uint8_t> request = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01};
        transport.SendData(request);
        
        // Receive response
        auto response = transport.RecivedData();
        std::cout << "Received " << response.size() << " bytes" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
```

### Platform Notes
- **Windows Only**: Uses Win32 API (`CreateFile`, `WriteFile`, `ReadFile`)
- For Linux/Unix, you would need to implement a similar class using termios

### Best Practices
- Always wrap in try-catch for error handling
- Check if the port opened successfully before using
- Use appropriate timeout values for your application

---

## 7. ModBusClient.hpp - Client Implementation

### Purpose
Provides MODBUS client (master) functionality for communicating with MODBUS servers.

### Constructor

```cpp
#include "ModBusClient.hpp"
#include "Serialnterface.hpp"

// Create transport first
CSerialTransCOMing transport("COM2");

// Create client with reference to transport
ModbusClient client(transport);
```

### Available Methods

#### Read Coils (FC 0x01)

```cpp
// Read 8 coils starting at address 0 from slave ID 1
std::vector<bool> coils = client.readCoils(1, 0, 8);

// Access individual coils
for (size_t i = 0; i < coils.size(); i++) {
    std::cout << "Coil " << i << ": " << (coils[i] ? "ON" : "OFF") << std::endl;
}
```

#### Read Discrete Inputs (FC 0x02)

```cpp
// Read 4 discrete inputs starting at address 0
std::vector<bool> inputs = client.readDiscreteInputs(1, 0, 4);
```

#### Read Holding Registers (FC 0x03)

```cpp
// Read 5 holding registers starting at address 0
std::vector<uint16_t> regs = client.ReadHoldingRegisters(1, 0, 5);

// Access values
for (size_t i = 0; i < regs.size(); i++) {
    std::cout << "Register[" << i << "] = " << regs[i] << std::endl;
}
```

#### Read Input Registers (FC 0x04)

```cpp
// Read 3 input registers starting at address 0
std::vector<uint16_t> inputRegs = client.ReadInputRegisters(1, 0, 3);
```

#### Write Single Coil (FC 0x05)

```cpp
// Turn on coil at address 5
client.writeSingleCoil(1, 5, true);

// Turn off coil at address 6
client.writeSingleCoil(1, 6, false);
```

#### Write Single Register (FC 0x06)

```cpp
// Write value 1234 to register at address 10
client.writeSingleRegister(1, 10, 1234);
```

#### Write Multiple Coils (FC 0x0F)

```cpp
// Write multiple coils starting at address 0
std::vector<bool> coilValues = {true, false, true, true};
client.writeMultipleCoils(1, 0, coilValues);
```

#### Write Multiple Registers (FC 0x10)

```cpp
// Write multiple registers starting at address 0
std::vector<uint16_t> values = {100, 200, 300};
client.writeMultipleRegisters(1, 0, values);
```

### Error Handling

```cpp
try {
    auto regs = client.ReadHoldingRegisters(1, 0, 10);
} catch (const std::runtime_error& e) {
    // Handle MODBUS exceptions and protocol errors
    std::cerr << "MODBUS Error: " << e.what() << std::endl;
}
```

### Complete Client Example

```cpp
#include "Serialnterface.hpp"
#include "ModBusClient.hpp"
#include <iostream>

int main() {
    try {
        // Setup
        CSerialTransCOMing transport("COM2");
        ModbusClient client(transport);
        
        // Read holding registers
        auto regs = client.ReadHoldingRegisters(1, 0, 10);
        std::cout << "Read " << regs.size() << " registers" << std::endl;
        
        // Write a coil
        client.writeSingleCoil(1, 0, true);
        std::cout << "Coil 0 set to ON" << std::endl;
        
        // Write a register
        client.writeSingleRegister(1, 0, 1234);
        std::cout << "Register 0 set to 1234" << std::endl;
        
        // Verify write
        auto verify = client.ReadHoldingRegisters(1, 0, 1);
        std::cout << "Verified value: " << verify[0] << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
```

### Best Practices
- Always handle exceptions for robust communication
- Use appropriate timeout values in the transport layer
- Verify writes by reading back values when critical

---

## 8. ModBusServer.hpp - Server Implementation

### Purpose
Provides MODBUS server (slave) functionality for responding to MODBUS client requests.

### Key Components

#### Function Handlers
Each MODBUS function code has a dedicated handler class:

| Handler | Function Code | Description |
|---------|---------------|-------------|
| `ReadCoilsHandler` | 0x01 | Read coils |
| `ReadDiscreteInputsHandler` | 0x02 | Read discrete inputs |
| `ReadHoldingRegistersHandler` | 0x03 | Read holding registers |
| `ReadInputRegistersHandler` | 0x04 | Read input registers |
| `WriteSingleCoilHandler` | 0x05 | Write single coil |
| `WriteSingleRegisterHandler` | 0x06 | Write single register |
| `ReadExceptionStatusHandler` | 0x07 | Read exception status |
| `DiagnosticsHandler` | 0x08 | Diagnostics |
| `GetCommEventCounterHandler` | 0x0B | Get comm event counter |
| `GetCommEventLogHandler` | 0x0C | Get comm event log |
| `WriteMultipleCoilsHandler` | 0x0F | Write multiple coils |
| `WriteMultipleRegistersHandler` | 0x10 | Write multiple registers |
| `ReCOMServerIDHandler` | 0x11 | Report server ID |
| `ReadFileRecordHandler` | 0x14 | Read file record |
| `WriteFileRecordHandler` | 0x15 | Write file record |
| `MaskWriteRegisterHandler` | 0x16 | Mask write register |
| `ReadWriteMultipleRegistersHandler` | 0x17 | Read/write multiple registers |
| `ReadFIFOQueueHandler` | 0x18 | Read FIFO queue |
| `EncapsulatedInterfaceHandler` | 0x2B | Encapsulated interface |

#### ModbusServer Class

```cpp
#include "ModBusServer.hpp"
#include "Serialnterface.hpp"
#include "DataModel.hpp"

// Define data model type
using MyDataModel = CcompilerDataModel<100, 100, 100, 100>;

int main() {
    // Create transport
    CSerialTransCOMing transport("COM1");
    
    // Create data model
    MyDataModel dataModel;
    
    // Initialize data
    dataModel.WriteHolding(0, 1234);
    dataModel.WriteCoil(0, true);
    
    // Create server with address 1
    ModbusServer server(transport, dataModel, 1);
    
    // Run server (blocking)
    server.run();
    
    return 0;
}
```

### Constructor Parameters

```cpp
ModbusServer(
    CInterfacing_TransCOMing& transport,  // Transport layer
    ModbusDataModel& dataModel,           // Data storage
    uint8_t serverAddress                 // MODBUS slave address
);
```

### Server Behavior

1. **Listen**: Continuously waits for incoming requests
2. **Filter**: Ignores requests not addressed to this server (except broadcast address 0)
3. **Process**: Routes request to appropriate handler
4. **Respond**: Sends response back to client

### Exception Handling

The server automatically generates exception responses for:
- Invalid function codes (0x01 Illegal Function)
- Out-of-range addresses (0x02 Illegal Data Address)
- Invalid data values (0x03 Illegal Data Value)

### Adding Custom Handlers

You can create custom function handlers:

```cpp
class MyCustomHandler : public FunctionHandler {
public:
    ModbusFrame handle(const ModbusFrame& request, ModbusDataModel& dm) override {
        // Process the request
        ModbusFrame response;
        response.m_slaveId = request.m_slaveId;
        response.m_functionCode = request.m_functionCode;
        // ... build response
        return response;
    }
};
```

### Best Practices
- Initialize data model before starting server
- Use appropriate server address (1-247 for standard devices)
- Handle exceptions in the run loop gracefully

---

## 9. Client_App.cpp - Client Application

### Purpose
Example client application demonstrating MODBUS client usage.

### File Content

```cpp
#include "Serialnterface.hpp"
#include "ModBusClient.hpp"
#include <iostream>

int main() {
    try {
        std::string port = "COM2"; // Change to your port
        std::cout << "--- MODBUS CLIENT STARTING ---" << std::endl;

        CSerialTransCOMing transport(port);
        ModbusClient client(transport);

        // 1. Read Holding Registers (Slave ID 1, Start Address 0, Count 1)
        auto regs = client.ReadHoldingRegisters(1, 0, 1);
        std::cout << "Register[0] value: " << regs[0] << std::endl;

        // 2. Write a Coil
        std::cout << "Setting Coil 5 to OFF..." << std::endl;
        client.writeSingleCoil(1, 5, false);

    } catch (const std::exception& e) {
        std::cerr << "Client Error: " << e.what() << std::endl;
    }
    return 0;
}
```

### Compilation

```bash
g++ -std=c++11 Client_App.cpp -o modbus_client.exe
```

### Customization

To modify for your application:

1. **Change COM Port**: Modify the `port` variable
2. **Add More Operations**: Add additional MODBUS calls
3. **Add Configuration**: Read settings from command line or config file

### Extended Example

```cpp
#include "Serialnterface.hpp"
#include "ModBusClient.hpp"
#include <iostream>

int main() {
    try {
        std::string port = "COM2";
        
        CSerialTransCOMing transport(port);
        ModbusClient client(transport);
        
        // Read multiple registers
        auto regs = client.ReadHoldingRegisters(1, 0, 10);
        std::cout << "Read " << regs.size() << " registers:" << std::endl;
        for (size_t i = 0; i < regs.size(); i++) {
            std::cout << "  Reg[" << i << "] = " << regs[i] << std::endl;
        }
        
        // Write to a register
        client.writeSingleRegister(1, 0, 5678);
        std::cout << "Wrote 5678 to register 0" << std::endl;
        
        // Read back to verify
        auto verify = client.ReadHoldingRegisters(1, 0, 1);
        std::cout << "Verified: " << verify[0] << std::endl;
        
        // Read coils
        auto coils = client.readCoils(1, 0, 8);
        std::cout << "Coil states:" << std::endl;
        for (size_t i = 0; i < coils.size(); i++) {
            std::cout << "  Coil[" << i << "] = " << (coils[i] ? "ON" : "OFF") << std::endl;
        }
        
        // Control a coil
        client.writeSingleCoil(1, 0, true);
        std::cout << "Turned coil 0 ON" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
```

---

## 10. Server_App.cpp - Server Application

### Purpose
Example server application demonstrating MODBUS server usage.

### File Content

```cpp
#include "Serialnterface.hpp"
#include "DataModel.hpp"
#include "MODBusServer.hpp"
#include <iostream>

using MyDataModel = CcompilerDataModel<100, 100, 100, 100>;

int main() {
    try {
        std::string port = "COM1"; // Change to your port
        std::cout << "--- MODBUS SERVER STARTING ---" << std::endl;
        
        CSerialTransCOMing transport(port);
        MyDataModel dataModel;
        
        // Initialize some data in the server
        dataModel.WriteHolding(0, 1234);
        dataModel.WriteCoil(5, true);

        ModbusServer server(transport, dataModel, 1); // Server ID = 1
        
        std::cout << "Listening on " << port << "..." << std::endl;
        server.run(); // This loop is blocking while(1)
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
    }
    return 0;
}
```

### Compilation

```bash
g++ -std=c++11 Server_App.cpp -o modbus_server.exe
```

### Server Configuration

Modify the data model type to suit your needs:

```cpp
// 1000 holding registers, 500 coils, 100 discrete inputs, 100 input registers
using MyDataModel = CcompilerDataModel<1000, 500, 100, 100>;
```

### Extended Example with Initial Data

```cpp
#include "Serialnterface.hpp"
#include "DataModel.hpp"
#include "MODBusServer.hpp"
#include <iostream>

using MyDataModel = CcompilerDataModel<100, 100, 100, 100>;

int main() {
    try {
        std::string port = "COM1";
        std::cout << "--- MODBUS SERVER STARTING ---" << std::endl;
        
        CSerialTransCOMing transport(port);
        MyDataModel dataModel;
        
        // Initialize holding registers with values
        for (int i = 0; i < 10; i++) {
            dataModel.WriteHolding(i, i * 100);
        }
        
        // Set some coils
        dataModel.WriteCoil(0, true);
        dataModel.WriteCoil(1, false);
        dataModel.WriteCoil(2, true);
        
        // Set input registers (typically from hardware sensors)
        for (int i = 0; i < 10; i++) {
            dataModel.ReadInputRegister(i);  // Would be set by hardware
        }
        
        // Create and run server
        ModbusServer server(transport, dataModel, 1);
        
        std::cout << "Server ID: 1" << std::endl;
        std::cout << "Holding Registers: 0-99" << std::endl;
        std::cout << "Coils: 0-99" << std::endl;
        std::cout << "Listening on " << port << "..." << std::endl;
        
        server.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
```

### Best Practices
- Set up initial data before calling `run()`
- Use meaningful server addresses
- Log all errors appropriately
- Consider adding signal handling for graceful shutdown

---

## Testing the Implementation

### Hardware Setup

For a complete test, you need:

1. **Two serial ports** (physical or virtual null-modem pair)
2. **COM1**: Server application
3. **COM2**: Client application

### Using Virtual Serial Ports

On Windows, you can use software like:
- com0com (free)
- Virtual Serial Port Driver

Create a virtual null-modem pair (e.g., COM1 ↔ COM2).

### Running the Test

1. Start the server:
   ```bash
   modbus_server.exe
   ```

2. In another terminal, run the client:
   ```bash
   modbus_client.exe
   ```

3. Expected output:
   - Server: Shows "Listening on COM1..."
   - Client: Shows "Register[0] value: 1234"

---

## Troubleshooting

### Common Issues

| Problem | Solution |
|---------|----------|
| "Failed to open COM port" | Check if port exists and is not in use |
| Timeout errors | Check baud rate and wiring |
| CRC errors | Check serial cable quality |
| No response | Verify server address matches |

### Debug Mode

The serial implementation prints raw hex data for debugging:

```
[RAW RX]: 01 03 02 04 D2 B8 44
```

Use this to verify frame content.

---

## Summary

This MODBUS RTU implementation provides:

- **Complete protocol support** for 19 function codes
- **Clean architecture** separating transport, protocol, and data layers
- **Easy-to-use API** for both client and server
- **Windows serial port support** out of the box
- **Extensible design** for custom transports and data models