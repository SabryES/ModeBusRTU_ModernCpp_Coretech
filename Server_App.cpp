#include "Serialnterface.hpp"
#include "DataModel.hpp"
#include "MODBusServer.hpp"
#include <iostream>

using MyDataModel = CcompilerDataModel<100, 100, 100, 100>;

int main() 
{
    try {
        std::string port = "COM1"; // Change to your port
        std::cout << "--- MODBUS SERVER STARTING ---" << std::endl;
        
        CSerialTransCOMing transport(port);
        MyDataModel dataModel;
        
        // Initialize somedata in the server
        dataModel.WriteHolding(0, 1234);
        dataModel.WriteCoil(5, true);

        ModbusServer server(transport, dataModel, 1); // Server ID = 1
        
        std::cout << "Listening on " << port << "..." << std::endl;
        server.run(); // This loop is blocking while(1)
        
    } catch (const std::exception& e) 
    {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
 }
    return 0;
}