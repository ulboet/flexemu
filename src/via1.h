/*
    via1.h

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



#ifndef VIA1_INCLUDED
#define VIA1_INCLUDED

#include "misc1.h"
#include "mc6522.h"
#include "bobservd.h"


class Scheduler;
class BObserver;

class Via1 : public Mc6522, public BObserved
{
protected:

    Scheduler &scheduler;
    
protected:

    void set_irq_A() override;
    void set_irq_B() override;
    Byte readInputA() override;
    void writeOutputA(Byte val) override;
 

public:
    Via1(Scheduler &x_scheduler);
    void resetIo() override;
    
    const char *getName() override
    {
        return "via1";
    }
};

#endif // VIA1_INCLUDED

