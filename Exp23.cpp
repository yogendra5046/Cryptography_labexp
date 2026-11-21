/*
 * Experiment 23: S-DES in Counter (CTR) Mode
 * 
 * Test Data:
 * Counter start:    0000 0000
 * Binary Key:       01111 11101
 * Binary Plaintext: 0000 0001 0000 0010 0000 0100 (3 blocks)
 * Expected Output:  0011 1000 0100 1111 0011 0010
 * Decryption works correspondingly in CTR mode.
 */

#include <stdio.h>
#include <stdint.h>

/* S-DES Tables */
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

    left = left_shift_5(left, 1);
    right = left_shift_5(right, 1);
    *k1 = (uint8_t)permute((left << 5) | right, P8, 8, 10);

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

    l ^= fk(r, k1);
    uint8_t temp = l; l = r; r = temp;
    l ^= fk(r, k2);

    return (uint8_t)permute((l << 4) | r, IP_INV, 8, 8);
}

void print_binary8(uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (byte >> i) & 1);
        if (i == 4) printf(" ");
    }
}

int main() {
    uint16_t key = 0b0111111101;
    uint8_t k1, k2;
    sdes_generate_keys(key, &k1, &k2);

    uint8_t pt[3] = {0b00000001, 0b00000010, 0b00000100};
    uint8_t ct[3];
    uint8_t decrypted[3];
    uint8_t counter_start = 0b00000000;

    printf("====================================================\n");
    printf("   EXPERIMENT 23: S-DES IN COUNTER (CTR) MODE       \n");
    printf("====================================================\n");
    printf("Binary Key: ");
    for (int i = 9; i >= 0; i--) {
        printf("%d", (key >> i) & 1);
        if (i == 5) printf(" ");
    }
    printf("\n");
    printf("Counter Start: "); print_binary8(counter_start); printf("\n");
    printf("Plaintext:     ");
    for (int i = 0; i < 3; i++) {
        print_binary8(pt[i]); printf("  ");
    }
    printf("\n\n--- ENCRYPTION ---\n");

    /* CTR Mode Encryption: C_i = P_i ^ E_K(CTR_i) */
    for (int i = 0; i < 3; i++) {
        uint8_t current_counter = (counter_start + i) & 0xFF;
        uint8_t keystream = sdes_encrypt_block(current_counter, k1, k2);
        ct[i] = pt[i] ^ keystream;
        printf("Block %d: CTR = ", i); print_binary8(current_counter);
        printf(", Keystream = "); print_binary8(keystream);
        printf(", Ciphertext = "); print_binary8(ct[i]);
        printf("\n");
    }

    printf("\nFull Ciphertext: ");
    for (int i = 0; i < 3; i++) {
        print_binary8(ct[i]); printf("  ");
    }
    printf("\n");

    /* CTR Mode Decryption: P_i = C_i ^ E_K(CTR_i) */
    printf("\n--- DECRYPTION ---\n");
    for (int i = 0; i < 3; i++) {
        uint8_t current_counter = (counter_start + i) & 0xFF;
        uint8_t keystream = sdes_encrypt_block(current_counter, k1, k2);
        decrypted[i] = ct[i] ^ keystream;
        printf("Block %d: Decrypted = ", i); print_binary8(decrypted[i]);
        printf("\n");
    }

    printf("\nDecrypted Plaintext: ");
    for (int i = 0; i < 3; i++) {
        print_binary8(decrypted[i]); printf("  ");
    }
    printf("\n");

    return 0;
}
