#ifndef ENCRYPTION_AL 
#define ENCRYPTION_AL

#define ENCRYPTION_MODE 1
#define DECRYPTION_MODE 0
#define KEY_SIZE 8

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
using namespace std;

/*
1 word : 32bit : 4byte : unsigned int
1 byte : 8bit : unsigned char
*/
typedef unsigned char byte;
typedef unsigned int bit32;

class Block_AL {
public:

	/* 파일 관련 변수 */
	ifstream infile, outfile, keyfile;
	FILE *key_file, *input_file, *output_file;
	unsigned long file_size = 0, number_of_blocks = 0;

	/* 임시 key 관련 변수 */
	unsigned char* tmp_key;

	Block_AL() { cout << "Block Algorithm" << " -> "; };
	virtual ~Block_AL() {};

	virtual void Encryption() {};
	virtual void Decryption() {};

	bool Get_file(char* input_file);
	bool Get_keyfile(char* input_keyfile);
	bool Make_keyfile();
};

class Hash_AL {
public:

	/* 파일 관련 변수 */
	ifstream infile;
	FILE *input_file;
	unsigned long file_size = 0, number_of_blocks = 0;

	Hash_AL() { cout << "Hash Algorithm" << " -> "; };
	virtual ~Hash_AL() {};

	virtual void Encryption() {};

	bool Get_file(char* input_file);

	string MD5_result;
};

#endif