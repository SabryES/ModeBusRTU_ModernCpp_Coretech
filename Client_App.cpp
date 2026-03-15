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

    } catch (const std::exception& e) 
    {
        std::cerr << "Client Error: " << e.what() << std::endl;
    }
    return 0;
}