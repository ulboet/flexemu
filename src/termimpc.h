/*
    termimpc.h


    flexemu, an MC6809 emulator running FLEX
    Copyright (C) 2024  W. Schwotzer

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


#ifndef NCURSESTERMINALIMPL_INCLUDED
#define NCURSESTERMINALIMPL_INCLUDED

#include "typedefs.h"
#include "termimpi.h"
#include "bobservd.h"
#include <string>
#include <deque>
#include <vector>
#include <mutex>
#ifdef UNIX
#include <ncurses.h>
#endif


struct sOptions;

class NCursesTerminalImpl : public ITerminalImpl, public BObserved
{
    const sOptions &options;
    bool is_cursor_visible{true};
    bool is_german{false};
    Word init_delay{500};
    Word input_delay{0};
    std::mutex serial_mutex;
    std::deque<Byte> key_buffer_serial;
#ifdef UNIX
    std::vector<Byte> esc_sequence;

    static WINDOW *win;
#endif

public:
    NCursesTerminalImpl() = delete;
    explicit NCursesTerminalImpl(const sOptions &p_options);
    ~NCursesTerminalImpl() override = default;

    // Interface ITerminalImpl
    bool init(Word reset_key, fct_sigaction fct) override;
    void reset_serial() override;
    bool has_char_serial() override;
    Byte read_char_serial() override;
    Byte peek_char_serial() override;
    void write_char_serial(Byte val) override;
    bool is_terminal_supported() override;
    void set_startup_command(const std::string &startup_command) override;

private:
    void reset_terminal_io() override;
    bool init_terminal_io(Word reset_key) const;
#ifdef UNIX
    void put_char_serial(Byte key);
    void write_char_serial_safe(Byte val);
    static void limit_column_and_line(int &column, int &line);
    void process_ctrl_character(Byte value);
    void process_esc_sequence(Byte value);
#endif
};
#endif
