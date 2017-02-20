#ifndef MD5_H
#define MD5_H

#include "wrapper.h"

/*
1 word : 32bit : 4byte : unsigned int
1 byte : 8bit : unsigned char
*/
typedef unsigned char byte;
typedef unsigned int bit32;

class MD5 : public Wrapper {
private:
	/* state ( A, B, C, D ) */ 
	bit32 state[4];
	/* 비트의 수 */
	bit32 count[2];
	/* input buffer */
	byte buffer[64];

	/* PADDING : 최대 패딩 비트 512bit ( 64 * 8 ) */
	byte PADDING[64] = {
		0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	byte buff[BUFSIZ];
	unsigned char output[16];

	void Encode(byte *output, bit32 *input, unsigned int len);
	void Decode(bit32 *input, byte *output, unsigned int len);
	void MD5_trnasform(byte block[64]);
public:
	void MD5_init();
	void MD5_update(byte *input, unsigned long input_len);
	void MD5_final(byte digest[16]);
	MD5() {};

	char hb2hex(unsigned char hb);
	string MD5_file();
};

#endif