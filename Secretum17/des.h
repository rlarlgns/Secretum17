
#ifndef DES_H
#define DES_H

#include "encryption_AL.h"

/*
1 word : 32bit : 4byte : unsigned int
1 byte : 8bit : unsigned char
*/
typedef unsigned char byte;
typedef unsigned int bit32;

typedef struct {
	/* 각 라운드의 key 저장 */
	byte k[8];
	byte c[4];
	byte d[4];
} key_set;

class DES : public Block_AL {
private:

	unsigned short int padding;

	short int bytes_written;
	unsigned long block_count = 0;
	byte *data_block, *processed_block;
	key_set* key_sets;

	void Generate_sub_keys(byte* main_key, key_set* key_sets);
	void Process_message(byte* message_piece, byte* processed_piece, key_set* key_sets, int mode);
	void Set_DES();

public:

	virtual void Encryption();
	virtual void Decryption();

	DES() { cout << "DES" << endl; };
	virtual ~DES() {};
};

#endif