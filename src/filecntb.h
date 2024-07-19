/*
    filecntb.h

    FLEXplorer, An explorer for FLEX disk image files and directory disks.
    Copyright (C) 1998-2024  W. Schwotzer

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

#ifndef FILECNTB_INCLUDED
#define FILECNTB_INCLUDED

#include "misc1.h"
#include <string>


/* possible constants for container type */

const unsigned TYPE_DISKFILE = 0x01U; /* type: file container */
const unsigned TYPE_DIRECTORY = 0x02U; /* type: directory */
const unsigned TYPE_DSK_DISKFILE =
    0x10U; /* subtype: a file container with DSK format */
const unsigned TYPE_FLX_DISKFILE =
    0x20U; /* subtype: a file container with FLX format */
const unsigned TYPE_DIRECTORY_BY_SECTOR = 0x40U; /* subtype: NAFS directory */
/* (means: without text conversion) */
const unsigned TYPE_RAM_DISKFILE =
    0x80U; /* subtype: filecontainer loaded in RAM */
const unsigned TYPE_JVC_HEADER =
    0x100U; /* subtype: DSK filecontainer with JVC header */

// This macro defines the name of a file. It contains the boot sector.
// It is used in directory containers to be able to boot from them.
#define BOOT_FILE "boot"

const int SECTOR_SIZE = 256;

class FlexDiskAttributes;

// Magic number to mark a file as random access file. This byte is stored
// in s_dir_entry in field sector_map.
const Byte IS_RANDOM_FILE = 0x02;
// Number of directory entries in one directory sector, struct s_dir_sector
const Byte DIRENTRIES = 10;

// Max. length of the diskname, without terminating NUL
const size_t FLEX_DISKNAME_LENGTH = 8U;
// Max. length of the disk extension, without terminating NUL
const size_t FLEX_DISKEXT_LENGTH = 3U;
// Max. length of the file basename, without extension, without term. NUL
const size_t FLEX_BASEFILENAME_LENGTH = 8U;
// Max. length of the file extension, without terminating NUL
const size_t FLEX_FILEEXT_LENGTH = 3U;
// Max. length of a FLEX filename incl. dot and terminating NUL
const size_t FLEX_FILENAME_LENGTH =
                 FLEX_BASEFILENAME_LENGTH + FLEX_FILEEXT_LENGTH + 2U;

// This interface gives basic properties access to a FLEX disk image.
// Rename: FileContainerIfBase => IFlexDiskBase
class IFlexDiskBase
{
public:
    virtual bool IsWriteProtected() const = 0;
    virtual bool GetAttributes(FlexDiskAttributes &diskAttributes) const = 0;
    virtual unsigned GetFlexDiskType() const = 0;
    virtual std::string GetPath() const = 0;

    virtual ~IFlexDiskBase() = default;
};

#endif /* FILECNTB_INCLUDED */

