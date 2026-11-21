/*
 * Experiment 29: SHA-3 State Propagation & Nonzero Lane Diffusion
 * 
 * Scenario:
 * - SHA-3 option with block size (rate r) = 1024 bits.
 * - State size b = 1600 bits (organized as 5x5 array of 64-bit lanes, 25 lanes total).
 * - Rate r = 1024 bits = 16 lanes.
 * - Capacity c = 576 bits = 9 lanes.
 * - First message block P0 has at least one nonzero bit in each of its 16 lanes.
 * - Initial capacity lanes (9 lanes) are all zeros.
 * 
 * Question:
 * How long will it take before all of these lanes have at least one nonzero bit?
 * (Ignore permutation - track original zero lanes).
 * 
 * Analysis & Result:
 * In the very first round of Keccak-f[1600]:
 * Step Theta (θ): Computes column parities C[x] = A[x,0] ^ ... ^ A[x,4].
 * Because every column contains nonzero rate lanes, every column parity C[x] is nonzero.
 * The diffusion value D[x] = C[x-1] ^ rot(C[x+1], 1) is nonzero for all x.
 * Each lane is updated by: A'[x, y] = A[x, y] ^ D[x].
 * For each capacity lane originally with all zeros: A'[x, y] = 0 ^ D[x] = D[x] != 0!
 * Thus, all capacity lanes become NONZERO during the Theta step of Round 1!
 * Total time: Exactly 1 round (in fact, in step Theta of Round 1).
 */

#include <stdio.h>
#include <stdint.h>

#define ROTL64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))

typedef struct {
    uint64_t lane[5][5];
} KeccakState;

/* Count how many lanes have at least one nonzero bit */
int count_nonzero_lanes(const KeccakState *state) {
    int count = 0;
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++) {
            if (state->lane[x][y] != 0) count++;
        }
    }
    return count;
}

void print_state_matrix(const char *stage_name, const KeccakState *state) {
    printf("--- %s ---\n", stage_name);
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++) {
            if (state->lane[x][y] == 0) {
                printf("[ ZERO ] ");
            } else {
                printf("[NONZER] ");
            }
        }
        printf("\n");
    }
    printf("Total Nonzero Lanes: %d / 25\n\n", count_nonzero_lanes(state));
}

/* Keccak-f Theta step */
void keccak_theta(KeccakState *state) {
    uint64_t C[5];
    uint64_t D[5];

    for (int x = 0; x < 5; x++) {
        C[x] = state->lane[x][0] ^ state->lane[x][1] ^ state->lane[x][2] ^ state->lane[x][3] ^ state->lane[x][4];
    }

    for (int x = 0; x < 5; x++) {
        D[x] = C[(x + 4) % 5] ^ ROTL64(C[(x + 1) % 5], 1);
    }

    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++) {
            state->lane[x][y] ^= D[x];
        }
    }
}

int main() {
    KeccakState state;

    printf("====================================================\n");
    printf("   EXPERIMENT 29: SHA-3 NONZERO LANE PROPAGATION    \n");
    printf("====================================================\n");
    printf("Parameters:\n");
    printf("- State size:    1600 bits (5x5 matrix of 64-bit lanes)\n");
    printf("- Rate (r):      1024 bits = 16 lanes (Lanes with nonzero bits)\n");
    printf("- Capacity (c):  576 bits  = 9 lanes  (Initially all zero)\n\n");

    /* Initialize state: first 16 lanes have nonzero bits, last 9 are zero */
    int lane_idx = 0;
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++) {
            if (lane_idx < 16) {
                state.lane[x][y] = (uint64_t)(0x1234567800000000ULL | (lane_idx + 1));
            } else {
                state.lane[x][y] = 0; /* Capacity lanes */
            }
            lane_idx++;
        }
    }

    print_state_matrix("Initial State (After XORing P0 into Rate)", &state);

    /* Step Theta of Round 1 */
    printf("Applying Round 1 Step Theta (Parity Diffusion)...\n");
    keccak_theta(&state);
    print_state_matrix("State After Step Theta of Round 1", &state);

    printf("----------------------------------------------------\n");
    printf("CONCLUSION:\n");
    printf("How long does it take before all lanes have at least one nonzero bit?\n");
    printf("Answer: Exactly 1 round (specifically during the Theta step of Round 1)!\n");
    printf("Because each column's parity C[x] is nonzero, D[x] is nonzero across all\n");
    printf("5 columns, causing every zero lane A[x,y] to become 0 ^ D[x] != 0.\n");
    printf("----------------------------------------------------\n");

    return 0;
}
