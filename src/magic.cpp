/*
  Beef is a UCI-compliant chess engine.
  Copyright (C) 2020 Jonathan Tseng.

  Beef is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Beef is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Beef.h"

//Preindexed rank attack masks
const U64 RANK_ATTACKS[512] = {0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f, 0x2, 0xfd, 0xfa, 0xf6, 0xee, 0xde, 0xbe, 0x7e, 0x6, 0x5, 0xfb, 0xf4, 0xec, 0xdc, 0xbc, 0x7c,
                         0x2, 0x5, 0xfa, 0xf4, 0xec, 0xdc, 0xbc, 0x7c, 0xe, 0xd, 0xb, 0xf7, 0xe8, 0xd8, 0xb8, 0x78, 0x2, 0xd, 0xa, 0xf6, 0xe8, 0xd8, 0xb8, 0x78, 0x6,
                         0x5, 0xb, 0xf4, 0xe8, 0xd8, 0xb8, 0x78, 0x2, 0x5, 0xa, 0xf4, 0xe8, 0xd8, 0xb8, 0x78, 0x1e, 0x1d, 0x1b, 0x17, 0xef, 0xd0, 0xb0, 0x70, 0x2,
                         0x1d, 0x1a, 0x16, 0xee, 0xd0, 0xb0, 0x70, 0x6, 0x5, 0x1b, 0x14, 0xec, 0xd0, 0xb0, 0x70, 0x2, 0x5, 0x1a, 0x14, 0xec, 0xd0, 0xb0, 0x70, 0xe,
                         0xd, 0xb, 0x17, 0xe8, 0xd0, 0xb0, 0x70, 0x2, 0xd, 0xa, 0x16, 0xe8, 0xd0, 0xb0, 0x70, 0x6, 0x5, 0xb, 0x14, 0xe8, 0xd0, 0xb0, 0x70, 0x2, 0x5,
                         0xa, 0x14, 0xe8, 0xd0, 0xb0, 0x70, 0x3e, 0x3d, 0x3b, 0x37, 0x2f, 0xdf, 0xa0, 0x60, 0x2, 0x3d, 0x3a, 0x36, 0x2e, 0xde, 0xa0, 0x60, 0x6, 0x5,
                         0x3b, 0x34, 0x2c, 0xdc, 0xa0, 0x60, 0x2, 0x5, 0x3a, 0x34, 0x2c, 0xdc, 0xa0, 0x60, 0xe, 0xd, 0xb, 0x37, 0x28, 0xd8, 0xa0, 0x60, 0x2, 0xd, 0xa,
                         0x36, 0x28, 0xd8, 0xa0, 0x60, 0x6, 0x5, 0xb, 0x34, 0x28, 0xd8, 0xa0, 0x60, 0x2, 0x5, 0xa, 0x34, 0x28, 0xd8, 0xa0, 0x60, 0x1e, 0x1d, 0x1b, 0x17,
                         0x2f, 0xd0, 0xa0, 0x60, 0x2, 0x1d, 0x1a, 0x16, 0x2e, 0xd0, 0xa0, 0x60, 0x6, 0x5, 0x1b, 0x14, 0x2c, 0xd0, 0xa0, 0x60, 0x2, 0x5, 0x1a, 0x14, 0x2c,
                         0xd0, 0xa0, 0x60, 0xe, 0xd, 0xb, 0x17, 0x28, 0xd0, 0xa0, 0x60, 0x2, 0xd, 0xa, 0x16, 0x28, 0xd0, 0xa0, 0x60, 0x6, 0x5, 0xb, 0x14, 0x28, 0xd0,
                         0xa0, 0x60, 0x2, 0x5, 0xa, 0x14, 0x28, 0xd0, 0xa0, 0x60, 0x7e, 0x7d, 0x7b, 0x77, 0x6f, 0x5f, 0xbf, 0x40, 0x2, 0x7d, 0x7a, 0x76, 0x6e, 0x5e, 0xbe,
                         0x40, 0x6, 0x5, 0x7b, 0x74, 0x6c, 0x5c, 0xbc, 0x40, 0x2, 0x5, 0x7a, 0x74, 0x6c, 0x5c, 0xbc, 0x40, 0xe, 0xd, 0xb, 0x77, 0x68, 0x58, 0xb8, 0x40,
                         0x2, 0xd, 0xa, 0x76, 0x68, 0x58, 0xb8, 0x40, 0x6, 0x5, 0xb, 0x74, 0x68, 0x58, 0xb8, 0x40, 0x2, 0x5, 0xa, 0x74, 0x68, 0x58, 0xb8, 0x40, 0x1e, 0x1d,
                         0x1b, 0x17, 0x6f, 0x50, 0xb0, 0x40, 0x2, 0x1d, 0x1a, 0x16, 0x6e, 0x50, 0xb0, 0x40, 0x6, 0x5, 0x1b, 0x14, 0x6c, 0x50, 0xb0, 0x40, 0x2, 0x5, 0x1a,
                         0x14, 0x6c, 0x50, 0xb0, 0x40, 0xe, 0xd, 0xb, 0x17, 0x68, 0x50, 0xb0, 0x40, 0x2, 0xd, 0xa, 0x16, 0x68, 0x50, 0xb0, 0x40, 0x6, 0x5, 0xb, 0x14, 0x68,
                         0x50, 0xb0, 0x40, 0x2, 0x5, 0xa, 0x14, 0x68, 0x50, 0xb0, 0x40, 0x3e, 0x3d, 0x3b, 0x37, 0x2f, 0x5f, 0xa0, 0x40, 0x2, 0x3d, 0x3a, 0x36, 0x2e, 0x5e,
                         0xa0, 0x40, 0x6, 0x5, 0x3b, 0x34, 0x2c, 0x5c, 0xa0, 0x40, 0x2, 0x5, 0x3a, 0x34, 0x2c, 0x5c, 0xa0, 0x40, 0xe, 0xd, 0xb, 0x37, 0x28, 0x58, 0xa0, 0x40,
                         0x2, 0xd, 0xa, 0x36, 0x28, 0x58, 0xa0, 0x40, 0x6, 0x5, 0xb, 0x34, 0x28, 0x58, 0xa0, 0x40, 0x2, 0x5, 0xa, 0x34, 0x28, 0x58, 0xa0, 0x40, 0x1e, 0x1d,
                         0x1b, 0x17, 0x2f, 0x50, 0xa0, 0x40, 0x2, 0x1d, 0x1a, 0x16, 0x2e, 0x50, 0xa0, 0x40, 0x6, 0x5, 0x1b, 0x14, 0x2c, 0x50, 0xa0, 0x40, 0x2, 0x5, 0x1a,
                         0x14, 0x2c, 0x50, 0xa0, 0x40, 0xe, 0xd, 0xb, 0x17, 0x28, 0x50, 0xa0, 0x40, 0x2, 0xd, 0xa, 0x16, 0x28, 0x50, 0xa0, 0x40, 0x6, 0x5, 0xb, 0x14, 0x28,
                         0x50, 0xa0, 0x40, 0x2, 0x5, 0xa, 0x14, 0x28, 0x50, 0xa0, 0x40};


U64 flipVertical(U64 x)
{
	U64 k1 = (0x00FF00FF00FF00FF);
	U64 k2 = (0x0000FFFF0000FFFF);
	x = ((x >>  8) & k1) | ((x & k1) <<  8);
	x = ((x >> 16) & k2) | ((x & k2) << 16);
	x = ( x >> 32)       | ( x       << 32);
	return x;
}

U64 slidingAttacks_slow(U64 occ, int sq, U64 *masks, U64 (*func)(U64))
{
	U64 f = occ & masks[sq];
	U64 r = func(f);
	f -= SQUARE_MASKS[sq];
	r -= SQUARE_MASKS[sq^56];
	U64 r2 = func(r);
	f ^= r2;
	f &= masks[sq];
	return f;
}

U64 rankAttacks(U64 occ, int sq) {
   unsigned int file = sq &  7;
   unsigned int rkx8 = sq & 56;
   unsigned int rankOccX2 = (occ >> rkx8) & 2*63; // shift rank back to one byte
   U64 attacks = RANK_ATTACKS[4*rankOccX2  + file]; // 4 * 2 * rank occupancy + file
   return attacks << rkx8;
}

U64 rookAttacks_slow(U64 occ, int sq)
{
    return rankAttacks(occ, sq) | slidingAttacks_slow(occ, sq, FILE_MASKS, flipVertical);
}

U64 bishopAttacks_slow(U64 occ, int sq)
{
    return slidingAttacks_slow(occ, sq, DIAG_MASKS, flipVertical) | slidingAttacks_slow(occ, sq, ANTIDIAG_MASKS, flipVertical);
}

void init_magics(U64 attackTable[], SMagic magics[], U64 *masks, U64 (*func)(U64, int), const U64 *magicNumbers)
{
    U64 n;
    int size = 0;
    for(int i = 0; i < 64; i++)
    {
        SMagic &m = magics[i];
        m.mask = masks[i];
        m.shift = 64 - POPCOUNT(m.mask);
        m.aptr = (i == 0) ? attackTable : magics[i - 1].aptr + size;
        m.magic = magicNumbers[i];
        n = size = 0;
        do {
            m.aptr[m.index(n)] = func(n, i);
            size++;
            n = (n - m.mask) & m.mask;
        } while ( n );
    }
}
