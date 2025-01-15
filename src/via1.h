/*
<<<<<<<< HEAD:src/fdoptman.h
    fdoptman.h

========
    via1.h
>>>>>>>> 1272a3db9d378f0108133dfa7c6cec361eb660b6:src/via1.h

    flexemu, an MC6809 emulator running FLEX
    Copyright (C) 2024-2025  W. Schwotzer

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

<<<<<<<< HEAD:src/fdoptman.h
#ifndef FDOPTMAN_INCLUDED
#define FDOPTMAN_INCLUDED

#include "misc1.h"
#include <string>


class FlexDirectoryDiskOptions
========


#ifndef VIA1_INCLUDED
#define VIA1_INCLUDED

#include "misc1.h"
#include "mc6522.h"
#include "bobservd.h"


class Scheduler;
class BObserver;

class Via1 : public Mc6522, public BObserved
>>>>>>>> 1272a3db9d378f0108133dfa7c6cec361eb660b6:src/via1.h
{
protected:

    Scheduler &scheduler;
    
protected:

    void set_irq_A() override;
    void set_irq_B() override;
    Byte readInputA() override;
    void writeOutputA(Byte val) override;
 

public:
<<<<<<<< HEAD:src/fdoptman.h
    FlexDirectoryDiskOptions() = delete;
    explicit FlexDirectoryDiskOptions(std::string directory);
    bool Read();
    bool Write(bool onlyIfNotExists);
    int GetTracks() const;
    int GetSectors() const;
    void SetTracks(int tracks);
    void SetSectors(int sectors);

private:
    static const std::string &GetRcFilename();

    std::string path;
    int tracks;
    int sectors;
};

#endif // FOPTMAN_INCLUDED
========
    Via1(Scheduler &x_scheduler);
    void resetIo() override;
    
    const char *getName() override
    {
        return "via1";
    }
};

#endif // VIA1_INCLUDED
>>>>>>>> 1272a3db9d378f0108133dfa7c6cec361eb660b6:src/via1.h

