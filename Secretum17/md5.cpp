/*
MD5 process
Step 1. Append Padding Bits
Step 2. Append Length
Step 3. Initialize MD Buffer
Step 4. Process Message in 16-Word Blocks
Step 5. Output
*/
#include "wrapper.h"
#include "md5.h"
#include <string>
#include <cstring>
/*
S : MD5Trandform routine.
1. round 과정의 shift 횟수 지정 값
*/
#define S11 7 
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

/*
F, G, H, I : Basic MD5 functions
1. param : 32bit 변수들
2. return : one 32bit
*/
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/*
ROTATE_LEFT : x 비트를 n 비트만큼 왼쪽으로 회전
1. param : 32bit 변수
2. return : bits after rotation
*/
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/*
FF, GG, HH, II : rounds 1, 2, 3, 4의 변환 과정
*/
#define FF(a, b, c, d, x, s, ac) { \
    (a) += F ((b), (c), (d)) + (x) + (bit32)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) { \
    (a) += G ((b), (c), (d)) + (x) + (bit32)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
    (a) += H ((b), (c), (d)) + (x) + (bit32)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
    (a) += I ((b), (c), (d)) + (x) + (bit32)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
}

/*
MD5Init : count, state 초기화 작업
1. state[] : 미리 초기화 값이 정해져 있음
2. little endian 형태로 구현되었기 때문에 low -> high 순서로 저장됨
*/
void MD5::MD5_init() {
	
	count[0] = count[1] = 0;

	state[0] = 0x67452301;
	state[1] = 0xefcdab89;
	state[2] = 0x98badcfe;
	state[3] = 0x10325476;
}

/*
MD5Update : MD5 블록 업데이트 작업
*/
void MD5::MD5_update(byte *input, unsigned long input_len) {
	
	int i;
	/*
	shift 연산 >> 3 : bit32 -> byte 변환
	비트 연산 0x3F : x % 64 의 값
	part_len : 전체길이 - 현재 index = 현재 처리 중인 블럭의 길이
	*/
	bit32 index = (bit32)((count[0] >> 3) & 0x3F);
	bit32 part_len = 64 - index;

	/*
	input_len의 값을 비트 값으로 변환하여 count에 저장
	패딩 작업을 위한 비트 수 확인
	little endian 방식이기 때문에 count[0] : low, count[1] : high
	if() 조건이 참인 경우는 low 영역에서 자리수 올림이 발생할 경우
	high 영역의 값을 1 증가
	*/
	if ((count[0] += ((bit32)input_len << 3)) < 
		((bit32)input_len << 3))
		count[1]++;
	count[1] += ((bit32)input_len >> 29);
	
	/*
	변환되지 않은 부분이 남아 있을 경우
	변환 작업을 수행 ( buffer, input )
	*/
	if (input_len >= part_len) {
		
		memcpy(&buffer[index], input, part_len);
		MD5_trnasform(buffer);

		for (i = part_len; i + 63 < input_len; i += 64)
			MD5_trnasform(&input[i]);

		index = 0;
	}
	else
		i = 0;

	/* 나머지 buffer 입력 */
	memcpy(&buffer[index], &input[i], input_len - i);
}

/*
MD5Final : message digest, 초기화 작업
1. param : 압축될 결과 digest
2. 알고리즘의 Step 1( Append Padding Bits ),
	Step 2( Append Length ) 수행
*/
void MD5::MD5_final(byte digest[16]) {
	
	byte bits[8];
	bit32 index, pad_len;

	/* bit32 -> byte 변환 작업 */
	Encode(bits, count, 8);

	/* 
	Step 1 : 56 mode 64가 되도록 패딩 작업 
	*/
	index = (bit32)((count[0] >> 3) & 0x3f);
	pad_len = (index < 56) ? (56 - index) : (120 - index);
	MD5_update(PADDING, pad_len);

	/* Step 2 : 원본 메세지의 길이 패딩 작업 */
	MD5_update(bits, 8);

	/* 결과 값 변환 */
	Encode(digest, state, 16);

	/* 문맥 정보 초기화 */
	memset(state, 0, sizeof(state));
	memset(count, 0, sizeof(count));
	memset(buffer, 0, sizeof(buffer));
}

/*
MD5Transform : 블록을 기반으로 한 MD5 변환 작업
1. param : 64 byte 블록 ( 512 bit )
*/
void MD5::MD5_trnasform(byte block[64]) {

	bit32 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

	/* byte[] block -> bit32[] x */
	Decode(x, block, 64);

	/* Round 1 */
	FF(a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
	FF(d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
	FF(c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
	FF(b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
	FF(a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
	FF(d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
	FF(c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
	FF(b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
	FF(a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
	FF(d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
	FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	/* Round 2 */
	GG(a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
	GG(d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
	GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG(b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
	GG(a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
	GG(d, a, b, c, x[10], S22,  0x2441453); /* 22 */
	GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG(b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
	GG(a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
	GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG(c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
	GG(b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
	GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG(d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
	GG(c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
	GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH(a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
	HH(d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
	HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH(a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
	HH(d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
	HH(c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
	HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH(d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
	HH(c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
	HH(b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
	HH(a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
	HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH(b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

	/* Round 4 */
	II(a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
	II(d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
	II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II(b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
	II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II(d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
	II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II(b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
	II(a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
	II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II(c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
	II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II(a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
	II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II(c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
	II(b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

	/* 문맥 x 초기화 */
	memset(x, 0, sizeof(x));
}


/*
Encode : 32 bit input을 1 byte output으로 변환
1. parm : 변환 후 값이 저장될 1 byte output, 32 bits input 값, 
		input의 길이 input_len
2. little endian으로 저장하기 위한 비트 연산 0xff
*/
void MD5::Encode(byte *output, bit32 *input, unsigned int input_len) {

	for (int i = 0, j = 0; j < input_len; i++, j += 4) {
		output[j] = (byte)(input[i] & 0xff);
		output[j+1] = (byte)((input[i] >> 8) & 0xff);
		output[j+2] = (byte)((input[i] >> 8) & 0xff);
		output[j+3] = (byte)((input[i] >> 8) & 0xff);
	}
}

/*
Decode : 1 byte input을 32 bits output으로 변환
1. parm : 변환 후 값이 저장될 32 bits output, 1 byte input 값, 
		input의 길이 input_len
*/
void MD5::Decode(bit32 *output, byte *input, unsigned int input_len) {
	
	for (int i = 0, j = 0; j < input_len; i++, j += 4) {
		output[i] = ((bit32)input[j]) | (((bit32)input[j + 1]) << 8) |
			(((bit32)input[j + 2]) << 16) | (((bit32)input[j + 3]) << 24);
	}
}


char MD5::hb2hex(unsigned char hb) {
	hb = hb & 0xF;
	return hb < 10 ? '0' + hb : hb - 10 + 'a';
}

string MD5::MD5_file() {

	MD5_init();
	size_t len = 0;
	while ((len = fread(buff, 1, BUFSIZ, input_file)) > 0) {
		MD5_update(buff, len);
	}
	MD5_final(output);

	string res;
	for (int i = 0; i < 16; ++i) {
		res.push_back(hb2hex(output[i] >> 4));
		res.push_back(hb2hex(output[i]));
	}

	cout << res << endl;
	//fwrite(res.c_str(), sizeof(char) , res.length, output_file);
	return res;
}


