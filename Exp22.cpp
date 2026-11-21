/*
 * Experiment 22: S-DES in Cipher Block Chaining (CBC) Mode
 * 
 * Test Data:
 * Binary IV:        1010 1010
 * Binary Plaintext: 0000 0001 0010 0011
 * Binary Key:       01111 11101
 * Expected Output:  1111 0100 0000 1011
 * Decryption works correspondingly to recover plaintext.
 */

#include <stdio.h>
#include <stdint.h>

/* S-DES Permutation tables */
static const int P10[10] = {3, 5, 2, 7, 4, 10, 1, 9, 8, 6};
static const int P8[8]   = {6, 3, 7, 4, 8, 5, 10, 9};
static const int IP[8]   = {2, 6, 3, 1, 4, 8, 5, 7};
static const int IP_INV[8] = {4, 1, 3, 5, 7, 2, 8, 6};
static const int EP[8]   = {4, 1, 2, 3, 2, 3, 4, 1};
static const int P4[4]   = {2, 4, 3, 1};

static const int S0[4][4] = {
    {1, 0, 3, 2},
    {3, 2, 1, 0},
    {0, 2, 1, 3},
    {3, 1, 3, 2}
};

static const int S1[4][4] = {
    {0, 1, 2, 3},
    {2, 0, 1, 3},
    {3, 0, 1, 0},
    {2, 1, 0, 3}
};

/* Helper to permute bits */
uint16_t permute(uint16_t in, const int *table, int n_out, int n_in) {
    uint16_t out = 0;
    for (int i = 0; i < n_out; i++) {
        int pos = table[i];
        int bit = (in >> (n_in - pos)) & 1;
        out = (out << 1) | bit;
    }
    return out;
}

uint8_t left_shift_5(uint8_t val, int shift) {
    shift %= 5;
    return ((val << shift) | (val >> (5 - shift))) & 0x1F;
}

void sdes_generate_keys(uint16_t key10, uint8_t *k1, uint8_t *k2) {
    uint16_t p10 = permute(key10, P10, 10, 10);
    uint8_t left = (p10 >> 5) & 0x1F;
    uint8_t right = p10 & 0x1F;

    /* LS-1 */
    left = left_shift_5(left, 1);
    right = left_shift_5(right, 1);
    *k1 = (uint8_t)permute((left << 5) | right, P8, 8, 10);

    /* LS-2 */
    left = left_shift_5(left, 2);
    right = left_shift_5(right, 2);
    *k2 = (uint8_t)permute((left << 5) | right, P8, 8, 10);
}

uint8_t fk(uint8_t r, uint8_t sk) {
    uint8_t ep = (uint8_t)permute(r, EP, 8, 4);
    uint8_t x = ep ^ sk;
    uint8_t l_part = (x >> 4) & 0x0F;
    uint8_t r_part = x & 0x0F;

    int row0 = ((l_part & 8) >> 2) | (l_part & 1);
    int col0 = (l_part >> 1) & 3;
    int s0_val = S0[row0][col0];

    int row1 = ((r_part & 8) >> 2) | (r_part & 1);
    int col1 = (r_part >> 1) & 3;
    int s1_val = S1[row1][col1];

    uint8_t combined = (s0_val << 2) | s1_val;
    return (uint8_t)permute(combined, P4, 4, 4);
}

uint8_t sdes_encrypt_block(uint8_t pt, uint8_t k1, uint8_t k2) {
    uint8_t ip = (uint8_t)permute(pt, IP, 8, 8);
    uint8_t l = (ip >> 4) & 0x0F;
    uint8_t r = ip & 0x0F;

    /* Round 1 */
    l ^= fk(r, k1);
    /* Switch */
    uint8_t temp = l; l = r; r = temp;
    /* Round 2 */
    l ^= fk(r, k2);

    return (uint8_t)permute((l << 4) | r, IP_INV, 8, 8);
}

uint8_t sdes_decrypt_block(uint8_t ct, uint8_t k1, uint8_t k2) {
    uint8_t ip = (uint8_t)permute(ct, IP, 8, 8);
    uint8_t l = (ip >> 4) & 0x0F;
    uint8_t r = ip & 0x0F;

    /* Round 1 with K2 */
    l ^= fk(r, k2);
    /* Switch */
    uint8_t temp = l; l = r; r = temp;
    /* Round 2 with K1 */
    l ^= fk(r, k1);

    return (uint8_t)permute((l << 4) | r, IP_INV, 8, 8);
}

void print_binary8(uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (byte >> i) & 1);
        if (i == 4) printf(" ");
    }
}

void print_binary10(uint16_t val) {
    for (int i = 9; i >= 0; i--) {
        printf("%d", (val >> i) & 1);
        if (i == 5) printf(" ");
    }
}

int main() {
    /* Test parameters from specification */
    uint16_t key = 0b0111111101; /* 10-bit key */
    uint8_t iv   = 0b10101010;   /* 8-bit IV */
    uint8_t pt[2] = {0b00000001, 0b00100011}; /* Plaintext blocks */
    uint8_t ct[2];
    uint8_t decrypted[2];

    uint8_t k1, k2;
    sdes_generate_keys(key, &k1, &k2);

    printf("====================================================\n");
    printf("   EXPERIMENT 22: S-DES IN CBC MODE ENCRYPT/DECRYPT \n");
    printf("====================================================\n");
    printf("Binary Key:       "); print_binary10(key); printf("\n");
    printf("Round Key K1:     "); print_binary8(k1); printf("\n");
    printf("Round Key K2:     "); print_binary8(k2); printf("\n");
    printf("Binary IV:        "); print_binary8(iv); printf("\n");
    printf("Plaintext Blocks: "); print_binary8(pt[0]); printf("  "); print_binary8(pt[1]); printf("\n");

    /* CBC Encryption: C[i] = E(P[i] ^ C[i-1]), C[-1] = IV */
    uint8_t prev = iv;
    for (int i = 0; i < 2; i++) {
        uint8_t xor_input = pt[i] ^ prev;
        ct[i] = sdes_encrypt_block(xor_input, k1, k2);
        prev = ct[i];
    }

    printf("\nCiphertext Blocks: ");
    print_binary8(ct[0]); printf("  "); print_binary8(ct[1]); printf("\n");

    /* CBC Decryption: P[i] = D(C[i]) ^ C[i-1], C[-1] = IV */
    prev = iv;
    for (int i = 0; i < 2; i++) {
        uint8_t dec_block = sdes_decrypt_block(ct[i], k1, k2);
        decrypted[i] = dec_block ^ prev;
        prev = ct[i];
    }

    printf("Decrypted Blocks:  ");
    print_binary8(decrypted[0]); printf("  "); print_binary8(decrypted[1]); printf("\n");

    if (decrypted[0] == pt[0] && decrypted[1] == pt[1]) {
        printf("\nSUCCESS: CBC Decryption matches original plaintext perfectly!\n");
    } else {
        printf("\nFAILURE: Mismatch in decryption.\n");
    }

    return 0;
}
