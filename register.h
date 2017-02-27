#ifndef REG_H
#define REG_H

#include <stdint.h>

class Reg {
    public:

        void set (uint16_t data) {
            _data = data;
        }

        Reg (uint8_t adr) :
            _adr(adr) {};


        inline uint8_t getAdr() {
            return(_adr);
        }
        inline uint16_t getData () {
            return(_data);
        }

        void applyMask (uint16_t mask) {
            _data &= ~mask;
        }

        void applyData (uint16_t data) {
            _data |= data;
        }

    private:
        uint16_t _data;
        uint8_t _adr;
};


class bitField {

    public:

        bitField (Reg* reg, const uint8_t bitlow, const uint8_t bithi) :
            _reg(reg) ,
            _bitlow (bitlow)
    {
        _fieldmask = (bithi+1-bitlow);
        _bitmask   = _fieldmask << bitlow;
    };

        void set (uint16_t value)  {

            _reg->applyMask(_bitmask);
            _reg->applyData((value & ~_fieldmask) << _bitlow);

        }

        uint16_t get () {
            return  ((_reg->getData() >> _bitlow) & _fieldmask);
        }

        uint8_t getAdr () {
            return (_reg->getAdr());
        }

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
