#pragma once
#include "VitualTransportInterfaceing.hpp"
#include "ModbusFrame.hpp"
#include "ModbusTypes.hpp"

// Small Modbus client wrapper around the transport interface.

class ModbusClient
        {
        private:

        CInterfacing_TransCOMing& transCOM;

        // helper to detect modbus exception responses
        void checkException(const ModbusFrame& resp, uint8_t expectedFc);

        public:

        // constructor just keeps reference to transport
        ModbusClient(CInterfacing_TransCOMing& t) : transCOM(t) {}

        std::vector<bool> readCoils(uint8_t slaveId, uint16_t start, uint16_t count);
        std::vector<bool> readDiscreteInputs(uint8_t slaveId, uint16_t start, uint16_t count);

        void writeSingleCoil(uint8_t slaveId, uint16_t addr, bool value);
        void writeMultipleCoils(uint8_t slaveId, uint16_t start, const std::vector<bool>& values);

        std::vector<uint16_t> ReadHoldingRegisters(uint8_t slaveId, uint16_t start, uint16_t count);
        std::vector<uint16_t> ReadInputRegisters(uint8_t slaveId, uint16_t start, uint16_t count);

        void writeSingleRegister(uint8_t slaveId, uint16_t addr, uint16_t value);
        void writeMultipleRegisters(uint8_t slaveId, uint16_t start, const std::vector<uint16_t>& values);

};

   inline void ModbusClient::checkException(const ModbusFrame& resp, uint8_t expectedFc)
        {
        //  check for exception response function code with MSB set
        if (resp.m_functionCode & 0x80)
        {
        ExceptionCode exc = static_cast<ExceptionCode>(resp.m_data[0]);

            //  throwing runtime_error for now
            throw std::runtime_error("Modbus exception: " + exceptionName(exc));
        }

        //the function code should match what  asked for
        if (resp.m_functionCode != expectedFc)
        {
            throw std::runtime_error("Protocol Error: unexpected function code in response");
        }

    }

inline std::vector<bool> ModbusClient::readCoils(uint8_t slaveId, uint16_t start, uint16_t count)
        {
        ModbusFrame req;

        req.m_slaveId = slaveId;
        req.m_functionCode = static_cast<uint8_t>(CFunctionCode::ReadCoils);

// building payload manually 
    req.m_data = {
        static_cast<uint8_t>(start >> 8),
        static_cast<uint8_t>(start & 0xFF),
        static_cast<uint8_t>(count >> 8),
        static_cast<uint8_t>(count & 0xFF)
    };

        transCOM.SendData(req.FrameCreation());

        auto resp = ModbusFrame::FrameExtraction(transCOM.RecivedData());

        checkException(resp, static_cast<uint8_t>(CFunctionCode::ReadCoils));

        std::vector<bool> coils;

        uint8_t expectedByteCount = (count + 7) / 8;

        if (resp.m_data[0] != expectedByteCount || resp.m_data.size() < (1 + expectedByteCount))
        {
            throw std::runtime_error("Byte count mismatch while reading coils");
        }

        // decode packed bits
        for (uint16_t i = 0; i < count; i++)
        {
            int byteindex = 1 + (i / 8);
            int bitIndex = i % 8;

            bool bit = (resp.m_data[byteindex] >> bitIndex) & 0x01;

            coils.push_back(bit);
        }

        return coils;

}

inline std::vector<bool> ModbusClient::readDiscreteInputs(uint8_t slaveId, uint16_t start, uint16_t count)
        {
        ModbusFrame req;

        req.m_slaveId = slaveId;
        req.m_functionCode = static_cast<uint8_t>(CFunctionCode::ReadDiscreteInputs);

        req.m_data = {
            static_cast<uint8_t>(start >> 8),
            static_cast<uint8_t>(start & 0xFF),
            static_cast<uint8_t>(count >> 8),
            static_cast<uint8_t>(count & 0xFF)
        };

            transCOM.SendData(req.FrameCreation());

            auto resp = ModbusFrame::FrameExtraction(transCOM.RecivedData());

            checkException(resp, static_cast<uint8_t>(CFunctionCode::ReadDiscreteInputs));

            std::vector<bool> inputs;

            uint8_t expectedByteCount = (count + 7) / 8;

            if (resp.m_data[0] != expectedByteCount || resp.m_data.size() < (1 + expectedByteCount))
            {
                throw std::runtime_error("Byte count mismatch");
            }

            for (uint16_t i = 0; i < count; i++)
            {
                int byteIdx = 1 + (i / 8);
                int bitIdx  = i % 8;

                bool bit = (resp.m_data[byteIdx] >> bitIdx) & 0x01;

                inputs.push_back(bit);
            }

            return inputs;

}

inline void ModbusClient::writeSingleCoil(uint8_t slaveId, uint16_t addr, bool value)
        {
        ModbusFrame req;

        req.m_slaveId = slaveId;
        req.m_functionCode = static_cast<uint8_t>(CFunctionCode::WriteSingleCoil);

        uint16_t modbusValue;

        // using standard modbus representation
        if (value)
            modbusValue = 0xFF00;
        else
            modbusValue = 0x0000;

        req.m_data = {
            static_cast<uint8_t>(addr >> 8),
            static_cast<uint8_t>(addr & 0xFF),
            static_cast<uint8_t>(modbusValue >> 8),
            static_cast<uint8_t>(modbusValue & 0xFF)
        };

        transCOM.SendData(req.FrameCreation());

        auto resp = ModbusFrame::FrameExtraction(transCOM.RecivedData());

        checkException(resp, static_cast<uint8_t>(CFunctionCode::WriteSingleCoil));

    }

inline void ModbusClient::writeMultipleCoils(uint8_t slaveId, uint16_t start, const std::vector<bool>& values)
        {
        uint16_t count = values.size();
        uint8_t byteCount = (count + 7) / 8;

        ModbusFrame req;

        req.m_slaveId = slaveId;
        req.m_functionCode = static_cast<uint8_t>(CFunctionCode::WriteMultipleCoils);

        req.m_data = {
            static_cast<uint8_t>(start >> 8),
            static_cast<uint8_t>(start & 0xFF),
            static_cast<uint8_t>(count >> 8),
            static_cast<uint8_t>(count & 0xFF),
            byteCount
        };

        // allocate bytes for packed bits
        req.m_data.insert(req.m_data.end(), byteCount, 0);

        for (uint16_t i = 0; i < count; i++)
        {
            if (values[i])
            {
                int byteIndex = 5 + (i / 8);
                int bitIndex  = i % 8;

                req.m_data[byteIndex] |= (1 << bitIndex);
            }
        }

        transCOM.SendData(req.FrameCreation());

        auto resp = ModbusFrame::FrameExtraction(transCOM.RecivedData());

        checkException(resp, static_cast<uint8_t>(CFunctionCode::WriteMultipleCoils));

            }

inline std::vector<uint16_t> ModbusClient::ReadHoldingRegisters(uint8_t slaveId, uint16_t start, uint16_t count)
        {
        ModbusFrame req;

        req.m_slaveId = slaveId;
        req.m_functionCode = static_cast<uint8_t>(CFunctionCode::ReadHoldingRegisters);

        req.m_data = {
            static_cast<uint8_t>(start >> 8),
            static_cast<uint8_t>(start & 0xFF),
            static_cast<uint8_t>(count >> 8),
            static_cast<uint8_t>(count & 0xFF)
        };

        transCOM.SendData(req.FrameCreation());

        auto resp = ModbusFrame::FrameExtraction(transCOM.RecivedData());

        checkException(resp, static_cast<uint8_t>(CFunctionCode::ReadHoldingRegisters));

        if (resp.m_data.empty())
        {
            throw std::runtime_error("Empty data in holding register response");
        }

        uint8_t expectedByteCount = count * 2;

        if (resp.m_data[0] != expectedByteCount || resp.m_data.size() < (1 + expectedByteCount))
        {
            throw std::runtime_error("Register byte count mismatch");
        }

        std::vector<uint16_t> registers;

        // convert bytes -> uint16
        for (size_t i = 0; i < expectedByteCount; i += 2)
        {
            size_t byteIndex = 1 + i;

            uint16_t val = (resp.m_data[byteIndex] << 8) |
                        (resp.m_data[byteIndex + 1]);

            registers.push_back(val);
        }

return registers;

}

inline std::vector<uint16_t> ModbusClient::ReadInputRegisters(uint8_t slaveId, uint16_t start, uint16_t count)
            {
            ModbusFrame req;

            req.m_slaveId = slaveId;
            req.m_functionCode = static_cast<uint8_t>(CFunctionCode::ReadInputRegisters);

            req.m_data = {
                static_cast<uint8_t>(start >> 8),
                static_cast<uint8_t>(start & 0xFF),
                static_cast<uint8_t>(count >> 8),
                static_cast<uint8_t>(count & 0xFF)
            };

            transCOM.SendData(req.FrameCreation());

            auto resp = ModbusFrame::FrameExtraction(transCOM.RecivedData());

            checkException(resp, static_cast<uint8_t>(CFunctionCode::ReadInputRegisters));

            if (resp.m_data.empty())
            {
                throw std::runtime_error("Empty data");
            }

            uint8_t expectedByteCount = count * 2;

            if (resp.m_data[0] != expectedByteCount || resp.m_data.size() < (1 + expectedByteCount))
            {
                throw std::runtime_error("Byte count mismatch");
            }

            std::vector<uint16_t> registers;

            for (size_t i = 1; i < 1 + expectedByteCount; i += 2)
            {
                uint16_t regVal = (resp.m_data[i] << 8) |
                                resp.m_data[i + 1];

                registers.push_back(regVal);
            }

            return registers;

}

inline void ModbusClient::writeSingleRegister(uint8_t slaveId, uint16_t addr, uint16_t value)
        {
        ModbusFrame req;

        req.m_slaveId = slaveId;
        req.m_functionCode = static_cast<uint8_t>(CFunctionCode::WriteSingleRegister);

        req.m_data = {
            static_cast<uint8_t>(addr >> 8),
            static_cast<uint8_t>(addr & 0xFF),
            static_cast<uint8_t>(value >> 8),
            static_cast<uint8_t>(value & 0xFF)
        };

        transCOM.SendData(req.FrameCreation());

        auto resp = ModbusFrame::FrameExtraction(transCOM.RecivedData());

        checkException(resp, static_cast<uint8_t>(CFunctionCode::WriteSingleRegister));

        }

inline void ModbusClient::writeMultipleRegisters(uint8_t slaveId, uint16_t start, const std::vector<uint16_t>& values)
{
        uint16_t count = values.size();
        uint8_t byteCount = count * 2;

        ModbusFrame req;

        req.m_slaveId = slaveId;
        req.m_functionCode = static_cast<uint8_t>(CFunctionCode::WriteMultipleRegisters);

        req.m_data = {
            static_cast<uint8_t>(start >> 8),
            static_cast<uint8_t>(start & 0xFF),
            static_cast<uint8_t>(count >> 8),
            static_cast<uint8_t>(count & 0xFF),
            byteCount
        };

        // putting values manually 
        for (uint16_t val : values)
        {
            req.m_data.push_back(static_cast<uint8_t>(val >> 8));
            req.m_data.push_back(static_cast<uint8_t>(val & 0xFF));
        }

        transCOM.SendData(req.FrameCreation());

        auto resp = ModbusFrame::FrameExtraction(transCOM.RecivedData());

        checkException(resp, static_cast<uint8_t>(CFunctionCode::WriteMultipleRegisters));

}