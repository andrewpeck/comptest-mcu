#ifndef REG_H
#define REG_H

#include <stdint.h>
#include <Arduino.h>

class Reg {
    public:

        void set (uint16_t set_data) {
            _data = set_data;
        }

        Reg (uint8_t reg_adr) :
            _adr(reg_adr) {};


        // redundant functions --- should phase out the getXXX

        inline uint8_t getAdr() {
            return(_adr);
        }
        inline uint8_t adr () {
            return(_adr);
        }
        inline uint16_t getData () {
            return (data());
        }
        inline uint16_t data () {
            return(_data);
        }

        void applyMask (uint16_t mask) {
//            SerialUSB.print("&= ~mask: ");
//            SerialUSB.print(mask, HEX);
//            SerialUSB.print("  to data: ");
//            SerialUSB.print(_data, HEX);
//            SerialUSB.print("\r\n");
            _data &= ~mask;
//            SerialUSB.print("  and get: ");
//            SerialUSB.print(_data, HEX);
//            SerialUSB.print("\r\n");
        }

        void applyData (uint16_t set_data) {
//            SerialUSB.print("ORing data: ");
//            SerialUSB.print(set_data, HEX);
//            SerialUSB.print("  to data: ");
            //SerialUSB.print(_data, HEX);
            _data |= set_data;
//            SerialUSB.print("  and get: ");
//            SerialUSB.print(_data, HEX);
//            SerialUSB.print("\r\n");
        }

    private:
        uint16_t _data;
        uint8_t _adr;
};


class bitField {

    public:

        bitField (Reg* reg, const uint8_t bitlow, const uint8_t bithi) :
            _reg(reg) ,
            _bitlow (bitlow),
            _bithi (bithi)
    {
        _fieldmask = (1<< (bithi-bitlow+1))-1;
        _bitmask   = _fieldmask << bitlow;
    };

        void set (uint16_t value)  {
//
//            SerialUSB.print("bitmask: ");
//            SerialUSB.print(_bitmask, HEX);
//            SerialUSB.print("     fieldmask: ");
//            SerialUSB.print(_fieldmask, HEX);
//            SerialUSB.print("     bithi: ");
//            SerialUSB.print(_bithi);
//            SerialUSB.print("     bitlow: ");
//            SerialUSB.print(_bitlow);
//            SerialUSB.print("     setval: ");
//            SerialUSB.print(value);
//            SerialUSB.print("\r\n");

            _reg->applyMask(_bitmask);
            _reg->applyData((value & _fieldmask) << _bitlow);

        }

        uint16_t data () {
            return  ((_reg->data() >> _bitlow) & _fieldmask);
        }

        uint16_t get () {
            return  data();
        }

        inline uint8_t getAdr () { return (_reg->getAdr()); }
        inline uint8_t adr () { return (_reg->getAdr()); }

        Reg* getReg () {
            return (_reg);
        }

    private:
        Reg* _reg;
        uint16_t  _bitmask;
        uint16_t  _fieldmask;
        uint8_t _bitlow;
        uint8_t _bithi;

};
#endif /*REG_H*/
