/*
 * Experiment 21: Padding in ECB, CBC, and CFB Modes
 * 
 * Demonstrates:
 * - Block padding using "1 bit followed by zeros" (0x80 followed by 0x00 bytes)
 * - Handling messages where length is NOT a multiple of block size
 * - Handling messages where length IS an exact multiple of block size (adds full padding block)
 * - ECB, CBC, and CFB mode encryption and decryption
 * - Unambiguous unpadding
 * 
 * Question: What is the motivation for including a padding block when padding is not needed?
 * Answer: If a message that is already a multiple of the block size were not padded,
 * the receiver would have no way of knowing whether the trailing bytes of the final block
 * were part of the original plaintext or padding bytes. By mandating that padding is
 * ALWAYS added, the receiver can unconditionally strip the padding, guaranteeing
 * unambiguous message recovery.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 8

/* Simple toy block cipher for demonstration (reversible substitution-permutation) */
void block_encrypt(const unsigned char in[BLOCK_SIZE], unsigned char out[BLOCK_SIZE], unsigned char key) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        out[i] = (unsigned char)(((in[i] ^ key) + 3) & 0xFF);
    }
}

void block_decrypt(const unsigned char in[BLOCK_SIZE], unsigned char out[BLOCK_SIZE], unsigned char key) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        out[i] = (unsigned char)(((in[i] - 3) ^ key) & 0xFF);
    }
}

/* 
 * Pad message using 1-bit followed by zeros (0x80 then 0x00...0x00).
 * Always adds padding even if length is already a multiple of BLOCK_SIZE.
 */
int pad_message(const unsigned char *in, int in_len, unsigned char *out) {
    memcpy(out, in, in_len);
    int pad_len = BLOCK_SIZE - (in_len % BLOCK_SIZE);
    out[in_len] = 0x80; /* '1' bit followed by seven '0' bits */
    for (int i = 1; i < pad_len; i++) {
        out[in_len + i] = 0x00;
    }
    return in_len + pad_len;
}

/* Unpad message: remove trailing zeros and the 0x80 byte */
int unpad_message(const unsigned char *in, int padded_len, unsigned char *out) {
    int i = padded_len - 1;
    while (i >= 0 && in[i] == 0x00) {
        i--;
    }
    if (i < 0 || in[i] != 0x80) {
        printf("Error: Invalid padding!\n");
        return -1;
    }
    int original_len = i;
    memcpy(out, in, original_len);
    out[original_len] = '\0';
    return original_len;
}

void print_hex(const char *label, const unsigned char *data, int len) {
    printf("%-20s (%2d bytes): ", label, len);
    for (int i = 0; i < len; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

/* ECB Mode */
void encrypt_ecb(const unsigned char *pt, int len, unsigned char *ct, unsigned char key) {
    for (int i = 0; i < len; i += BLOCK_SIZE) {
        block_encrypt(pt + i, ct + i, key);
    }
}

void decrypt_ecb(const unsigned char *ct, int len, unsigned char *pt, unsigned char key) {
    for (int i = 0; i < len; i += BLOCK_SIZE) {
        block_decrypt(ct + i, pt + i, key);
    }
}

/* CBC Mode */
void encrypt_cbc(const unsigned char *pt, int len, unsigned char *ct, const unsigned char *iv, unsigned char key) {
    unsigned char prev[BLOCK_SIZE];
    memcpy(prev, iv, BLOCK_SIZE);
    for (int i = 0; i < len; i += BLOCK_SIZE) {
        unsigned char block[BLOCK_SIZE];
        for (int j = 0; j < BLOCK_SIZE; j++) block[j] = pt[i + j] ^ prev[j];
        block_encrypt(block, ct + i, key);
        memcpy(prev, ct + i, BLOCK_SIZE);
    }
}

void decrypt_cbc(const unsigned char *ct, int len, unsigned char *pt, const unsigned char *iv, unsigned char key) {
    unsigned char prev[BLOCK_SIZE];
    memcpy(prev, iv, BLOCK_SIZE);
    for (int i = 0; i < len; i += BLOCK_SIZE) {
        unsigned char decrypted[BLOCK_SIZE];
        block_decrypt(ct + i, decrypted, key);
        for (int j = 0; j < BLOCK_SIZE; j++) pt[i + j] = decrypted[j] ^ prev[j];
        memcpy(prev, ct + i, BLOCK_SIZE);
    }
}

/* CFB Mode (8-byte feedback segment) */
void encrypt_cfb(const unsigned char *pt, int len, unsigned char *ct, const unsigned char *iv, unsigned char key) {
    unsigned char feedback[BLOCK_SIZE];
    memcpy(feedback, iv, BLOCK_SIZE);
    for (int i = 0; i < len; i += BLOCK_SIZE) {
        unsigned char out[BLOCK_SIZE];
        block_encrypt(feedback, out, key);
        for (int j = 0; j < BLOCK_SIZE; j++) {
            ct[i + j] = pt[i + j] ^ out[j];
        }
        memcpy(feedback, ct + i, BLOCK_SIZE);
    }
}

void decrypt_cfb(const unsigned char *ct, int len, unsigned char *pt, const unsigned char *iv, unsigned char key) {
    unsigned char feedback[BLOCK_SIZE];
    memcpy(feedback, iv, BLOCK_SIZE);
    for (int i = 0; i < len; i += BLOCK_SIZE) {
        unsigned char out[BLOCK_SIZE];
        block_encrypt(feedback, out, key);
        for (int j = 0; j < BLOCK_SIZE; j++) {
            pt[i + j] = ct[i + j] ^ out[j];
        }
        memcpy(feedback, ct + i, BLOCK_SIZE);
    }
}

void run_test(const char *msg_str, unsigned char key, const unsigned char *iv) {
    int msg_len = (int)strlen(msg_str);
    unsigned char padded[128], decrypted_padded[128], unpadded[128];
    unsigned char ct[128];

    printf("\n======================================================\n");
    printf("Original Text: \"%s\" (Length: %d bytes)\n", msg_str, msg_len);
    
    int padded_len = pad_message((const unsigned char *)msg_str, msg_len, padded);
    print_hex("Padded plaintext", padded, padded_len);

    /* ECB */
    encrypt_ecb(padded, padded_len, ct, key);
    print_hex("ECB Ciphertext", ct, padded_len);
    decrypt_ecb(ct, padded_len, decrypted_padded, key);
    unpad_message(decrypted_padded, padded_len, unpadded);
    printf("ECB Decrypted Text: \"%s\"\n", unpadded);

    /* CBC */
    encrypt_cbc(padded, padded_len, ct, iv, key);
    print_hex("CBC Ciphertext", ct, padded_len);
    decrypt_cbc(ct, padded_len, decrypted_padded, iv, key);
    unpad_message(decrypted_padded, padded_len, unpadded);
    printf("CBC Decrypted Text: \"%s\"\n", unpadded);

    /* CFB */
    encrypt_cfb(padded, padded_len, ct, iv, key);
    print_hex("CFB Ciphertext", ct, padded_len);
    decrypt_cfb(ct, padded_len, decrypted_padded, iv, key);
    unpad_message(decrypted_padded, padded_len, unpadded);
    printf("CFB Decrypted Text: \"%s\"\n", unpadded);
}

int main() {
    unsigned char key = 0x5A;
    unsigned char iv[BLOCK_SIZE] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};

    printf("--- Experiment 21: Padding in ECB, CBC, and CFB Modes ---\n");

    /* Case 1: Incomplete block (5 bytes) */
    run_test("HELLO", key, iv);

    /* Case 2: Exactly complete block (8 bytes) -> extra full padding block added */
    run_test("COMPUTER", key, iv);

    printf("\n------------------------------------------------------\n");
    printf("MOTIVATION FOR INCLUDING PADDING WHEN NOT NEEDED:\n");
    printf("If an already-complete block were not padded, the receiver would\n");
    printf("not know whether trailing bytes (like 0x80 0x00) are user data or\n");
    printf("padding. Always adding a padding block guarantees unambiguous removal!\n");
    printf("------------------------------------------------------\n");

    return 0;
}
