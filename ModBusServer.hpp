#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>


#include "MODBUSTypes.hpp" 
#include "DataModel.hpp"
#include "VitualTransportInterfaceing.hpp"
#include "MODBUSFRAME.hpp"
#include <stdexcept>

// Base class for all function handlers
// Each Modbus function code maps to one of these.

class FunctionHandler 
{
public:
    virtual ModbusFrame handle(const ModbusFrame& request , ModbusDataModel& dataModel) = 0;
    virtual ~FunctionHandler() = default;
};

// quick helper – verify request data length

inline void ensureLength(const ModbusFrame& frame, size_t expected)
{
    if (frame.m_data.size() < expected)
        throw std::invalid_argument("small size request");
}






class ReadCoilsHandler : public FunctionHandler 
{

public:

    ModbusFrame handle(const ModbusFrame& request,ModbusDataModel& dataModel ) override 
    {
        ensureLength(request, 4);

        ModbusFrame res;


        uint16_t startAddr = (request.m_data[0] << 8) | request.m_data[1];
        uint16_t count = (request.m_data[2] << 8) | request.m_data[3];

        uint8_t byteCount = (count + 7) / 8;
        
        res.m_slaveId = request.m_slaveId;
        res.m_functionCode = request.m_functionCode;
        
        // allocate byteCount + byteCount field
        res.m_data.assign(byteCount + 1, 0); 
        res.m_data[0] = byteCount;

        for (uint16_t i = 0; i < count; i++) {
            if (dataModel.ReadCoil(startAddr + i)) {
                
                int byteindex = 1 + (i / 8);
                int bitindex  = i % 8;
                res.m_data[byteindex] |= (1 << bitindex);
            }
        }
        return res;
    }
};



class ReadDiscreteInputsHandler : public FunctionHandler {
public:
    ModbusFrame handle(const ModbusFrame& request, ModbusDataModel& dm  ) override
    {
        ensureLength(request, 4);
        uint16_t start = (request.m_data[0] << 8) | request.m_data[1];
        uint16_t count = (request.m_data[2] << 8) | request.m_data[3];

        uint8_t byteCount = (count + 7) / 8;

        ModbusFrame resp;
        resp.m_slaveId = request.m_slaveId;
        resp.m_functionCode = request.m_functionCode;

        resp.m_data.assign(byteCount + 1, 0);

        resp.m_data[0] = byteCount;

        try {
            for (uint16_t i = 0; i < count; i++)
                if (dm.ReadDiscreteInput(start + i))
                {
                    int byteIndex = 1 + (i / 8);
                    int bitIndex = i % 8;
                    resp.m_data[byteIndex] |= (1 << bitIndex);
                }
        } catch (std::out_of_range&) 
        {
            resp.m_functionCode |= 0x80;
            resp.m_data = {static_cast<uint8_t>(ExceptionCode::IllegalDataAddress)};
        }
        return resp;
    }
};


class ReadHoldingRegistersHandler : public FunctionHandler
{
public:
    ModbusFrame handle(const ModbusFrame& request , ModbusDataModel& dataModel) override
    {
        ensureLength(request, 4);

        uint16_t startAddr = (request.m_data[0] << 8) | request.m_data[1];
        uint16_t count = (request.m_data[2] << 8) | request.m_data[3];
        try {
            ModbusFrame res;
            res.m_slaveId = request.m_slaveId;
            res.m_functionCode = request.m_functionCode;
            res.m_data.push_back(static_cast<uint8_t> (count *2)); // Byte count

            for (uint16_t i = 0; i < count; i++)
            {
                uint16_t value = dataModel.ReadHolding(startAddr + i);
                res.m_data.push_back(value >> 8); // High byte
                res.m_data.push_back(value & 0xFF); // Low byte
            }
            return res;
        }
        catch (const std::out_of_range&)
        {   
            ModbusFrame errorFrame;
            errorFrame.m_slaveId = request.m_slaveId;
            errorFrame.m_functionCode = request.m_functionCode | 0x80;
            errorFrame.m_data.push_back(static_cast<uint8_t>(ExceptionCode::IllegalDataAddress));
            return errorFrame;
        }
    }
};


class ReadInputRegistersHandler : public FunctionHandler {
public:
    ModbusFrame handle(const ModbusFrame& request,  ModbusDataModel& dm) override
    {
        ensureLength(request, 4);
        uint16_t start = (request.m_data[0] << 8) |   request.m_data[1];
        uint16_t count = (request.m_data[2] << 8) | request.m_data[3];

        ModbusFrame resp;
        resp.m_slaveId =   request.m_slaveId;
        resp.m_functionCode =  request.m_functionCode;
        resp.m_data.push_back(static_cast<uint8_t> (count * 2) );

        try {
            for (uint16_t i = 0; i < count; i++) 
            {
                uint16_t v = dm.ReadInputRegister(start + i);
                resp.m_data.push_back(v >> 8);
                resp.m_data.push_back(v & 0xFF);
            }
        } catch (std::out_of_range&) 
        {
            resp.m_functionCode |= 0x80;
            resp.m_data = {static_cast<uint8_t>(ExceptionCode::IllegalDataAddress)};
        }
        return resp;
    }
};

class WriteSingleCoilHandler : public FunctionHandler 
{
public:
    ModbusFrame handle(const ModbusFrame& request,  ModbusDataModel& dm) override
    {
        ensureLength(request, 4);

        uint16_t addr = (request.m_data[0] << 8) | request.m_data[1];
        uint16_t value = (request.m_data[2] << 8) | request.m_data[3];

   
        if (value != 0x0000 && value != 0xFF00) {
            ModbusFrame err;
            err.m_slaveId = request.m_slaveId;
            err.m_functionCode = request.m_functionCode | 0x80;
            err.m_data = {static_cast<uint8_t>(ExceptionCode::IllegalDataValue)};
            return err;
        }

        bool coilValue = (value == 0xFF00);

        try {
            dm.WriteCoil(addr, coilValue);
        }
        catch (std::out_of_range&) {
            ModbusFrame err;
            err.m_slaveId = request.m_slaveId;
            err.m_functionCode = request.m_functionCode | 0x80;
            err.m_data = {static_cast<uint8_t>(ExceptionCode::IllegalDataAddress)};
            return err;
        }

        return request; 
    }
};


class WriteSingleRegisterHandler : public FunctionHandler
{
public:
    ModbusFrame handle(const ModbusFrame& request , ModbusDataModel& dataModel) override
    {
        ensureLength(request, 4);
        // Parse 
        uint16_t addr = (request.m_data[0] << 8) | request.m_data[1];
        uint16_t value = (request.m_data[2] << 8) | request.m_data[3];

        dataModel.WriteHolding(addr, value);

     
        return request;
    }
};


class WriteMultipleCoilsHandler : public FunctionHandler {
public:
    ModbusFrame handle(const ModbusFrame& request,  ModbusDataModel& dm) override
    {
        ensureLength(request, 5);
        uint16_t start = (request.m_data[0] << 8) |      request.m_data[1];
        uint16_t count = (request.m_data[2] << 8) | request.m_data[3];
        uint8_t byteCount = request.m_data[4];
        
        ensureLength(request, 5 + byteCount);

        if (byteCount != (count + 7) / 8) 
        {
            ModbusFrame err;
            err.m_slaveId = request.m_slaveId;
            err.m_functionCode = request.m_functionCode | 0x80;
            err.m_data = {static_cast<uint8_t>(ExceptionCode::IllegalDataValue)};

            return err;
        }

        try {
            for (uint16_t i = 0; i < count; i++) 
            {
                bool bit = (request.m_data[5 + (i / 8)] & (1 << (i % 8))) != 0;
                dm.WriteCoil(start + i, bit);
            }
        } catch (std::out_of_range&)
         {
            ModbusFrame err;
            err.m_slaveId = request.m_slaveId;
            err.m_functionCode = request.m_functionCode | 0x80;
            err.m_data = {static_cast<uint8_t>(ExceptionCode::IllegalDataAddress)};
            return err;
        }

        ModbusFrame resp;

        resp.m_slaveId = request.m_slaveId;

        resp.m_functionCode = request.m_functionCode;

        resp.m_data.push_back(start >> 8);
        resp.m_data.push_back(start & 0xFF);

        resp.m_data.push_back(count >> 8);
        resp.m_data.push_back(count & 0xFF);
        return resp;
    }
};
class WriteMultipleRegistersHandler : public FunctionHandler
 {
public:
    ModbusFrame handle(const ModbusFrame& request,  ModbusDataModel& dm) override
    {
        ensureLength(request, 5);
        uint16_t start = (request.m_data[0] << 8)   | request.m_data[1];

        uint16_t count = (request.m_data[2] << 8) | request.m_data[3];
        uint8_t byteCount = request.m_data[4];

        ensureLength(request, 5 + byteCount);
        if (byteCount != count * 2) 
        {
            ModbusFrame err;
            err.m_slaveId = request.m_slaveId;
            err.m_functionCode = request.m_functionCode | 0x80;
            err.m_data = {static_cast<uint8_t>(ExceptionCode::IllegalDataValue)};

            return err;
        }
        try {
            for (uint16_t i = 0; i < count; i++)
             {
                uint16_t val = (request.m_data[5 + (i * 2)] << 8) | request.m_data[5 + (i * 2) + 1];
                dm.WriteHolding(start + i, val);
            }
        } catch (std::out_of_range&) {
            ModbusFrame err;
            err.m_slaveId = request.m_slaveId;
            err.m_functionCode = request.m_functionCode | 0x80;
            err.m_data = {static_cast<uint8_t>(ExceptionCode::IllegalDataAddress)};
            return err;
        }

        ModbusFrame resp;
        resp.m_slaveId = request.m_slaveId;
        resp.m_functionCode = request.m_functionCode;
        resp.m_data.push_back(start >> 8);
        resp.m_data.push_back(start & 0xFF);
        resp.m_data.push_back(count >> 8);
        resp.m_data.push_back(count & 0xFF);
        return resp;
    }
};





class ReadExceptionStatusHandler : public FunctionHandler {
public:
    ModbusFrame handle(const ModbusFrame& request,  ModbusDataModel& dm) override
    {
       
        ModbusFrame resp;
        resp.m_slaveId = request.m_slaveId;
        resp.m_functionCode = request.m_functionCode;
        resp.m_data.push_back(0x00); 
        return resp;
    }
};

class DiagnosticsHandler : public FunctionHandler {
public:
    ModbusFrame handle(const ModbusFrame& request,     ModbusDataModel& dm) override
    {
     
        ensureLength(request, 4);
        uint16_t subcode = (request.m_data[0] << 8) | request.m_data[1];
        
        ModbusFrame resp;
        resp.m_slaveId = request.m_slaveId;
        resp.m_functionCode = request.m_functionCode;
 
        resp.m_data = request.m_data;
        return resp;
    }
};

class GetCommEventCounterHandler : public FunctionHandler {
public:
    ModbusFrame handle(const ModbusFrame& request,   ModbusDataModel& dm) override
    {
        
        ModbusFrame resp;
        resp.m_slaveId = request.m_slaveId;
        resp.m_functionCode = request.m_functionCode;

        resp.m_data.push_back(0x00); 
        resp.m_data.push_back(0x00);
        resp.m_data.push_back(0x01); 
        return resp;
    }
};

class GetCommEventLogHandler : public FunctionHandler {
public:
    ModbusFrame handle(const ModbusFrame& request,   ModbusDataModel& dm) override
    {
        ModbusFrame resp;
        resp.m_slaveId = request.m_slaveId;
        resp.m_functionCode = request.m_functionCode;
        resp.m_data.push_back(0x08); 
        resp.m_data.push_back(0x00); 
        resp.m_data.push_back(0x00); 
        resp.m_data.push_back(0x00);
        resp.m_data.push_back(0x00); 
        resp.m_data.push_back(0x00); 
        return resp;
    }
};

class ReCOMServerIDHandler : public FunctionHandler {
public:
    ModbusFrame handle(const ModbusFrame& request,ModbusDataModel& dm) override
    {
        
        ModbusFrame resp;
        resp.m_slaveId = request.m_slaveId;
        resp.m_functionCode = request.m_functionCode;
        
    std::string serverId = "ModbusServerId ";
        uint8_t byteCount = serverId.length() + 1; 
        
        resp.m_data.push_back(byteCount);
     
        
        for (char c : serverId) 
        {
            resp.m_data.push_back(static_cast<uint8_t>(c));

        }
        resp.m_data.push_back(0xFF); 
        return resp;
    }
};

class ReadFileRecordHandler : public FunctionHandler {
public:
    ModbusFrame handle(const ModbusFrame& request, ModbusDataModel& dm) override
    {
        
        ensureLength(request, 2);

        uint8_t byteCount = request.m_data[0];
        
        ModbusFrame resp;
        resp.m_slaveId = request.m_slaveId;

        resp.m_functionCode = request.m_functionCode;

        resp.m_data.push_back(byteCount); 

        resp.m_data.push_back(0x00); 

        resp.m_data.push_back(0x00); 

        return resp;
    }
};

class WriteFileRecordHandler : public FunctionHandler {
public:
    ModbusFrame handle(const ModbusFrame& request,  ModbusDataModel& dm) override
    {
        
        ensureLength(request, 2);
        uint8_t byteCount = request.m_data[0];
        
        ModbusFrame resp;
        resp.m_slaveId = request.m_slaveId;
        resp.m_functionCode = request.m_functionCode;
        resp.m_data.push_back(byteCount); // Echo byte count
        resp.m_data.push_back(0x00); // File record written
        return resp;
    }
};

class MaskWriteRegisterHandler : public FunctionHandler {
public:
    ModbusFrame handle(const ModbusFrame& request,  ModbusDataModel& dm) override
    {
        
        ensureLength(request, 6);
        uint16_t addr = (request.m_data[0] << 8) | request.m_data[1];
        uint16_t andMask = (request.m_data[2] << 8) | request.m_data[3];
        uint16_t orMask = (request.m_data[4] << 8) | request.m_data[5];
        
        try {
            uint16_t current = dm.ReadHolding(addr);
           
            uint16_t newValue = (current & andMask) | orMask;
            dm.WriteHolding(addr, newValue);
        } 
        catch (std::out_of_range&)
         {
            ModbusFrame err;
            err.m_slaveId = request.m_slaveId;
            err.m_functionCode = request.m_functionCode | 0x80;
            err.m_data = {static_cast<uint8_t>(ExceptionCode::IllegalDataAddress)};
            return err;
        }
        
     
        return request;
    }
};

class ReadWriteMultipleRegistersHandler : public FunctionHandler {
public:
    ModbusFrame handle(const ModbusFrame& request,  ModbusDataModel& dm) override
    {
        
        ensureLength(request, 9);
        uint16_t readStart = (request.m_data[0] << 8)   | request.m_data[1];
        uint16_t readCount = (request.m_data[2] << 8) | request.m_data[3];

        uint16_t writeStart = (request.m_data[4] << 8)  | request.m_data[5];
        uint16_t writeCount = (request.m_data[6] << 8) | request.m_data[7];

        uint8_t writeByteCount = request.m_data[8];
        
        try {
          
            for (uint16_t i = 0; i < writeCount; i++) 
            {


                uint16_t val = (request.m_data[9 + (i * 2)] << 8) |request.m_data[9 + (i * 2) + 1];


                dm.WriteHolding(writeStart + i, val);
            }
      
            ModbusFrame resp;
            resp.m_slaveId = request.m_slaveId;
            resp.m_functionCode = request.m_functionCode;

            resp.m_data.push_back(static_cast<uint8_t>(readCount * 2));

            
            for (uint16_t i = 0; i < readCount; i++) 
            {


                uint16_t val = dm.ReadHolding(readStart + i);
                resp.m_data.push_back(val >> 8);
                resp.m_data.push_back(val & 0xFF);


            }
            return resp;

        } catch (std::out_of_range&)
         {
            ModbusFrame err;
            err.m_slaveId = request.m_slaveId;

            err.m_functionCode = request.m_functionCode | 0x80;

            err.m_data = {static_cast<uint8_t>(ExceptionCode::IllegalDataAddress)};
            return err;
        }
    }
};

class ReadFIFOQueueHandler : public FunctionHandler
 {
public:





    ModbusFrame handle(const ModbusFrame& request,   ModbusDataModel& dm) override
    {
        
        ensureLength(request, 2);
        uint16_t fifoAddr = (request.m_data[0] << 8) | request.m_data[1];
        
        ModbusFrame resp;
        resp.m_slaveId = request.m_slaveId;
        resp.m_functionCode = request.m_functionCode;

        resp.m_data.push_back(0x00); 
        resp.m_data.push_back(0x00);
        resp.m_data.push_back(0x00); 
        resp.m_data.push_back(0x00); 
        return resp;
    }
};

class EncapsulatedInterfaceHandler : public FunctionHandler {
public:
    ModbusFrame handle(const ModbusFrame& request,ModbusDataModel& dm) override
    {
       
        ModbusFrame resp;
        resp.m_slaveId = request.m_slaveId;
        resp.m_functionCode = request.m_functionCode;
        resp.m_data = request.m_data;
        return resp;
    }
};





















class HandlerFactory
{
public:
  
   static std::unique_ptr<FunctionHandler>  create(uint8_t functionCode)
    {
    switch (functionCode) 
        {
            case 0x01: return std::make_unique<ReadCoilsHandler>();
            case 0x02: return std::make_unique<ReadDiscreteInputsHandler>();
            case 0x03: return std::make_unique<ReadHoldingRegistersHandler>();
            case 0x04: return std::make_unique<ReadInputRegistersHandler>();
            case 0x05: return std::make_unique<WriteSingleCoilHandler>();
            case 0x06: return std::make_unique<WriteSingleRegisterHandler>();
            case 0x07: return std::make_unique<ReadExceptionStatusHandler>();
            case 0x08: return std::make_unique<DiagnosticsHandler>();
            case 0x0B: return std::make_unique<GetCommEventCounterHandler>();
            case 0x0C: return std::make_unique<GetCommEventLogHandler>();
            case 0x0F: return std::make_unique<WriteMultipleCoilsHandler>();
            case 0x10: return std::make_unique<WriteMultipleRegistersHandler>();
            case 0x11: return std::make_unique<ReCOMServerIDHandler>();
            case 0x14: return std::make_unique<ReadFileRecordHandler>();
            case 0x15: return std::make_unique<WriteFileRecordHandler>();
            case 0x16: return std::make_unique<MaskWriteRegisterHandler>();
            case 0x17: return std::make_unique<ReadWriteMultipleRegistersHandler>();
            case 0x18: return std::make_unique<ReadFIFOQueueHandler>();
            case 0x2B: return std::make_unique<EncapsulatedInterfaceHandler>();
            default:   return nullptr;
        }
}

};


class ModbusServer
{
private:
    CInterfacing_TransCOMing& transCOM;
    ModbusDataModel& dataModel;
    std::map<uint8_t, std::unique_ptr<FunctionHandler>> handlers;
    uint8_t m_ServerAddress;
public:
    ModbusServer(CInterfacing_TransCOMing& t, ModbusDataModel& dm , uint8_t serverAddress) : transCOM(t), dataModel(dm), m_ServerAddress(serverAddress)
    {
        
        for (uint8_t functioncode_id = 1; functioncode_id <= 0x2B; functioncode_id++)
         {
            auto handler = HandlerFactory::create(functioncode_id);
            if (handler) {
                handlers[functioncode_id] = std::move(handler);
            }
        }   

    }
   
    void run()
    {
        while (true)
        {
            try 
            {
            auto raw = transCOM.RecivedData();
            if (raw.empty()) {
                
                continue; 
            }
            ModbusFrame request = ModbusFrame::FrameExtraction (raw);


            if (request.m_slaveId != m_ServerAddress && request.m_slaveId != 0) 
            {
                    continue; 
                }

            auto it = handlers.find(request.m_functionCode);

            if (it != handlers.end())
            {
                ModbusFrame res = it->second->handle(request, dataModel);
               if (res.m_slaveId != 0){
                transCOM.SendData(res.FrameCreation ());
                }
            }
            else
            {
            
                ModbusFrame res;
                res.m_slaveId = request.m_slaveId;
                res.m_functionCode = request.m_functionCode | 0x80; // Set exception 
                res.m_data.push_back(static_cast<uint8_t>(ExceptionCode::IllegalFunction));
                transCOM.SendData(res.FrameCreation ());
            }
        }
        catch (const std::exception& e)
        {
           
            std::cerr << "Error processing Modbus frame: " << e.what() << std::endl;
            continue; 
        }
     }


    }
   
};

    
