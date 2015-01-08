////////////////////////////////////////////////////////////////////////////////
//
// crc.cpp
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

#include "stdafx.h"
#include <stdio.h>
#include "crc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////////////
//
//	Construct a checksum instance from a given polynomial
//
//	PARAMETERS:
//		unsigned int polynomial		Polynomial to use in checksum calculations
//
//	RETURNS:
//		-none-
//
////////////////////////////////////////////////////////////////////////////////

CCrc::CCrc(unsigned int polynomial)
{
	int i;
	for(i = 0; i < 256; i++)
	{
		m_table[i] = Reflect(i, 8) << 24;

		int j;
		for (j = 0; j < 8; j++)
		{
			m_table[i] = (m_table[i] << 1) ^ (m_table[i] & (1 << 31) ? polynomial : 0);
		}

		m_table[i] = Reflect(m_table[i], 32);
	}
}



////////////////////////////////////////////////////////////////////////////////
//
//	Destruct instance (empty)
//
//	PARAMETERS:
//		-none-
//
//	RETURNS:
//		-none-
//
////////////////////////////////////////////////////////////////////////////////

CCrc::~CCrc()
{
}



////////////////////////////////////////////////////////////////////////////////
//
//	Mirror a number of bits in a value
//
//	PARAMETERS:
//		unsigned int source		Source value
//		int c					Number of bits to mirror
//
//	RETURNS:
//		unsigned int			Mirrored value
//
////////////////////////////////////////////////////////////////////////////////

unsigned int CCrc::Reflect(unsigned int source, int c) 
{
	unsigned int value = 0;

	int i;
	for(i = 1; i < (c + 1); i++)
	{
		if(source & 0x1)
		{
			value |= 1 << (c - i);
		}

		source >>= 1;
	}

	return value;
} 



////////////////////////////////////////////////////////////////////////////////
//
//	Calculate the checksum and the length of a given file (CRC32)
//
//	PARAMETERS:
//		const char *strFilename		Filename
//		unsigned int *pCrc			Pointer to checksum buffer
//		unsigned int *pLength		Pointer to length buffer
//
//	RETURNS:
//		bool						"true" if successful, "false" if not
//
////////////////////////////////////////////////////////////////////////////////

bool CCrc::GetCrc(const char *strFilename, unsigned int *pCrc, unsigned int *pLength)
{
	unsigned int crc = 0xffffffff;

	FILE *hFile = fopen(strFilename, "rb");
	if(hFile == 0) return false;

	unsigned int length;
	for(length = 0; ; length++)
	{
		unsigned char c;
		fread(&c, sizeof(unsigned char), 1, hFile);

		if(feof(hFile) != 0) break;

		crc = (crc >> 8) ^ m_table[(crc & 0xff) ^ c];
	}

	fclose(hFile);

	*pCrc = crc ^ 0xffffffff;
	if(pLength != 0) *pLength = length;

	return true;
}
