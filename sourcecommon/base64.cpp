#include <ctype.h>
#include <string.h>
#include "base64.h"

static const char base64_chars[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

char *
base64_encode(const unsigned char *input, int length)
{
    /* http://www.adp-gmbh.ch/cpp/common/base64.html */
    int i=0, j, s=0;
    unsigned char char_array_3[3], char_array_4[4];

    int b64len = (length+2 - ((length+2)%3))*4/3;
    char *b64str = new char[b64len + 1];

    while (length--) {
        char_array_3[i++] = *(input++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4; i++)
                b64str[s++] = base64_chars[char_array_4[i]];

            i = 0;
        }
    }
    if (i) {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; j < i + 1; j++)
            b64str[s++] = base64_chars[char_array_4[j]];

        while (i++ < 3)
            b64str[s++] = '=';
    }
    b64str[b64len] = '\0';

    return b64str;
}

static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

unsigned char *
base64_decode(const char *input, int length, int *outlen)
{
    int i = 0;
    int j;
    int r = 0;
    int idx = 0;
    unsigned char char_array_4[4], char_array_3[3];
    unsigned char *output = new unsigned char[length*3/4];

    while (length-- && input[idx] != '=') {
	//skip invalid or padding based chars
	if (!is_base64(input[idx])) {
	    idx++;
	    continue;
	}
        char_array_4[i++] = input[idx++];
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = strchr(base64_chars, char_array_4[i]) - base64_chars;

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                output[r++] = char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j <4; j++)
            char_array_4[j] = 0;

        for (j = 0; j <4; j++)
            char_array_4[j] = strchr(base64_chars, char_array_4[j]) - base64_chars;

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++)
            output[r++] = char_array_3[j];
    }

    *outlen = r;

    return output;
}
