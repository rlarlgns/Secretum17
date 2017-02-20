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

#include "wrapper.h"
#include "des.h"

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
void DES::DES_encode() {

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
void DES::DES_decode() {
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
