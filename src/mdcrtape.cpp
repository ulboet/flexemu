/*
    mdcrtape.cpp


    FLEXplorer, An explorer for any FLEX file or disk container
    Copyright (C) 2018-2019  W. Schwotzer

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


#include "mdcrtape.h"
#include <fstream>
#include <iostream>
#include <algorithm>

const std::array<char, 4> MiniDcrTape::magic_bytes { 'M', 'D', 'C', 'R' };

MiniDcrTape::MiniDcrTape(MiniDcrTape &&src) :
    stream(std::move(src.stream)), is_write_protected(src.is_write_protected),
    max_pos(src.max_pos),
    record_positions(src.record_positions),
    record_index(src.record_index)
{
}

MiniDcrTape::MiniDcrTape(const char *path) : is_write_protected(false)
{
    stream.open(path, std::ios::in | std::ios::out | std::ios::binary);

    if (!IsOpen())
    {
        stream.open(path, std::ios::in | std::ios::binary);
        if (IsOpen())
        {
            // file only can be opened read-only.
            is_write_protected = true;
        }
        else
        {
            // file does not exist, create a new one.
            stream.open(path, std::ios::out | std::ios::binary);
            if (IsOpen())
            {
                stream.write(MiniDcrTape::magic_bytes.data(),
                             MiniDcrTape::magic_bytes.size());
                stream.put(0);
                stream.put(0);
                stream.close();
                stream.open(path,
                        std::ios::in | std::ios::out | std::ios::binary);
            }
        }
    }

    if (!VerifyTape())
    {
        // Wrong file format
        std::cerr << "wrong file format: " << path << std::endl;
        stream.close();
    }

    record_index = 0;
}

MiniDcrTape::MiniDcrTape::~MiniDcrTape()
{
    if (IsOpen())
    {
        Close();
    }
}

MiniDcrTape &MiniDcrTape::operator= (MiniDcrTape &&src)
{
    if (&src != this)
    {
        stream = std::move(src.stream);
        is_write_protected = src.is_write_protected;
        max_pos = src.max_pos;
        record_positions = src.record_positions;
        record_index = src.record_index;
    }
    return *this;
}

MiniDcrTapePtr MiniDcrTape::Create(const char *path)
{
    return MiniDcrTapePtr(new MiniDcrTape(path));
}

int MiniDcrTape::Close()
{
    stream.close();
    record_positions.clear();
    record_index = 0;
    return 0;
}

bool MiniDcrTape::IsOpen() const
{
    return stream.is_open();
}

bool MiniDcrTape::IsWriteProtected() const
{
    return is_write_protected;
}

bool MiniDcrTape::HasRecord() const
{
    if (IsOpen())
    {
        // Check it file positions a valid (= not empty) record.
        return record_index < record_positions.size() - 1;
    }

    return false;
}

RecordType MiniDcrTape::GetRecordType() const
{
    if (IsOpen() && (record_index < record_types.size()))
    {
        return record_types[record_index];
    }

    return RecordType::NONE;
}

DWord MiniDcrTape::GetRecordIndex() const
{
    return record_index;
}

bool MiniDcrTape::ReadRecord(std::vector<Byte> &buffer)
{
    if (IsOpen())
    {
        stream.seekg(record_positions[record_index]);
        Word size = (stream.get() & 0xFF) << 8;
        size |= stream.get() & 0xFF;

        if (size == 0)
        {
            return false;
        }

        buffer.clear();
        buffer.resize(size, 0);
        auto pbuffer = reinterpret_cast<char *>(buffer.data());
        // Read the buffer contents
        stream.read(pbuffer, buffer.size());
        record_index++;

        return stream.good();
    }

    return false;
}

bool MiniDcrTape::WriteRecord(const std::vector<Byte> &buffer)
{
    if (IsOpen())
    {
        stream.seekp(record_positions[record_index]);
        // Write two bytes containing the buffer size
        // Write most significant byte first
        Word size = static_cast<Word>(buffer.size());
        stream.put(static_cast<char>(size >> 8));
        stream.put(static_cast<char>(size & 0xFF));
        auto pbuffer = reinterpret_cast<const char *>(buffer.data());
        // Write the buffer contents
        stream.write(pbuffer, buffer.size());
        std::ios::pos_type record_position = stream.tellp();
        stream.put(0);
        stream.put(0);
        stream.seekp(record_position);

        if (stream.good())
        {
            record_index++;
            if (record_index >= record_positions.size())
            {
                record_positions.push_back(record_position);
            }
            else
            {
                // An old record has been overwitten.
                // Shrink number of record positions.
                record_positions[record_index] = record_position;
                record_positions.resize(record_index + 1);
            }
            max_pos = record_position;
            VerifyTape();
        }
        else
        {
            // Write record failed. Position to previous record.
            stream.seekp(record_positions[record_index]);
        }

        return stream.good();
    }

    return false;
}

bool MiniDcrTape::VerifyTape()
{
    bool result = false;

    record_positions.clear();
    record_types.clear();
    max_pos = 0;

    if (IsOpen())
    {
        std::vector<Word> record_sizes;
        static std::array<char, 4> magic_bytes { 0, 0, 0, 0 };
        std::ios::pos_type read_pos = stream.tellg();
        std::ios::pos_type record_position;

        stream.seekg(0);

        stream.read(magic_bytes.data(), magic_bytes.size());
        if (stream.fail() || stream.tellg() != magic_bytes.size() ||
                magic_bytes != MiniDcrTape::magic_bytes)
        {
            // Wrong or incomplete magic number
            return result;
        }

        Word size = 0xFFFF;

        while (size != 0)
        {
            record_position = stream.tellg();
            size = (stream.get() << 8);
            size |= (stream.get() & 0xFF);
            if (stream.eof() || stream.fail())
            {
                size = 0xFFFF;
                break;
            }

            stream.seekg(size, std::ios::cur);

            if (stream.eof() || stream.fail())
            {
                size = 0xFFFF;
                break;
            }

            record_sizes.push_back(size);
            record_positions.push_back(record_position);
        }

        if (size == 0)
        {
            max_pos = stream.tellg();
            result = true;
        }

        // Restore previous read position.
        stream.seekg(read_pos);

        RecordType previous_record_type = RecordType::NONE;
        for (auto sizeIter = record_sizes.crbegin();
             sizeIter < record_sizes.crend();
             ++sizeIter)
        {
            auto record_type = RecordType::NONE;

            if (*sizeIter == 1027)
            {
                record_type = RecordType::Data;
            }
            else if (previous_record_type == RecordType::Data ||
                       previous_record_type == RecordType::LastData)
            {

                record_type = *sizeIter == 13 ?
                    RecordType::Header :
                    RecordType::Data;
            }
            else if (previous_record_type == RecordType::NONE ||
                     previous_record_type == RecordType::Header)
            {
                record_type = RecordType::LastData;
            }

            record_types.push_back(record_type);
            previous_record_type = record_type;
            /*
            switch(record_type)
            {
                case RecordType::NONE: std::cerr << "type=NONE\n"; break;
                case RecordType::Data: std::cerr << "type=Data\n"; break;
                case RecordType::LastData: std::cerr << "type=LastData\n"; break;
                case RecordType::Header: std::cerr << "type=Header\n"; break;
            }
            */
        }
        std::reverse(record_types.begin(), record_types.end());
    }

    return result;
}

bool MiniDcrTape::GotoPreviousRecord()
{
    if (!IsOpen() || !record_index)
    {
        // No drive selected or at Begin-of-Tape.
        return false;
    }

    --record_index;

    return true;
}
