/*
    mc6522.cpp


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

//only data ports are modelled, timer, interrupts and shiftregister are not modelled

#include "misc1.h"
#include <stdio.h>

#include "mc6522.h"

/* Mc6522::Mc6522() : ora(0), ddra(0), orb(0), ddrb(0),
                   cls(ControlLine::NONE)
{
}


Mc6522::~Mc6522()
{
}

*/

void Mc6522::resetIo()
{
    ora = 0;    // output register A
    ddra = 0;   // data direction register A
    orb = 0;    // output register B
    ddrb = 0;   // data direction register B

    cls = ControlLine::NONE; // control lines CA1, CA2, CB1, CB2
    rtc_address = 0;
}

Byte Mc6522::readIo(Word offset)
{
    switch (offset & 0x0f)
    {
        case 0:
            {
                Byte result = readInputB(); // get data from HW-input
                return result;
            }

        case 1:
            {
                Byte result = readInputA(); // get data from HW-input
                return result;        
            }
        case 2:
            return ddrb;

        case 3:
            return ddra;

        case 4:
            return t1c_l;

        case 5:
            return t1c_h;

        case 6:
            return t1l_l;
        
        case 7:
            return t1l_h;

        case 8:
            return t2c_l;

        case 9:
            return t2c_h;

        case 10:
            return sr;

        case 11:
            return acr;

        case 12:
            return pcr;

        case 13:
            return ifr;

        case 14:
            return ier;

        case 15:
            {
                Byte result = readInputA(); // get data from HW-input
                return result;
            }
    }

    return 0;
}

void Mc6522::writeIo(Word offset, Byte val)
{
    switch (offset & 0x0f)
    {
        case 0:
            orb = val & ddrb;            
            writeOutputB(orb); // write output to port-pins
            break;

        case 1:
            ora = val & ddra;
            writeOutputA(ora);
            break;

        case 2:
            ddrb = val;
            break;

        case 3:
            ddra = val;
            break;

        case 4: t1c_l = val; break;
        case 5: t1c_h = val; break;
        case 6: t1l_l = val; break;
        case 7: t1l_h = val; break;
        case 8: t2c_l = val; break;
        case 9: t2c_h = val; break;
        case 10: sr = val; break;
        case 11: acr = val; break;
        case 12: pcr = val; break;
        case 13: ifr = val; break;
        case 14: ier = val; break;

        case 15: 
            ora = val & ddra;
            writeOutputA(ora);
            break;
    }
}

// read data from port-Pins (should be overwritten by subclass)
// if non strobed data input should be read

// RTC 58321 has 4 bit data/address bus D[3:0] but connected to port PA[3:0]
// READ connected to PA4 - negative logic
// WRITE connected to PA5 - negative logic
// ADDRESS WRITE connected to PA6 - negative logic
// STOP connected to PA7 - negative logic

Byte Mc6522::readInputA()
{
    return ora;
}

Byte Mc6522::readInputB()
{
    return orb;
}

// request for data from port-Pins (should be implemented by subclass)
// if strobed data input should be read

void Mc6522::requestInputA()
{
}

void Mc6522::requestInputB()
{
}


// write output to port-Pins (should be overwritten by subclass)

void Mc6522::writeOutputA(Byte val)
{
}

void Mc6522::set_irq_A()
{
}

void Mc6522::set_irq_B()
{
}

void Mc6522::writeOutputB(Byte)
{
}

// generate an active transition on CA1, CA2, CB1 or CB2

void Mc6522::activeTransition(Mc6522::ControlLine control_line)
{

}


// test contol lines CB1 or CB2
// contents of control lines only valid, if used as outputs

bool Mc6522::testControlLine(Mc6522::ControlLine control_line)
{
    return false;
}

