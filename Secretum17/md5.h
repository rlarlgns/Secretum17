#ifndef MD5_H
#define MD5_H

#include "encryption_AL.h"

class MD5 : public Hash_AL {
private:

	/* state ( A, B, C, D ) */ 
	bit32 state[4];
	/* 비트의 수 */
	bit32 count[2];
	/* input buffer */
	byte buffer[64];

	byte buff[BUFSIZ];
	unsigned char output[16];

	void Encode(byte *output, bit32 *input, unsigned int len);
	void Decode(bit32 *input, byte *output, unsigned int len);
	void MD5_trnasform(byte block[64]);

	void MD5_init();
	void MD5_update(byte *input, unsigned long input_len);
	void MD5_final(byte digest[16]);

public:

	virtual void Encryption();

	MD5() { cout << "MD5" << endl; };
	virtual ~MD5() {};
};

#endif