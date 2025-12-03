#include "utils.h"

static const int mod_table[] = { 0, 2, 1 };
static const unsigned char xor_key = 0x5A;

static const unsigned char encrypted_data[] = {
	0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1, 0xE0, 0xDF, 0xDE, 0xDD, 0xDC,
	0xDB, 0xDA, 0xD9, 0xD8, 0xD7, 0xD6, 0xD5, 0xD4, 0xD3, 0xD2, 0x8B, 0x8A, 0x89, 0x88, 0x87, 0x86,
	0x85, 0x84, 0x83, 0x82, 0x81, 0x80, 0x9F, 0x9E, 0x9D, 0x9C, 0x9B, 0x9A, 0x99, 0x98, 0x97, 0x96,
	0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xE2, 0xE3, 0x00
};

static char* getEncodingTable() {
	static char table[65] = { 0 };
	if (table[0] == 0) {
		for (int i = 0; i < 64; i++) {
			table[i] = encrypted_data[i] ^ xor_key;
		}
		table[64] = '\0';
	}
	return table;
}

char* base64_encode(const unsigned char* data, size_t input_length) {
	const char* encoding_table = getEncodingTable();
	static const char padding_char = '=';

	size_t output_length = 4 * ((input_length + 2) / 3);
	char* encoded_data = new char[output_length + 1];
	if (encoded_data == nullptr) return nullptr;
	for (size_t i = 0, j = 0; i < input_length;) {
		uint32_t octet_a = i < input_length ? data[i++] : 0;
		uint32_t octet_b = i < input_length ? data[i++] : 0;
		uint32_t octet_c = i < input_length ? data[i++] : 0;
		uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;
		encoded_data[j++] = encoding_table[(triple >> 18) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 12) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 6) & 0x3F];
		encoded_data[j++] = encoding_table[triple & 0x3F];
	}
	for (size_t i = 0; i < mod_table[input_length % 3]; i++)
		encoded_data[output_length - 1 - i] = padding_char;
	encoded_data[output_length] = '\0';
	return encoded_data;
}