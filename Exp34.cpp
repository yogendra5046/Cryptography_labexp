/*
 * Experiment 34: Block Padding Analysis in ECB, CBC, and CFB Modes
 * 
 * Question:
 * One common method of padding, if needed, consists of a 1 bit followed
 * by as few zero bits, possibly none, as are necessary to complete the
 * final block. It is considered good practice for the sender to pad every
 * message, including messages in which the final message block is already complete.
 * What is the motivation for including a padding block when padding is not needed?
 * 
 * Answer & Demonstration:
 * 1. UNAMBIGUOUS DE-PADDING:
 *    If the rule were "only pad when the message does not fill a full block",
 *    then for an unpadded message that naturally ends with a byte sequence
 *    resembling padding (e.g. 0x80 followed by 0x00...0x00), the receiver
 *    would have no way to determine whether those trailing bytes were part
 *    of the original message or padding!
 * 2. CONSISTENT RECEIVER LOGIC:
 *    When EVERY message is padded (even adding an entire dummy block of
 *    [0x80, 0x00, ..., 0x00] when already block-aligned), the receiver's
 *    depadding logic is 100% unconditional:
 *    - Search backwards from the end of the last block for the first '1' bit (0x80).
 *    - Discard that bit and all following zeros.
 *    This guarantees that the original message is recovered without data loss or corruption.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 8

/* Simple substitution-permutation block cipher */
void encrypt_block(const unsigned char in[BLOCK_SIZE], unsigned char out[BLOCK_SIZE], unsigned char key) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        out[i] = (unsigned char)(((in[i] + key) ^ 0x5C) & 0xFF);
    }
}

void decrypt_block(const unsigned char in[BLOCK_SIZE], unsigned char out[BLOCK_SIZE], unsigned char key) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        out[i] = (unsigned char)(((in[i] ^ 0x5C) - key) & 0xFF);
    }
}

/* Bit padding: 0x80 followed by 0x00 bytes */
int apply_mandatory_padding(const unsigned char *in, int len, unsigned char *out) {
    memcpy(out, in, len);
    int pad_len = BLOCK_SIZE - (len % BLOCK_SIZE);
    out[len] = 0x80;
    for (int i = 1; i < pad_len; i++) {
        out[len + i] = 0x00;
    }
    return len + pad_len;
}

int remove_mandatory_padding(const unsigned char *in, int padded_len, unsigned char *out) {
    int i = padded_len - 1;
    while (i >= 0 && in[i] == 0x00) {
        i--;
    }
    if (i < 0 || in[i] != 0x80) {
        printf("Error: Invalid padding sequence!\n");
        return -1;
    }
    int orig_len = i;
    memcpy(out, in, orig_len);
    out[orig_len] = '\0';
    return orig_len;
}

void print_bytes(const char *label, const unsigned char *buf, int len) {
    printf("%-24s (%2d bytes): ", label, len);
    for (int i = 0; i < len; i++) {
        printf("%02X ", buf[i]);
    }
    printf("\n");
}

/* CBC Mode Encrypt / Decrypt */
void cbc_encrypt(const unsigned char *pt, int len, unsigned char *ct, const unsigned char *iv, unsigned char key) {
    unsigned char prev[BLOCK_SIZE];
    memcpy(prev, iv, BLOCK_SIZE);
    for (int i = 0; i < len; i += BLOCK_SIZE) {
        unsigned char blk[BLOCK_SIZE];
        for (int j = 0; j < BLOCK_SIZE; j++) blk[j] = pt[i + j] ^ prev[j];
        encrypt_block(blk, ct + i, key);
        memcpy(prev, ct + i, BLOCK_SIZE);
    }
}

void cbc_decrypt(const unsigned char *ct, int len, unsigned char *pt, const unsigned char *iv, unsigned char key) {
    unsigned char prev[BLOCK_SIZE];
    memcpy(prev, iv, BLOCK_SIZE);
    for (int i = 0; i < len; i += BLOCK_SIZE) {
        unsigned char dec[BLOCK_SIZE];
        decrypt_block(ct + i, dec, key);
        for (int j = 0; j < BLOCK_SIZE; j++) pt[i + j] = dec[j] ^ prev[j];
        memcpy(prev, ct + i, BLOCK_SIZE);
    }
}

int main() {
    unsigned char key = 0x7E;
    unsigned char iv[BLOCK_SIZE] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11, 0x22};

    printf("====================================================\n");
    printf("   EXPERIMENT 34: MANDATORY BLOCK PADDING ANALYSIS  \n");
    printf("====================================================\n\n");

    /* Case A: Incomplete block */
    printf("--- CASE A: Message NOT a multiple of block size (6 bytes) ---\n");
    const char *msgA = "SECRET";
    unsigned char paddedA[64], ctA[64], decA[64], recA[64];
    int lenA = (int)strlen(msgA);
    int pad_lenA = apply_mandatory_padding((const unsigned char *)msgA, lenA, paddedA);
    print_bytes("Original Plaintext", (const unsigned char *)msgA, lenA);
    print_bytes("Padded Plaintext", paddedA, pad_lenA);
    cbc_encrypt(paddedA, pad_lenA, ctA, iv, key);
    print_bytes("CBC Ciphertext", ctA, pad_lenA);
    cbc_decrypt(ctA, pad_lenA, decA, iv, key);
    remove_mandatory_padding(decA, pad_lenA, recA);
    printf("Unpadded Plaintext:      \"%s\"\n\n", recA);

    /* Case B: Exactly complete block (8 bytes) -> MANDATORY EXTRA BLOCK ADDED */
    printf("--- CASE B: Message EXACT multiple of block size (8 bytes) ---\n");
    const char *msgB = "SECURITY";
    unsigned char paddedB[64], ctB[64], decB[64], recB[64];
    int lenB = (int)strlen(msgB);
    int pad_lenB = apply_mandatory_padding((const unsigned char *)msgB, lenB, paddedB);
    print_bytes("Original Plaintext", (const unsigned char *)msgB, lenB);
    print_bytes("Padded Plaintext", paddedB, pad_lenB);
    printf("   Notice: An entire 8-byte padding block [80 00 00 ...] was appended!\n");
    cbc_encrypt(paddedB, pad_lenB, ctB, iv, key);
    print_bytes("CBC Ciphertext", ctB, pad_lenB);
    cbc_decrypt(ctB, pad_lenB, decB, iv, key);
    remove_mandatory_padding(decB, pad_lenB, recB);
    printf("Unpadded Plaintext:      \"%s\"\n\n", recB);

    /* Theoretical Demonstration of Failure WITHOUT mandatory padding */
    printf("----------------------------------------------------\n");
    printf("WHY IS THE EXTRA PADDING BLOCK ESSENTIAL?\n");
    printf("Suppose an 8-byte message ended naturally with byte 0x80 followed by 0x00.\n");
    printf("If sender DID NOT append a full padding block, receiver would look at\n");
    printf("those trailing bytes and mistakenly strip them off as padding,\n");
    printf("permanently corrupting the user's data!\n");
    printf("Always adding padding guarantees unambiguous message extraction.\n");
    printf("----------------------------------------------------\n");

    return 0;
}
