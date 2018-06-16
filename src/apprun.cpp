/*
    apprun.cpp


    flexemu, an MC6809 emulator running FLEX
    Copyright (C) 2018  W. Schwotzer

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
#include <new>
#include <sstream>
#ifdef _MSC_VER
    #include <new.h>
#endif
#ifndef _WIN32
    #include <sched.h>
#endif
#include <sys/types.h>
#ifndef _MSC_VER
    #include <unistd.h>
#endif

#include "e2.h"
#include "apprun.h"
#include "win32gui.h"
#include "xtgui.h"
#include "foptman.h"
#include "flexerr.h"
#include "fileread.h"


ApplicationRunner::ApplicationRunner(
                    struct sGuiOptions &x_guiOptions,
                    struct sOptions &x_options) :
    guiOptions(x_guiOptions),
    options(x_options),
    memory(options.isHiMem),
    cpu(memory),
    inout(cpu, scheduler),
    mmu(memory),
    acia1(inout, cpu),
    pia1(cpu, scheduler, keyboardIO),
    pia2(cpu, keyboardIO, joystickIO),
    drisel(fdc),
    command(inout, cpu, scheduler),
    video(memory),
    rtc(cpu)
{
}

int ApplicationRunner::run()
{
    scheduler.set_cpu(&cpu);
    scheduler.set_inout(&inout);
    cpu.set_disassembler(&disassembler);
    cpu.set_use_undocumented(options.use_undocumented);

    fdc.disk_directory(options.disk_dir.c_str());
    fdc.mount_all_drives(options.drive);
    inout.set_fdc(&fdc);
    command.set_fdc(&fdc);

    inout.init(options.reset_key);

    if (!(options.term_mode && inout.is_terminal_supported()))
    {
        inout.create_gui(guiOptions.guiType, joystickIO, keyboardIO, pia1,
                      memory, video, guiOptions);
    }

    // Add all memory mapped I/O devices to memory.
    memory.add_io_device(mmu, MMU_BASE, MMU_MASK, 0, 0);
    memory.add_io_device(acia1, ACIA1_BASE, ACIA1_MASK, 0, 0);
    memory.add_io_device(pia1, PIA1_BASE, PIA1_MASK, 0, 0);
    memory.add_io_device(pia2, PIA2_BASE, PIA2_MASK, 0, 0);
    memory.add_io_device(fdc, FDC_BASE, FDC_MASK, 0, 0);
    // drisel: Same register is mirrored 4 times in address space.
    memory.add_io_device(drisel, DRISEL_BASE, DRISEL_MASK, 0, 0);
    memory.add_io_device(drisel, DRISEL_BASE+1, DRISEL_MASK, 0, 0);
    memory.add_io_device(drisel, DRISEL_BASE+2, DRISEL_MASK, 0, 0);
    memory.add_io_device(drisel, DRISEL_BASE+3, DRISEL_MASK, 0, 0);
    memory.add_io_device(command, COMM_BASE, COMM_MASK, 0, 0);
    memory.add_io_device(video, VICO_BASE, VICO_MASK, 0, 0);
    memory.add_io_device(rtc, RTC_LOW, RTC_HIGH - RTC_LOW + 1, 0, 0);
    inout.set_rtc(&rtc);

    // Load monitor program into ROM.
    int error;
    if ((error = load_hexfile(options.hex_file.c_str(), memory)) < 0)
    {
        std::string hexFilePath;

        hexFilePath = options.disk_dir + PATHSEPARATORSTRING +
                      options.hex_file;

        if ((error = load_hexfile(hexFilePath.c_str(), memory)) < 0)
        {
            std::stringstream pmsg;

            pmsg << "File \"" << hexFilePath
                 << "\" not found or has unknown file format (" << error <<")"
                 << std::endl;
#ifdef _WIN32
            MessageBox(nullptr, pmsg.str().c_str(), PROGRAMNAME " error",
            MB_OK | MB_ICONERROR);
#endif
#ifdef UNIX
            fprintf(stderr, "%s", pmsg.str().c_str());
#endif
            return 1;
        }
    }

    memory.reset_io();
    cpu.reset();

    // start CPU thread
    if (!scheduler.Start())
    {
        fprintf(stderr, "Unable to start CPU thread\n");
        return 1;
    }
    else
    {
        inout.main_loop();
        scheduler.Join();  // wait for termination of CPU thread
    }

    return 0;
}
