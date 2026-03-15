#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <stdexcept>

// -----------------------------------------------------------------------------
// Base interface for Modbus data model
// This abstraction lets the Modbus server work with any size .
// -----------------------------------------------------------------------------
class ModbusDataModel
{
public:

// ---- Coils ----
virtual bool ReadCoil(uint16_t addr) = 0;
virtual void WriteCoil(uint16_t addr, bool value) = 0;

// ---- Discrete Inputs ----
virtual bool ReadDiscreteInput(uint16_t addr) = 0;

// ---- Input Registers ----
virtual uint16_t ReadInputRegister(uint16_t addr) = 0;

// ---- Holding Registers ----
virtual uint16_t ReadHolding(uint16_t addr) = 0;
virtual void WriteHolding(uint16_t addr, uint16_t value) = 0;

virtual ~ModbusDataModel() = default;

};

// -----------------------------------------------------------------------------
// Simple compile-time sized data model implementation
// The template parameters define how many registers/inputs/coils exist.
// for embedded systems where memory size is fixed.
// -----------------------------------------------------------------------------
template <
size_t HoldingRegisterCount,size_t CoilCount ,size_t DiscreteInputCount, size_t InputRegisterCount>
class CcompilerDataModel : public ModbusDataModel
{
private:

   
    std::array<uint16_t, HoldingRegisterCount> m_holdingRegisters{};
    std::array<bool, CoilCount>                m_coils{};
    std::array<bool, DiscreteInputCount>       m_discreteInputs{};
    std::array<uint16_t, InputRegisterCount>   m_inputRegisters{};

public:

// Holding Registers

uint16_t ReadHolding(uint16_t addr) override
{
    if (addr < HoldingRegisterCount)
    {
        uint16_t val = m_holdingRegisters[addr];
        return val;
    }

    throw std::out_of_range("Invalid holding register address");
}

void WriteHolding(uint16_t addr, uint16_t value) override
{
    if (addr < HoldingRegisterCount)
    {
        m_holdingRegisters[addr] = value;
        return;
    }

    
    throw std::out_of_range("Invalid holding register address");
}


// Coils
bool ReadCoil(uint16_t addr) override
{
    if (addr < CoilCount)
    {
        bool state = m_coils[addr];
        return state;
    }

    throw std::out_of_range("Invalid coil address");
}

void WriteCoil(uint16_t addr, bool value) override
{
    if (addr < CoilCount)
    {
        m_coils[addr] = value;
    }
    else
    {
        throw std::out_of_range("Invalid coil address");
    }
}


// Discrete Inputs

bool ReadDiscreteInput(uint16_t addr) override
{
    if (addr < DiscreteInputCount)
    {
        return m_discreteInputs[addr];
    }

    throw std::out_of_range("Invalid discrete input address");
}

// Input Registers

uint16_t ReadInputRegister(uint16_t addr) override
{
    if (addr < InputRegisterCount)
    {
        uint16_t regVal = m_inputRegisters[addr];
        return regVal;
    }

    throw std::out_of_range("Invalid input register address");
}

};