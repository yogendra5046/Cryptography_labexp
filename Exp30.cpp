/*
 * Experiment 30: CBC-MAC Extension Attack / Forgery
 * 
 * Theorem:
 * For a one-block message X, let tag T = CBC_MAC(K, X) = E_K(X).
 * An adversary who observes (X, T) can forge a valid CBC-MAC for the
 * two-block message M = X || (X ⊕ T) without knowing the secret key K.
 * 
 * Proof:
 * For CBC-MAC with IV = 0:
 * Block 1: C1 = E_K(X ⊕ 0) = E_K(X) = T.
 * Block 2: C2 = E_K((X ⊕ T) ⊕ C1)
 *             = E_K((X ⊕ T) ⊕ T)
 *             = E_K(X ⊕ (T ⊕ T))
 *             = E_K(X ⊕ 0)
 *             = E_K(X) = T!
 * Thus, CBC_MAC(K, X || (X ⊕ T)) is once again T!
 */

#include <stdio.h>
#include <stdint.h>

#define BLOCK_SIZE 8

/* Simple 64-bit Feistel block cipher for testing */
uint64_t block_encrypt(uint64_t block, uint64_t key) {
    uint32_t L = (uint32_t)(block >> 32);
    uint32_t R = (uint32_t)(block & 0xFFFFFFFFULL);
    for (int r = 0; r < 8; r++) {
        uint32_t round_key = (uint32_t)((key >> (r * 4)) ^ 0x9E3779B9);
        uint32_t temp = R;
        R = L ^ (((R << 5) ^ (R >> 2)) + round_key);
        L = temp;
    }
    return (((uint64_t)L) << 32) | R;
}

/* CBC-MAC computation for n blocks (IV = 0) */
uint64_t compute_cbc_mac(const uint64_t *blocks, int n_blocks, uint64_t key) {
    uint64_t state = 0; /* IV = 0 */
    for (int i = 0; i < n_blocks; i++) {
        state = block_encrypt(blocks[i] ^ state, key);
    }
    return state;
}

int main() {
    /* Secret key known only to the sender/verifier */
    uint64_t secret_key = 0x0123456789ABCDEFULL;

    /* Original 1-block message X */
    uint64_t X = 0xCAFEBABE11223344ULL;

    printf("====================================================\n");
    printf("   EXPERIMENT 30: CBC-MAC ONE-BLOCK FORGERY ATTACK  \n");
    printf("====================================================\n");
    printf("Secret Key: 0x%016llX (Unknown to adversary)\n", secret_key);
    printf("Original 1-block Message X: 0x%016llX\n\n", X);

    /* 1. Legitimate MAC calculation for 1-block message X */
    uint64_t T = compute_cbc_mac(&X, 1, secret_key);
    printf("Step 1: Legitimate Tag T = MAC(K, X):\n");
    printf("   Tag T: 0x%016llX\n\n", T);

    /* 2. Adversary constructs forged 2-block message */
    uint64_t forged_msg[2];
    forged_msg[0] = X;
    forged_msg[1] = X ^ T;

    printf("Step 2: Adversary creates 2-block message X || (X ^ T):\n");
    printf("   Block 1 = X       = 0x%016llX\n", forged_msg[0]);
    printf("   Block 2 = X ^ T   = 0x%016llX\n\n", forged_msg[1]);

    /* 3. Verifier checks MAC of the 2-block message */
    uint64_t T_forged = compute_cbc_mac(forged_msg, 2, secret_key);
    printf("Step 3: Verification of MAC(K, X || (X ^ T)):\n");
    printf("   Computed Tag: 0x%016llX\n", T_forged);
    printf("   Adversary predicted tag: 0x%016llX (T)\n\n", T);

    if (T_forged == T) {
        printf("SUCCESSFUL FORGERY: The MAC matches exactly T!\n");
        printf("The adversary successfully forged a valid message-tag pair\n");
        printf("for a 2-block message WITHOUT knowing the secret key.\n\n");
        printf("----------------------------------------------------\n");
        printf("LESSON: Basic CBC-MAC is insecure for messages of variable\n");
        printf("length without length-prepending or output transformation (CMAC)!\n");
        printf("----------------------------------------------------\n");
    } else {
        printf("Attack failed.\n");
    }

    return 0;
}
