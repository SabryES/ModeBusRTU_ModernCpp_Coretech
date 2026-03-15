# MODBUS RTU C++ Implementation

## Overview

This project provides a complete, header-only C++ implementation of the MODBUS RTU protocol, supporting both client (master) and server (slave) functionality. The implementation is designed for Windows platforms using serial COM port communication and provides a clean, modular architecture that separates protocol logic from transport mechanisms.

## Architecture

The codebase follows a layered architecture pattern:

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                         │
│         (Client_App.cpp / Server_App.cpp)                   │
├─────────────────────────────────────────────────────────────┤
│                    Protocol Layer                            │
│         (ModBusClient.hpp / ModBusServer.hpp)               │
├─────────────────────────────────────────────────────────────┤
│                    Frame Layer                               │
│              (MODBUSFRAME.hpp)                              │
├─────────────────────────────────────────────────────────────┤
│                    Data Model Layer                          │
│              (DataModel.hpp)                                │
├─────────────────────────────────────────────────────────────┤
│                    Transport Layer                           │
│   (VitualTransportInterfaceing.hpp / Serialnterface.hpp)   │
├─────────────────────────────────────────────────────────────┤
│                    Types & Constants                         │
│              (MODBUSTypes.hpp)                              │
└─────────────────────────────────────────────────────────────┘
```

## File Structure

| File | Description |
|------|-------------|
| `class.hpp` | Common includes and Windows API headers |
| `MODBUSTypes.hpp` | Enumerations for function codes, exception codes, and error types |
| `MODBUSFRAME.hpp` | Frame creation, extraction, and CRC16 calculation |
| `DataModel.hpp` | Abstract data model interface and template-based implementation |
| `VitualTransportInterfaceing.hpp` | Abstract transport interface for communication abstraction |
| `Serialnterface.hpp` | Windows serial COM port implementation |
| `ModBusClient.hpp` | MODBUS client (master) implementation with all function code methods |
| `ModBusServer.hpp` | MODBUS server (slave) implementation with function handlers |
| `Client_App.cpp` | Example client application |
| `Server_App.cpp` | Example server application |

## Quick Start

### Server Setup

```cpp
#include "Serialnterface.hpp"
#include "DataModel.hpp"
#include "MODBusServer.hpp"

// Define data model with 100 holding registers, 100 coils, 100 discrete inputs, 100 input registers
using MyDataModel = CcompilerDataModel<100, 100, 100, 100>;

int main() {
    CSerialTransCOMing transport("COM1");
    MyDataModel dataModel;
    
    // Initialize data
    dataModel.WriteHolding(0, 1234);
    dataModel.WriteCoil(5, true);
    
    ModbusServer server(transport, dataModel, 1); // Server ID = 1
    server.run(); // Blocking loop
}
```

### Client Setup

```cpp
#include "Serialnterface.hpp"
#include "ModBusClient.hpp"

int main() {
    CSerialTransCOMing transport("COM2");
    ModbusClient client(transport);
    
    // Read holding registers
    auto regs = client.ReadHoldingRegisters(1, 0, 1);
    
    // Write a coil
    client.writeSingleCoil(1, 5, false);
}
```

## Requirements

- Windows OS (uses Win32 API for serial communication)
- C++17 or later
- Serial COM port available
-choose port for server and one for Client and by null modem connect them vertualized

## Building

Compile with any C++ compiler supporting C++17:

```bash
# Server
g++ -std=c++17 Server_App.cpp -o modbus_server.exe

# Client  
g++ -std=c++17 Client_App.cpp -o modbus_client.exe
```
##  Execution 
each Executable file in different terminals 
-terminal 1    :\build\Debug\modbus_server.exe
-terminal 2    :\build\Debug\modbus_client.exe

