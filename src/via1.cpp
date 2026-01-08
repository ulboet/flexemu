/*
    via1.cpp


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


#include "misc1.h"
#include "via1.h"
#include "schedule.h"
#include "cacttrns.h"
#include "bobserv.h"


Via1::Via1(Scheduler &x_scheduler) : scheduler(x_scheduler)
{
}

void Via1::resetIo()
{
    Mc6522::resetIo();
}

void Via1::set_irq_A()
{
}

void Via1::set_irq_B()
{
}

// RTC 58321 has 4 bit data/address bus D[3:0] but connected to port PA[3:0]
// READ connected to PA4 - negative logic due to inverter in line
// WRITE connected to PA5 - negative logic due to inverter in line
// ADDRESS WRITE connected to PA6 - negative logic due to inverter in line
// STOP connected to PA7 - negative logic due to inverter in line
Byte Via1::readInputA()
{
    if(((ddra & ora) & 0x10) == 0x00)     // PA4/Read active
    {
        struct tm   *lt;
        time_t       time_now;

        // read system time
        time_now = time(nullptr);
        lt = localtime(&time_now);
  
        switch(rtc_address & 0xf)
        {
            case 0: return static_cast<Byte>(lt->tm_sec) % 10; // seconds unit register
            case 1: return static_cast<Byte>(lt->tm_sec) / 10; // seconds tens register
            case 2: return static_cast<Byte>(lt->tm_min) % 10; // minutes unit register
            case 3: return static_cast<Byte>(lt->tm_min) / 10; // minutes tens register
            case 4: return static_cast<Byte>(lt->tm_hour) % 10; // hours units register
            case 5: return static_cast<Byte>(lt->tm_hour) / 10; // hours tens register
            case 6: 
                {       // to be fixed in FLEX
                    Byte weekofday = static_cast<Byte>(lt->tm_wday);
                    if(weekofday==0)
                        return 7;
                    else
                        return weekofday;
                }
            // correct implementation:                     return static_cast<Byte>(lt->tm_wday); // day-of-week register
            case 7: return static_cast<Byte>(lt->tm_mday) % 10; // day units register
            case 8: return static_cast<Byte>(lt->tm_mday) / 10; // day tens register
            case 9: return static_cast<Byte>(lt->tm_mon + 1) % 10; // month units register
            case 10: return static_cast<Byte>(lt->tm_mon + 1) / 10; // month tens register
            case 11: return static_cast<Byte>(lt->tm_year % 100) % 10; // Year units register
            case 12: return static_cast<Byte>(lt->tm_year % 100) / 10; // Year tens register
        }
    }
    return ora;
}

void Via1::writeOutputA(Byte val)
{
    if(((ddra & val) & 0x40) == 0x00)     // PA6/Address Write active
    {
        rtc_address = val & ddra & 0x0f;
        return;
    }
    if(((ddra & val) & 0x20) == 0x00)    // PA5/Write active
    {
        return;  // RTC is not written
    }
    ora = ddra & val;
}