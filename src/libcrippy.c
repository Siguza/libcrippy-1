/**
  * libcrippy-1.0 - libcrippy.c
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

#include <libcrippy-1.0/boolean.h>
#include <libcrippy-1.0/libcrippy.h>

#include <plist/plist.h>

/*
 * This function converts the unix permission bits into understandable letters. OMG!
 */
char* prot2str(uint32_t prot) {
	int i = 0;
	char* str = (char*) malloc(BUFSMALL);
	if(str) {
		memset(str, '\0', BUFSMALL);
		if(prot & 4) {
			str[i++] = 'w';
		} else str[i++] = '-';
		if(prot & 2) {
			str[i++] = 'r';
		} else str[i++] = '-';
		if(prot & 1) {
			str[i++] = 'x';
		} else str[i++] = '-';
	}
	return str;
}

int check_ascii_string(const char* string, size_t length) {
	size_t i = 0;
	if (string) {
		// Loop through each byte in this string and make sure it contains no invalid
		//  ASCII characters that might screw up our exploit
		for (i = 0; i < length; i++) {
			char letter = string[i];
			if ((letter & 0x80) > 0 || (letter & 0x7F) == 0) {
				// We have an invalid ASCII character here folks!
				return kFalse;
			}
		}
	}

	return kTrue;
}

int check_ascii_pointer(uint32_t pointer) {
	if((pointer & 0x80808080) > 0) {
		//debug("FAIL\n");
		return 0;
	}
	//debug("Passed ASCII test\n");
	if((pointer & 0x7F000000) == 0 ||
		(pointer & 0x007F0000) == 0 ||
		(pointer & 0x00007F00) == 0 ||
		(pointer & 0x0000007F) == 0) {
		//debug("FAIL\n");
		//debug("0x%08x & 0x7F7F7F7F = 0x%08x\n", pointer, (pointer & 0x7F7F7F7F));
		return 0;
	}
	//debug("PASS\n");
	return 1;
}

void hexdump (unsigned char* data, unsigned int amount) {
    unsigned int    dp, p;  /* data pointer */
    const char      trans[] =
    "................................ !\"#$%&'()*+,-./0123456789"
    ":;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklm"
    "nopqrstuvwxyz{|}~...................................."
    "....................................................."
    "........................................";


    for (dp = 1; dp <= amount; dp++) {
        fprintf (stderr, "%02x ", data[dp-1]);
        if ((dp % 8) == 0)
            fprintf (stderr, " ");
        if ((dp % 16) == 0) {
            fprintf (stderr, "| ");
            p = dp;
            for (dp -= 16; dp < p; dp++)
                fprintf (stderr, "%c", trans[data[dp]]);
            fflush (stderr);
            fprintf (stderr, "\n");
        }
        fflush (stderr);
    }
    // tail
    if ((amount % 16) != 0) {
        p = dp = 16 - (amount % 16);
        for (dp = p; dp > 0; dp--) {
            fprintf (stderr, "   ");
            if (((dp % 8) == 0) && (p != 8))
                fprintf (stderr, " ");
            fflush (stderr);
        }
        fprintf (stderr, " | ");
        for (dp = (amount - (16 - p)); dp < amount; dp++)
            fprintf (stderr, "%c", trans[data[dp]]);
        fflush (stderr);
    }
    fprintf (stderr, "\n");

    return;
}

void print_progress(double progress, void* data) {
	int i = 0;
	if(progress < 0) {
		return;
	}

	if(progress > 100) {
		progress = 100;
	}

	printf("\r[");
	for(i = 0; i < 50; i++) {
		if(i < progress / 2) {
			printf("=");
		} else {
			printf(" ");
		}
	}

	printf("] %3.1f%%", progress);
	if(progress == 100) {
		printf("\n");
	}
}

char *str_toupper(char* str)
{
	char *res = strdup(str);
	unsigned int i;
	for (i = 0; i < strlen(res); i++) {
		res[i] = toupper(res[i]);
	}
	return res;
}

char* format_size_for_display(uint64_t size)
{
	char buf[32];
	double sz;
	if (size >= 1000000000LL) {
		sz = ((double)size / 1000000000.0f);
		sprintf(buf, "%0.1f GB", sz);
	} else if (size >= 1000000LL) {
		sz = ((double)size / 1000000.0f);
		sprintf(buf, "%0.1f MB", sz);
	} else if (size >= 1000LL) {
		sz = ((double)size / 1000.0f);
		sprintf(buf, "%0.1f kB", sz);
	} else {
		sprintf(buf, "%d Bytes", (int)size);
	}
	return strdup(buf);
}


int plist_read(const char* path, plist_t* plist) {
	int x = 0;
	unsigned int length = 0;
	unsigned char* buffer = NULL;
    
	if (!path) {
		return -1;
	}
    
	x = file_read(path, &buffer, &length);
	if (x < 0 || buffer == NULL || length <= 0) {
		return -1;
	}
    
	if ((length > 8) && (memcmp(buffer, "bplist00", 8) == 0)) {
		plist_from_bin(buffer, length, plist);
	} else {
		plist_from_xml(buffer, length, plist);
	}
    
	if (buffer) {
		free(buffer);
		buffer = NULL;
	}
    
	return 0;
}

int plist_write(const char* path, plist_t plist, plist_format_t format) {
	int x = 0;
	unsigned int length = 0;
	unsigned char* buffer = NULL;
    
	if (!plist || !path) {
		return -1;
	}
    
	if (format == PLIST_FORMAT_XML) {
		plist_to_xml(plist, &buffer, &length);
	} else if (format == PLIST_FORMAT_BINARY) {
		plist_to_bin(plist, &buffer, &length);
	} else {
		return -1;
	}
    
	x = file_write(path, &buffer, &length);
	if (x < 0) {
		free(buffer);
		return -1;
	}
    
	if (buffer) {
		free(buffer);
		buffer = NULL;
	}
    
	return 0;
}


void print_progress_real(double progress, int flush)
{
	int i = 0;
	fprintf(stderr, "\r[");
	for(i = 0; i < 50; i++) {
		if(i < progress / 2) {
			fprintf(stderr, "=");
		} else {
			fprintf(stderr, " ");
		}
	}
	fprintf(stderr, "] %3.0f%%", progress);

	if (flush > 0) {
		fflush(stdout);
		if (progress == 100)
			fprintf(stderr, "\n");
	}
}
