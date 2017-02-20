
#ifndef WRAPPER_H
#define WRAPPER_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
using namespace std;
 
/* DES key is 8 bytes long */
#define KEY_SIZE 8

class Wrapper {
private:

public:
	
	/* 파일 관련 변수 */
	ifstream infile;
	ofstream outfile;
	fstream keyfile;
	FILE *key_file, *input_file, *output_file;
	unsigned long file_size = 0, number_of_blocks = 0;

	/* 임시 key 관련 변수 */
	unsigned char* tmp_key;

	bool Get_file(char* input_file);
	bool Get_keyfile(char* input_keyfile);
	bool Make_keyfile();

};



#endif
