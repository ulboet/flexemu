/*
    mc6522.h


    flexemu, an MC6809 emulator running FLEX
    Copyright (C) 1997-2022  W. Schwotzer

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/



#ifndef MC6522_INCLUDED
#define MC6522_INCLUDED

#include <cstdint>
#include <type_traits>
#include "iodevice.h"


class Mc6522 : public IoDevice
{
public:
    enum class ControlLine : Byte
    {
        NONE = 0,
        CA1 = 1,
        CA2 = 2,
        CB1 = 4,
        CB2 = 8,
    };

protected:

    // Internal registers:
    //
    // cra, crb control register A, B
    // ddra, ddrb   data direction register A, B
    // ora, orb output register A, B

    Byte ora, ddra;
    Byte orb, ddrb;
    Byte t1c_l, t1c_h, t1l_l, t1l_h, t2c_l, t2c_h, sr, acr, pcr, ifr, ier;
    Mc6522::ControlLine cls;
    Byte rtc_address;

public:

    // IoDevice interface
    Byte readIo(Word offset) override;
    void writeIo(Word offset, Byte value) override;
    void resetIo() override;
    const char *getName() override
    {
        return "mc6522";
    }
    Word sizeOfIo() override
    {
        return 16;
    }
    const char *getDescription() override { return "VIA"; }
    const char *getClassName() override { return "Mc6522"; }
    const char *getClassDescription() override { return "Versatile Interface Adapter, VIA"; }
    const char *getVendor() override { return "MOS Technology"; }

public:

    // generate an active transition on CA1, CA2, CB1 or CB2
    void activeTransition(Mc6522::ControlLine control_line);

    // test contol line
    bool testControlLine(Mc6522::ControlLine control_line);


protected:

    // read non strobed data
    virtual Byte readInputA();
    virtual Byte readInputB();
    virtual void set_irq_A();
    virtual void set_irq_B();

    // read strobed data
    virtual void requestInputA();
    virtual void requestInputB();

    // write data to port-pins
    virtual void writeOutputA(Byte val);
    virtual void writeOutputB(Byte val);

public:

    Mc6522() = default;
    ~Mc6522() override = default;
};

inline Mc6522::ControlLine operator| (Mc6522::ControlLine lhs,
                                      Mc6522::ControlLine rhs)
{
    using T = std::underlying_type<Mc6522::ControlLine>::type;

    return static_cast<Mc6522::ControlLine>(static_cast<T>(lhs) |
                                            static_cast<T>(rhs));
}

inline Mc6522::ControlLine operator& (Mc6522::ControlLine lhs,
                                      Mc6522::ControlLine rhs)
{
    using T = std::underlying_type<Mc6522::ControlLine>::type;

    return static_cast<Mc6522::ControlLine>(static_cast<T>(lhs) &
                                            static_cast<T>(rhs));
}

inline Mc6522::ControlLine operator|= (Mc6522::ControlLine &lhs,
                                       Mc6522::ControlLine rhs)
{
    return lhs = lhs | rhs;
}

inline Mc6522::ControlLine operator&= (Mc6522::ControlLine &lhs,
                                       Mc6522::ControlLine rhs)
{
    return lhs = lhs & rhs;
}

inline Mc6522::ControlLine operator~ (const Mc6522::ControlLine lhs)
{
    using T = std::underlying_type<Mc6522::ControlLine>::type;

    return static_cast<Mc6522::ControlLine>(~static_cast<T>(lhs));
}

#endif // MC6522_INCLUDED
