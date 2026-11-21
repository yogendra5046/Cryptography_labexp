/*
 * Experiment 31: CMAC Subkey Generation
 * 
 * Background (NIST SP 800-38B / RFC 4493):
 * To compute CMAC, two subkeys (K1, K2) are derived from the cipher key K:
 * 1. L = E_K(0^b)  [Block cipher applied to all-zero block]
 * 2. If MSB(L) == 0:  K1 = L << 1
 *    Else:             K1 = (L << 1) ^ R_b
 * 3. If MSB(K1) == 0: K2 = K1 << 1
 *    Else:             K2 = (K1 << 1) ^ R_b
 * 
 * Questions & Answers:
 * a. What constants are needed for block sizes of 64 and 128 bits?
 *    - For 64-bit blocks:  R_64  = 0x000000000000001B (0x1B)
 *    - For 128-bit blocks: R_128 = 0x0000...0087       (0x87)
 * 
 * b. How the left shift and XOR accomplishes the desired result:
 *    The derivation implements multiplication by 'x' in the Galois Field GF(2^b)
 *    modulo the irreducible polynomial:
 *    - For 64-bit:  P(x) = x^64 + x^4 + x^3 + x + 1   (Constant 0x1B)
 *    - For 128-bit: P(x) = x^128 + x^7 + x^2 + x + 1  (Constant 0x87)
 *    Left-shifting multiplies the polynomial by x.
 *    If the MSB before shift was 1, multiplying by x yields a degree-b term
 *    that overflows the field. Reduction modulo P(x) is performed by XORing
 *    with R_b (the lower bits of the irreducible polynomial), bringing the
 *    result back into the field. If MSB was 0, no overflow occurs.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* 64-bit Subkey Generation */
void cmac_subkeys_64(uint64_t L, uint64_t *k1, uint64_t *k2) {
    const uint64_t R64 = 0x1B;

    /* K1 */
    if ((L & 0x8000000000000000ULL) == 0) {
        *k1 = L << 1;
    } else {
        *k1 = (L << 1) ^ R64;
    }

    /* K2 */
    if ((*k1 & 0x8000000000000000ULL) == 0) {
        *k2 = *k1 << 1;
    } else {
        *k2 = (*k1 << 1) ^ R64;
    }
}

/* 128-bit structure and helper functions */
typedef struct {
    uint64_t hi;
    uint64_t lo;
} uint128_t;

uint128_t shl_128(uint128_t val) {
    uint128_t res;
    res.hi = (val.hi << 1) | (val.lo >> 63);
    res.lo = val.lo << 1;
    return res;
}

void cmac_subkeys_128(uint128_t L, uint128_t *k1, uint128_t *k2) {
    const uint64_t R128 = 0x87;

    /* K1 */
    int msb_L = (L.hi >> 63) & 1;
    *k1 = shl_128(L);
    if (msb_L) {
        k1->lo ^= R128;
    }

    /* K2 */
    int msb_k1 = (k1->hi >> 63) & 1;
    *k2 = shl_128(*k1);
    if (msb_k1) {
        k2->lo ^= R128;
    }
}

int main() {
    printf("====================================================\n");
    printf("   EXPERIMENT 31: CMAC SUBKEY GENERATION            \n");
    printf("====================================================\n\n");

    /* Part (a): Constants */
    printf("--- PART (a): CONSTANTS NEEDED ---\n");
    printf("1. Block size = 64 bits:   R_64  = 0x1B (binary: 0001 1011)\n");
    printf("   Irreducible poly: x^64 + x^4 + x^3 + x + 1\n");
    printf("2. Block size = 128 bits:  R_128 = 0x87 (binary: 1000 0111)\n");
    printf("   Irreducible poly: x^128 + x^7 + x^2 + x + 1\n\n");

    /* Part (b): Demonstration for 64-bit block */
    printf("--- DEMO 1: 64-BIT SUBKEY GENERATION ---\n");
    uint64_t L64 = 0x8A123456789ABCDEULL; /* MSB = 1 to show XOR */
    uint64_t k1_64, k2_64;
    cmac_subkeys_64(L64, &k1_64, &k2_64);

    printf("L       = 0x%016llX (MSB = %d)\n", L64, (int)(L64 >> 63));
    printf("K1      = 0x%016llX\n", k1_64);
    printf("K2      = 0x%016llX\n\n", k2_64);

    /* Part (b): Demonstration for 128-bit block */
    printf("--- DEMO 2: 128-BIT SUBKEY GENERATION ---\n");
    uint128_t L128 = {0x9B3779B97F4A7C15ULL, 0xF39CC0605CED8011ULL};
    uint128_t k1_128, k2_128;
    cmac_subkeys_128(L128, &k1_128, &k2_128);

    printf("L       = %016llX %016llX (MSB = %d)\n", L128.hi, L128.lo, (int)(L128.hi >> 63));
    printf("K1      = %016llX %016llX\n", k1_128.hi, k1_128.lo);
    printf("K2      = %016llX %016llX\n\n", k2_128.hi, k2_128.lo);

    printf("----------------------------------------------------\n");
    printf("--- PART (b): EXPLANATION OF LEFT-SHIFT AND XOR ---\n");
    printf("1. The operation computes multiplication by 'x' in GF(2^b).\n");
    printf("2. Shifting left by 1 bit corresponds to multiplying by x.\n");
    printf("3. If the MSB before shift was 1, multiplying by x produces x^b,\n");
    printf("   overflowing the degree-(b-1) polynomial field.\n");
    printf("4. Reduction modulo the irreducible polynomial is achieved by\n");
    printf("   conditionally XORing with R_b (0x1B for 64-bit, 0x87 for 128-bit).\n");
    printf("----------------------------------------------------\n");

    return 0;
}
