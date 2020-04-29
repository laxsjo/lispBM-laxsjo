/*
    Copyright 2020 Joel Svensson	svenssonjoel@yahoo.se

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
   Motivation: Memory manager for allocation of strings and arrays
   that will not be be located on the lisp-heap. These kinds of values
   have thus far been allocated using the "malloc" function provided
   on the platform. Using malloc is something I want to move away from
   doing within the guts of lispBM as I want it to be possible on
   running on the bare metal.

   Later perhaps things such as the symbol table with symbol mappings
   should also be located on this managed memory area.  Symbols,
   however, are never freed after being created in lispBM. Currently I
   dont know if that is a good idea? or if it is possible to free
   unused symbols at all. To become entirely free from malloc, the entire
   symbol table management must be rewritten.
*/

/*
  Notes:
  64Bytes = 16 * 4Byte words
  4Bytes = 32Bits = 16 * 2Bit of status

  Status Bitpatterns:
  00 - FREE or USED 4Byte word
  01 - END of a sequence of allocated words
  10 - START of a sequence of allocated words
  11 - START and END of a sequence of allocated words (a 1 word allocation)

    0  1  2  3  4  5  6  7  8  9
  [11 00 00 00 00 10 01 11 00 00]

  Favours allocation of small amounts of data.

  Requirements:
   - Memory space is a multiple of 64Bytes.
   - Memory status bitmap is the same multiple of 4Bytes.
*/
#ifndef _MEMORY_H_
#define _MEMORY_H_

#define MEMORY_SIZE_64BYTES_TIMES_X(X) (64*(X))
#define MEMORY_BITMAP_SIZE(X) (4*(X))

#define MEMORY_SIZE_512 MEMORY_SIZE_64BYTES_TIMES_X(8)
#define MEMORY_SIZE_1K MEMORY_SIZE_64BYTES_TIMES_X(16)
#define MEMORY_SIZE_2K MEMORY_SIZE_64BYTES_TIMES_X(32)
#define MEMORY_SIZE_4K MEMORY_SIZE_64BYTES_TIMES_X(64)
#define MEMORY_SIZE_8K MEMORY_SIZE_64BYTES_TIMES_X(128)
#define MEMORY_SIZE_16K MEMORY_SIZE_64BYTES_TIMES_X(256)
#define MEMORY_SIZE_32K MEMORY_SIZE_64BYTES_TIMES_X(512)
#define MEMORY_SIZE_1M MEMORY_SIZE_64BYTES_TIMES_X(16384)

#define MEMORY_BITMAP_SIZE_512 MEMORY_BITMAP_SIZE(8)
#define MEMORY_BITMAP_SIZE_1K  MEMORY_BITMAP_SIZE(16)
#define MEMORY_BITMAP_SIZE_2K  MEMORY_BITMAP_SIZE(32)
#define MEMORY_BITMAP_SIZE_4K  MEMORY_BITMAP_SIZE(64)
#define MEMORY_BITMAP_SIZE_8K  MEMORY_BITMAP_SIZE(128)
#define MEMORY_BITMAP_SIZE_16K MEMORY_BITMAP_SIZE(256)
#define MEMORY_BITMAP_SIZE_32K MEMORY_BITMAP_SIZE(512)
#define MEMORY_BITMAP_SIZE_1M  MEMORY_BITMAP_SIZE(16384)

extern int memory_init(unsigned char *data, uint32_t data_size,
		       unsigned char *bitmap, uint32_t bitmap_size);
extern uint32_t *memory_allocate(uint32_t num_words);
extern int memory_free(uint32_t *ptr);

#endif
