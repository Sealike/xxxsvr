#ifndef __BASE64_H__
#define __BASE64_H__


static const unsigned char base64_enc_map[64];
static const unsigned char base64_dec_map[128];
int base64_encode(char* dst, int* dlen, const unsigned char* src, int slen);
int base64_decode(unsigned char* dst, int* dlen, const char* src, int slen);








#endif