////////////////////////////////////////////////////////////////////////////////
//
// crc.h
//
// Copyright (C) 2000 Bruno 'Beosil' Heidelberger
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//
//	This class handles checksum calculation of a given file (CRC32).
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _CRC_H_
#define _CRC_H_

// Default polynomial used for the table generation
#define CRC_DEFAULT_POLYNOMIAL	0x04c11db7

class CCrc
{
protected:
	unsigned int m_table[256];

public:
	// Construct a checksum instance from a given polynomial
	CCrc(unsigned int polynomial = CRC_DEFAULT_POLYNOMIAL);

	// Destruct instance (empty)
	virtual ~CCrc();

	// Calculate the checksum and the length of a given file (CRC32)
	bool GetCrc(const char *strFilename, unsigned int *pCrc, unsigned int *pLength = 0);

protected:
	// Mirror a number of bits in a value
	unsigned int Reflect(unsigned int source, int c);
};

#endif
