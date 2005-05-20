#ifndef ORO_TEMPLATEREGISTER_HPP
#define ORO_TEMPLATEREGISTER_HPP

namespace ORO_DeviceDriver
{
    /**
     * Use this macro to create bit-set enumeration values.
     * This allows to encode the following example :
     * @verbatim
     enum STATUS_bits
     {
         TX_BUFFER_EMPTY = ORO_REG_BIT_DEFN(0,0),
         RX_BUFFER_EMPTY = ORO_REG_BIT_DEFN(1,1),
         TX_UNDERRUN     = ORO_REG_BIT_DEFN(5,5),
         RX_OVERFLOW     = ORO_REG_BIT_DEFN(6,6),
     };@endverbatim
     * Then use the enum value as the last argument for bitRead and
     * bitWrite functions. 
     * @param start The start-bit of a field.
     * @param end   The end-bit of a field.
     */
#define ORO_REG_BIT_DEFN(start, end) (( start << 16)|(end-start+1))
    
    /**
     * A Template to construct register-accessor functions for memory-mapped devices.
     * Use it as a typedef, as in the example below. This specialisation requires an
     * at compile time known base address of the device's register.
     * @param RegType An enum describing the register layout of the device.
     * @verbatim
        enum UARTRegisters
        {
                STATUS = 0x00, // UART status register
                TXCTL  = 0x01, // Transmit control
                RXCTL  = 0x02 // Receive control
        };
        enum STATUS_bits
        {
                TX_BUFFER_EMPTY = ORO_REG_BIT_DEFN(0,0),
                RX_BUFFER_EMPTY = ORO_REG_BIT_DEFN(1,1),
                TX_UNDERRUN     = ORO_REG_BIT_DEFN(5,5),
                RX_OVERFLOW     = ORO_REG_BIT_DEFN(6,6),
        };
        typedef TemplateRegister<UARTRegisters, uint8_t, 0x0000ffff> UARTDevice;
        UARTDevice uart;
        // ...
        uint16_t v = uart.regRead( STATUS );
        bool     b = uart.bitRead( STATUS, TX_BUFFER_EMPTY );
       @endverbatim
     * @param Size The type of a register value. For example, uint8_t if the
     *             register contains 8 bits.
     * @param BaseAddress The at compile time known base address of the register of the device.
     *        If you omit BaseAddress, you need to supply it as the first argument of each
     *        method call of this class.
     */
    template<typename RegType, class Size, unsigned int BaseAddress_ = 0>
    struct TemplateRegister
    {
        typedef unsigned int uint32_t;

        /**
         * The Constructor takes no arguments.
         */
        TemplateRegister() {}

        /**
         * An unsigned int containing the compile-time base address of this register.
         */
        //unsigned int BaseAddress;
        
        /**
         * Resolve the full address of a register.
         */
        inline volatile Size *regAddress(RegType reg)
        {
            return reinterpret_cast<volatile Size*>(BaseAddress_ + reg);
        }

        /**
         * Read a single register of a device.
         */
        inline Size regRead(RegType reg)
        {
            return *regAddress(reg);
        } 

        /**
         * Write a single register of a device.
         */
        inline void regWrite(RegType reg, Size value)
        {
            *regAddress(reg) = value;
        } 

        /**
         * Read one or more (consecutive) bits from a device.
         * @param reg  a value of RegType, denoting from which register to read the bits.
         * @param bits the bits to read, using the ORO_REG_BIT_DEFN defined enum.
         */
        inline uint32_t bitRead( RegType reg, uint32_t bits)
        {
            uint32_t       regval = *regAddress(reg);
            const uint32_t width  = bits & 0xff;
            const uint32_t bitno  = bits >> 16;
            regval >>= bitno;
            regval  &= ((1<<width)-1);
            return regval;
        }

        /**
         * Write one or more (consecutive) bits to a device.
         * @param reg  a value of RegType, denoting from which register to read the bits.
         * @param bits the bits to read, using the ORO_REG_BIT_DEFN defined enum.
         * @param value the value to write to \a bits
         */
        inline void bitWrite(RegType reg, uint32_t bits, uint32_t value)
        {
            uint32_t           regval = *regAddress(reg);
            const uint32_t     width  = bits & 0xff;
            const uint32_t     bitno  = bits >> 16;
            regval &= ~(((1<<width)-1) << bitno);
            regval |=  value << bitno;
            *regAddress(reg) = regval;
        }

    };

    /**
     * A Template to construct register-accessor functions for memory-mapped devices,
     * which address is only known at run-time.
     * Use it as a typedef, as in the example below.
     * @param RegType An enum describing the register layout of the device.
     * @verbatim
        enum UARTRegisters
        {
                STATUS = 0x00, // UART status register
                TXCTL  = 0x01, // Transmit control
                RXCTL  = 0x02 // Receive control
        };
        enum STATUS_bits
        {
                TX_BUFFER_EMPTY = ORO_REG_BIT_DEFN(0,0),
                RX_BUFFER_EMPTY = ORO_REG_BIT_DEFN(1,1),
                TX_UNDERRUN     = ORO_REG_BIT_DEFN(5,5),
                RX_OVERFLOW     = ORO_REG_BIT_DEFN(6,6),
        };
        typedef TemplateRegister<UARTRegisters, uint8_t> UARTDevice;
        // ...
        unsigned int base_address = ...;
        UARTDevice uart( base_address );
        // ...
        uint16_t v = uart.regRead( STATUS );
        bool     b = uart.bitRead( STATUS, TX_BUFFER_EMPTY );
       @endverbatim
     * @param Size The type of a register value. For example, uint8_t if the
     *             register contains 8 bits.
     * @param BaseAddress The at compile time known base address of the register of the device.
     *        If you omit BaseAddress, you need to supply it as the first argument of each
     *        method call of this class.
     */
    template<typename RegType, class Size>
    struct TemplateRegister<RegType,Size,0>
    {
        typedef unsigned int uint32_t;

        /**
         * An unsigned int containing the run-time obtained base address of this register.
         */
        unsigned int BaseAddress;

        /**
         * The constructor takes the \a base_address of the register as an unsigned integer.
         */
        TemplateRegister( unsigned int base_address )
            : BaseAddress(base_address)
        {}

        /**
         * Resolve the full address of a register.
         */
        inline volatile Size *regAddress(RegType reg)
        {
            return reinterpret_cast<volatile Size*>(BaseAddress + reg);
        }

        /**
         * Read a single register of a device.
         */
        inline Size regRead(RegType reg)
        {
            return *regAddress(reg);
        } 

        /**
         * Write a single register of a device.
         */
        inline void regWrite(RegType reg, Size value)
        {
            *regAddress(reg) = value;
        } 

        /**
         * Read one or more (consecutive) bits from a device.
         * @param reg  a value of RegType, denoting from which register to read the bits.
         * @param bits the bits to read, using the ORO_REG_BIT_DEFN defined enum.
         */
        inline uint32_t bitRead( RegType reg, uint32_t bits)
        {
            uint32_t       regval = *regAddress(reg);
            const uint32_t width  = bits & 0xff;
            const uint32_t bitno  = bits >> 16;
            regval >>= bitno;
            regval  &= ((1<<width)-1);
            return regval;
        }

        /**
         * Write one or more (consecutive) bits to a device.
         * @param reg  a value of RegType, denoting from which register to read the bits.
         * @param bits the bits to read, using the ORO_REG_BIT_DEFN defined enum.
         * @param value the value to write to \a bits
         */
        inline void bitWrite(RegType reg, uint32_t bits, uint32_t value)
        {
            uint32_t           regval = *regAddress(reg);
            const uint32_t     width  = bits & 0xff;
            const uint32_t     bitno  = bits >> 16;
            regval &= ~(((1<<width)-1) << bitno);
            regval |=  value << bitno;
            *regAddress(reg) = regval;
        }

    };



}


#endif
