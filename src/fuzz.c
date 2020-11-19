/**
  * libcrippy-1.0 - fuzz.c
  * Copyright (C) 2013 Crippy-Dev Team
  * Copyright (C) 2010-2013 Joshua Hill
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include <stdlib.h>

int random_string(unsigned char* buffer, unsigned int size) {
	unsigned int i = 0;
	for (i = 0; i < size; i++) {
		buffer[i] = rand() & 0xFF;
	}
	return 0;
}

unsigned int random_int() {
	unsigned int i = 0;
    unsigned int v = 0;
    unsigned char buffer[4];
	for (i = 0; i < 4; i++) {
		buffer[i] = rand() & 0xFF;
	}
    v = *(unsigned int*) buffer;
	return v;
}

unsigned short random_short() {
	unsigned int i = 0;
    unsigned short v = 0;
    unsigned char buffer[2];
	for (i = 0; i < 2; i++) {
		buffer[i] = rand() & 0xFF;
	}
    v = *(unsigned short*) buffer;
	return v;
}

unsigned char random_char() {
	unsigned int i = 0;
    unsigned char v = 0;
    unsigned char buffer[1];
	for (i = 0; i < 1; i++) {
		buffer[i] = rand() & 0xFF;
	}
    v = *(unsigned char*) buffer;
	return v;
}

int randomize_string(unsigned char* buffer, int size, float amount) {
	int i = 0;
	//srand(seed++);
	//printf("Seeded with %d\8n", seed);
	//char dbg[0x100];
	int times = (int) ((amount * (float) (size*8)) + 0.5);
	//snprintf(dbg, 0xFF, "%d * %f = %d\n", size, amount, times);
	//printf("%s\n", dbg);
	int bits = size * 8;
	//memset(buffer, '\1', size);
	for (i = 0; i < times; i++) {
		//printf("%d\n", i);
		// Here we're going to flip some bits and record the type of crash we get
		int bit = rand() % bits; // returns a number 0x0 to size * 8
		//printf("Bit = %d\n", bit);
		int index = bit / 8; // returns the byte index of the bit we're going to change
		//printf("Index = 0x%x\n", index);
		//printf("Original byte found was 0x%x at 0x%08x\n", buffer[index], &buffer[index]);

		int shift = bit - (index * 8); // the index of the bit from the byte
		//debug("Shift = %d\n", shift);
		char bit_mask = 1 << shift;
		//debug("BitMask = 0x%x\n\n", bit_mask);
		buffer[index] ^= bit_mask;
		//printf("New byte is 0x%x\n\n", buffer[index]);
		//buffer[index] &= ~0x80;
	}
	return 0;
}
