#ifndef base64H
#define base64H

char * base64_encode(const unsigned char *input, int length);
unsigned char * base64_decode(const char *input, int length, int *outlen);

#endif
