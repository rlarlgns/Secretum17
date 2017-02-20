#pragma once
#include "wrapper.h"

/*
bool get_keyfile(input_keyfile) : key 파일을 불러옴
*/
bool Wrapper::Get_keyfile(char* input_keyfile) {
	printf("%s\n", input_keyfile);
	key_file = fopen(input_keyfile, "rb");
	if (!key_file) {
		printf("Get_keyfile error!\n");
		return 1;
	}

	printf("Success get_keyfile\n");
	return 0;
}

/*
bool get_file(input_file) : 암호화, 복호화 대상이 될 파일 선택
1. file_size, number_of_blocks 값 할당
2. 에러 발생 시 return 값 0
*/
bool Wrapper::Get_file(char* input_filename) {
	char* output_filename = "aaa.txt";

	printf("Input file : %s\n", input_filename);
	printf("Output file : %s\n", output_filename);

	input_file = fopen(input_filename,"rb");
	if (!input_file) {
		printf("Error input_file!\n");
		return 1;
	}

	output_file = fopen(output_filename, "wb");
	if (!output_file) {
		printf("Error output_file!\n");
		return 1;
	}

	fseek(input_file, 0L, SEEK_END);
	file_size = ftell(input_file);

	fseek(input_file, 0L, SEEK_SET);
	number_of_blocks = file_size / 8 + ((file_size % 8) ? 1 : 0);

	printf("Success get_file :) \n");
	return 0;
}

/*
void generate_key() : 난수 key 생성
1. 8byte 난수 키 생성
*/
void Generate_key(unsigned char* key) {
	for (int i = 0; i<8; i++) {
		key[i] = rand() % 255;
	}
}


/*
bool make_keyfile() : key file 생성
1. 8byte 임시키 파일 생성
2. 에러 발생 시 return 값 0
*/
bool Wrapper::Make_keyfile() {
	short int bytes_written;

	key_file = fopen("keyfile_tmp.key", "wb");
	if (!key_file) {
		printf("Error make_keyfile !!\n");
		return 1;
	}

	tmp_key = (unsigned char*)malloc(8 * sizeof(char));
	Generate_key(tmp_key);
	bytes_written = fwrite(tmp_key, 1, KEY_SIZE, key_file);

	free(tmp_key);
	fclose(key_file);

	printf("Success make_keyfile\n");
	return 0;
}



