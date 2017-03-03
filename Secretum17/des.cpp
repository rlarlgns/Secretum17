/*
DES process

Data Encryption
1. 64 bit 평문 IP(Initial Permutation) 수행
2. 좌우 32bit( lo, ro ) 나뉘어 round 진행
3. ro 32bit는 첫 라운드의 48bit key( key_sets[0] )와 F 함수에 
들어가서 32bit return
4. ro와 lo의 XOR 연산
5. ro, lo change ( ro -> lo, lo -> ro )
6. 16round 진행
7. IP의 역을 수행하여 64bit 암호문 출력
*/

#include "stdafx.h"
#include "des.h"

/* key 생성 변수 start */

/* key initial permutaion : 56bit key 초기 치환 */
static int key_ip[56] = {
	57, 49, 41, 33, 25, 17,  9,
	1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27,
	19, 11,  3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29,
	21, 13,  5, 28, 20, 12,  4 };

/* 키 스케줄 쉬프트 횟수 정의 1,2,9,16번째는 한번, 나머지는 두번 */
static int key_shift_sizes[17] = { -1, 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

/* 각 라운드 별 subkey 치환 */
static int sub_key_permutation[48] = {
	14, 17, 11, 24,  1,  5,
	3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8,
	16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32 };

/* key 생성 변수 end */


/* DES process 관련 변수 start */

/* message initial permutation :
64bit 메세지 초기 치환  */
static int message_ip[64] = {
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6,
	64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17,  9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7 };

/* 메세지 확장 : 32bit => 48bit */
static int message_expansion[48] = {
	32,  1,  2,  3,  4,  5,
	4,  5,  6,  7,  8,  9,
	8,  9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32,  1 };

/* 메세지 축소 : 48bit => 32bit */
static int right_sub_message_permutation[32] = {
	16,  7, 20, 21,
	29, 12, 28, 17,
	1, 15, 23, 26,
	5, 18, 31, 10,
	2,  8, 24, 14,
	32, 27,  3,  9,
	19, 13, 30,  6,
	22, 11,  4, 25 };

/* message inverse permutation : 마지막 메세지 역치환 */
static int message_final_ip[64] = {
	40,  8, 48, 16, 56, 24, 64, 32,
	39,  7, 47, 15, 55, 23, 63, 31,
	38,  6, 46, 14, 54, 22, 62, 30,
	37,  5, 45, 13, 53, 21, 61, 29,
	36,  4, 44, 12, 52, 20, 60, 28,
	35,  3, 43, 11, 51, 19, 59, 27,
	34,  2, 42, 10, 50, 18, 58, 26,
	33,  1, 41,  9, 49, 17, 57, 25 };

/* S-BOX */
static int S1[64] = {
	14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
	0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
	4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
	15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13 };
static int S2[64] = {
	15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
	3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
	0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
	13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9 };
static int S3[64] = {
	10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
	13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
	13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
	1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12 };
static int S4[64] = {
	7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
	13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
	10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
	3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14 };
static int S5[64] = {
	2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
	14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
	4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
	11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3 };
static int S6[64] = {
	12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
	10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
	9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
	4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13 };
static int S7[64] = {
	4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
	13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
	1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
	6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12 };
static int S8[64] = {
	13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
	1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
	7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
	2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11 };

/* DES process 관련 변수 end */

/*
void generate_sub_keys : sub_key 생성 함수
1. input : 64bit main_key, key_set[16]
2. output : 48bit sub_key 16개
*/
void DES::Generate_sub_keys(byte* main_key, key_set* key_sets) {
	int shift_size;
	byte shift_byte, first_shift_bits, second_shift_bits, third_shift_bits, fourth_shift_bits;

	/*
	PC1 start ( Permuted Choice 1 )
	64bit key => 56bit key ( key initial permutation 사용 )
	56bit key => 28bit, 28bit
	*/

	/* key_sets 초기화 */
	for (int i = 0; i < 8; i++) {
		key_sets[0].k[i] = 0;
	}

	/*
	key_ip[i]의 값에 해당하는 main_key 인덱스의 값을
	key_ip 인덱스의 위치로 이동하기 위한 연산
	58bit 키 생성
	*/
	for (int i = 0; i < 56; i++) {
		shift_size = key_ip[i];
		shift_byte = 0x80 >> ((shift_size - 1) % 8);
		shift_byte &= main_key[(shift_size - 1) / 8];
		shift_byte <<= ((shift_byte - 1) % 8);

		key_sets[0].k[i / 8] |= (shift_byte >> i % 8);
	}

	/* 앞부분의 28bit key_set.c */
	for (int i = 0; i < 3; i++) {
		key_sets[0].c[i] = key_sets[0].k[i];
	}
	key_sets[0].c[3] = key_sets[0].k[3] & 0xF0;

	/* 뒷부분의 28bit key_set.d */
	for (int i = 0; i < 3; i++) {
		key_sets[0].d[i] = (key_sets[0].k[i + 3] & 0x0F) << 4;
		key_sets[0].d[i] |= (key_sets[0].k[i + 3] & 0xF0) >> 4;
	}
	key_sets[0].d[3] = (key_sets[0].k[6] & 0x0F) << 4;

	/* PC1 end */

	/* 16 round start */
	for (int i = 1; i < 17; i++) {
		for (int j = 0; j < 4; j++) {
			key_sets[i].c[j] = key_sets[i - 1].c[j];
			key_sets[i].d[j] = key_sets[i - 1].d[j];
		}

		/* shift size 1 = 0x80, 2 = 0xC0 */
		shift_size = key_shift_sizes[i];
		if (shift_size == 1) {
			shift_byte = 0x80;
		}
		else {
			shift_byte = 0xC0;
		}

		/* 28bit key c shift 연산 start */
		first_shift_bits = shift_byte & key_sets[i].c[0];
		second_shift_bits = shift_byte & key_sets[i].c[1];
		third_shift_bits = shift_byte & key_sets[i].c[2];
		fourth_shift_bits = shift_byte & key_sets[i].c[3];

		key_sets[i].c[0] <<= shift_size;
		key_sets[i].c[0] |= (second_shift_bits >> (8 - shift_size));

		key_sets[i].c[1] <<= shift_size;
		key_sets[i].c[1] |= (third_shift_bits >> (8 - shift_size));

		key_sets[i].c[2] <<= shift_size;
		key_sets[i].c[2] |= (fourth_shift_bits >> (8 - shift_size));

		key_sets[i].c[3] <<= shift_size;
		key_sets[i].c[3] |= (first_shift_bits >> (4 - shift_size));
		/* 28bit key c shift 연산 end */

		/* 28bit key d shift 연산 start */
		first_shift_bits = shift_byte & key_sets[i].d[0];
		second_shift_bits = shift_byte & key_sets[i].d[1];
		third_shift_bits = shift_byte & key_sets[i].d[2];
		fourth_shift_bits = shift_byte & key_sets[i].d[3];

		key_sets[i].d[0] <<= shift_size;
		key_sets[i].d[0] |= (second_shift_bits >> (8 - shift_size));

		key_sets[i].d[1] <<= shift_size;
		key_sets[i].d[1] |= (third_shift_bits >> (8 - shift_size));

		key_sets[i].d[2] <<= shift_size;
		key_sets[i].d[2] |= (fourth_shift_bits >> (8 - shift_size));

		key_sets[i].d[3] <<= shift_size;
		key_sets[i].d[3] |= (first_shift_bits >> (4 - shift_size));
		/* 28bit key d shift 연산 end */


		/* 
		PC2 start ( Permuted Choice 2 ) 
		56bit( left, right ) -> 48bit
		*/
		for (int j = 0; j < 48; j++) {
			shift_size = sub_key_permutation[j];
			if (shift_size <= 28) {
				shift_byte = 0x80 >> ((shift_size - 1) % 8);
				shift_byte &= key_sets[i].c[(shift_size - 1) / 8];
				shift_byte <<= ((shift_size - 1) % 8);
			}
			else {
				shift_byte = 0x80 >> ((shift_size - 29) % 8);
				shift_byte &= key_sets[i].d[(shift_size - 29) / 8];
				shift_byte <<= ((shift_size - 29) % 8);
			}

			key_sets[i].k[j / 8] |= (shift_byte >> j % 8);
		}
		/* PC2 end ( Permuted Choice 2 ) */
	}
	/* 16 round end */ 
};

/*
void process_message : message 함호화 함수
1. input : 8byte 원본 message_piece, processed_message, 8byte key_sets, mode
2. output : 암호화 또는 복화화된 processed_piece
*/
void DES::Process_message(byte* message_piece, byte* processed_piece, key_set* key_sets, int mode) {
	int shift_size;
	byte shift_byte;

	byte initial_permutation[8];
	memset(initial_permutation, 0, 8);
	memset(processed_piece, 0, 8);

	/* IP : 64bit 메세지 초기 치환 작업 */
	for (int i = 0; i < 64; i++) {
		shift_size = message_ip[i];
		shift_byte = 0x80 >> ((shift_size - 1) % 8);
		shift_byte &= message_piece[(shift_size - 1) / 8];
		shift_byte <<= ((shift_size - 1) % 8);

		initial_permutation[i / 8] |= (shift_byte >> i % 8);
	}

	/* left 32bit, right 32bit 할당 */
	byte l[4], r[4];
	for (int i = 0; i < 4; i++) {
		l[i] = initial_permutation[i];
		r[i] = initial_permutation[i + 4];
	}

	byte ln[4], rn[4], er[6], ser[4];

	int key_index;
	for (int k = 1; k <= 16; k++) {
		memcpy(ln, r, 4);
		memset(er, 0, 6);

		/* f start */

		/* 메세지 확장 : 32bit => 48bit */
		for (int i = 0; i < 48; i++) {
			shift_size = message_expansion[i];
			shift_byte = 0x80 >> ((shift_size - 1) % 8);
			shift_byte &= r[(shift_size - 1) / 8];
			shift_byte <<= ((shift_size - 1) % 8);

			er[i / 8] |= (shift_byte >> i % 8);
		}

		/* 모드 선택 : Encryption, Decryption */
		if (mode == DECRYPTION_MODE) {
			key_index = 17 - k;
		}
		else {
			key_index = k;
		}

		for (int i = 0; i < 6; i++) {
			er[i] ^= key_sets[key_index].k[i];
		}

		byte row, column;

		for (int i = 0; i < 4; i++) {
			ser[i] = 0;
		}

		/* S-Box start */
		/* Byte 1 */
		row = 0;
		row |= ((er[0] & 0x80) >> 6);
		row |= ((er[0] & 0x04) >> 2);

		column = 0;
		column |= ((er[0] & 0x78) >> 3);

		ser[0] |= ((byte)S1[row * 16 + column] << 4);

		row = 0;
		row |= (er[0] & 0x02);
		row |= ((er[1] & 0x10) >> 4);

		column = 0;
		column |= ((er[0] & 0x01) << 3);
		column |= ((er[1] & 0xE0) >> 5);

		ser[0] |= (byte)S2[row * 16 + column];

		/* Byte 2 */
		row = 0;
		row |= ((er[1] & 0x08) >> 2);
		row |= ((er[2] & 0x40) >> 6);

		column = 0;
		column |= ((er[1] & 0x07) << 1);
		column |= ((er[2] & 0x80) >> 7);

		ser[1] |= ((byte)S3[row * 16 + column] << 4);

		row = 0;
		row |= ((er[2] & 0x20) >> 4);
		row |= (er[2] & 0x01);

		column = 0;
		column |= ((er[2] & 0x1E) >> 1);

		ser[1] |= (byte)S4[row * 16 + column];

		/* Byte 3 */
		row = 0;
		row |= ((er[3] & 0x80) >> 6);
		row |= ((er[3] & 0x04) >> 2);

		column = 0;
		column |= ((er[3] & 0x78) >> 3);

		ser[2] |= ((byte)S5[row * 16 + column] << 4);

		row = 0;
		row |= (er[3] & 0x02);
		row |= ((er[4] & 0x10) >> 4);

		column = 0;
		column |= ((er[3] & 0x01) << 3);
		column |= ((er[4] & 0xE0) >> 5);

		ser[2] |= (byte)S6[row * 16 + column];

		/* Byte 4 */
		row = 0;
		row |= ((er[4] & 0x08) >> 2);
		row |= ((er[5] & 0x40) >> 6);

		column = 0;
		column |= ((er[4] & 0x07) << 1);
		column |= ((er[5] & 0x80) >> 7);

		ser[3] |= ((byte)S7[row * 16 + column] << 4);

		row = 0;
		row |= ((er[5] & 0x20) >> 4);
		row |= (er[5] & 0x01);

		column = 0;
		column |= ((er[5] & 0x1E) >> 1);

		ser[3] |= (byte)S8[row * 16 + column];

		for (int i = 0; i<4; i++) {
			rn[i] = 0;
		}
		/* S-Box end */

		/* permutation 수행 */
		for (int i = 0; i<32; i++) {
			shift_size = right_sub_message_permutation[i];
			shift_byte = 0x80 >> ((shift_size - 1) % 8);
			shift_byte &= ser[(shift_size - 1) / 8];
			shift_byte <<= ((shift_size - 1) % 8);

			rn[i / 8] |= (shift_byte >> i % 8);
		}
		/* f end */

		/* rn = left bits XOR right bits */
		for (int i = 0; i < 4; i++) {
			rn[i] ^= l[i];
		}

		for (int i = 0; i < 4; i++) {
			l[i] = ln[i];
			r[i] = rn[i];
		}
	}

	/* pre_end_permutation = right bits + left bits */
	byte pre_end_permutation[8];
	for (int i = 0; i < 4; i++) {
		pre_end_permutation[i] = r[i];
		pre_end_permutation[4 + i] = l[i];
	}

	/* 마지막 메세지 역치환 작업 */
	for (int i = 0; i<64; i++) {
		shift_size = message_final_ip[i];
		shift_byte = 0x80 >> ((shift_size - 1) % 8);
		shift_byte &= pre_end_permutation[(shift_size - 1) / 8];
		shift_byte <<= ((shift_size - 1) % 8);

		processed_piece[i / 8] |= (shift_byte >> i % 8);
	}
}

/*
DES set_DES() : DES의 초기작업
1. 변수 초기 공간 할당 ( data_block, processed_block, key_sets )
*/
void DES::Set_DES() {
	data_block = (byte*)malloc(8 * sizeof(char));
	processed_block = (byte*)malloc(8 * sizeof(char));
	key_sets = (key_set*)malloc(17 * sizeof(key_set));
}

/*
void DES_encode() : DES 암호화 작업 수행
*/
void DES::Encryption() {

	Set_DES();

	/* 파일을 64bits 만큼 읽어 들임 */
	while (fread(data_block, 1, 8, input_file)) {
		block_count++;
		if (block_count == number_of_blocks) {

			padding = 8 - file_size % 8;

			/* 패딩으로 빈 데이터 블록 바이트 채움 */
			if (padding < 8) {
				memset((data_block + 8 - padding), (byte)padding, padding);
			}

			Process_message(data_block, processed_block, key_sets, ENCRYPTION_MODE);
			bytes_written = fwrite(processed_block, 1, 8, output_file);

			/* 패팅을 위한 여분의 블록 작성 */
			if (padding == 8) {
				memset(data_block, (byte)padding, 8);
				Process_message(data_block, processed_block, key_sets, ENCRYPTION_MODE);
				bytes_written = fwrite(processed_block, 1, 8, output_file);
			}
		}
		else {
			Process_message(data_block, processed_block, key_sets, ENCRYPTION_MODE);
			bytes_written = fwrite(processed_block, 1, 8, output_file);
		}
		memset(data_block, 0, 8);
	}

	free(data_block);
	free(processed_block);
	fclose(input_file);
	fclose(output_file);
}

/*
void DES_decode() : DES 복호화 작업 수행
*/
void DES::Decryption() {
	Set_DES();
	while (fread(data_block, 1, 8, input_file)) {
		block_count++;
		if (block_count == number_of_blocks) {

			Process_message(data_block, processed_block, key_sets, DECRYPTION_MODE);
			padding = processed_block[7];

			if (padding < 8) {
				bytes_written = fwrite(processed_block, 1, 8 - padding, output_file);
			}
		}
		else {
			Process_message(data_block, processed_block, key_sets, DECRYPTION_MODE);
			bytes_written = fwrite(processed_block, 1, 8, output_file);
		}
		memset(data_block, 0, 8);
	}

	free(data_block);
	free(processed_block);
	fclose(input_file);
	fclose(output_file);
}
