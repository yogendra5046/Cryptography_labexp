/*
 * Experiment 37: Automated Frequency Attack on Monoalphabetic Substitution Cipher
 * 
 * Strategy:
 * 1. Count letter frequencies in ciphertext.
 * 2. Compare against standard English letter frequency order:
 *    "ETAOINSHRDLCUMWFGYPBVKJXQZ"
 * 3. Generate candidate substitution keys by permuting neighboring frequency letters.
 * 4. Score each candidate plaintext using standard English letter frequency statistics.
 * 5. Rank and output the top N (e.g. top 10) most likely plaintexts.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* English letter frequencies in percentages (A-Z) */
static const double ENGLISH_FREQ[26] = {
    8.167, 1.492, 2.782, 4.253, 12.702, 2.228, 2.015, 6.094, 6.966,
    0.153, 0.772, 4.025, 2.406,  6.749, 7.507, 1.929, 0.095, 5.987,
    6.327, 9.056, 2.758, 0.978,  2.360, 0.150, 1.974, 0.074
};

static const char ENG_ORDER[] = "ETAOINSHRDLCUMWFGYPBVKJXQZ";

typedef struct {
    char letter;
    int count;
} FreqPair;

typedef struct {
    char key[26]; /* key[c - 'A'] = decrypted letter */
    char plaintext[256];
    double score; /* Lower chi-squared is better */
} Candidate;

int compare_freq(const void *a, const void *b) {
    return ((const FreqPair *)b)->count - ((const FreqPair *)a)->count;
}

int compare_candidates(const void *a, const void *b) {
    double diff = ((const Candidate *)a)->score - ((const Candidate *)b)->score;
    if (diff < 0) return -1;
    if (diff > 0) return 1;
    return 0;
}

/* Calculate Chi-Square goodness-of-fit for English */
double evaluate_fitness(const char *text) {
    int counts[26] = {0};
    int total = 0;

    for (int i = 0; text[i] != '\0'; i++) {
        if (isalpha(text[i])) {
            counts[toupper(text[i]) - 'A']++;
            total++;
        }
    }
    if (total == 0) return 1e9;

    double chi2 = 0.0;
    for (int i = 0; i < 26; i++) {
        double expected = (ENGLISH_FREQ[i] / 100.0) * total;
        double diff = counts[i] - expected;
        chi2 += (diff * diff) / (expected + 0.001);
    }
    return chi2;
}

void apply_key(const char *ct, const char key[26], char *pt) {
    for (int i = 0; ct[i] != '\0'; i++) {
        if (isalpha(ct[i])) {
            char base = isupper(ct[i]) ? 'A' : 'a';
            pt[i] = isupper(ct[i]) ? key[ct[i] - 'A'] : tolower(key[ct[i] - 'a']);
        } else {
            pt[i] = ct[i];
        }
    }
    pt[strlen(ct)] = '\0';
}

int main() {
    /* Sample encrypted ciphertext */
    const char *default_ct = 
        "WKH TXLFN EURZQ IRA MXPSV RYHU WKH ODCB GRJ DQG UXQV DZDB WR VDIHWB";
    char ciphertext[512];
    int top_n = 10;

    printf("====================================================\n");
    printf("   EXPERIMENT 37: MONOALPHABETIC FREQUENCY ATTACK   \n");
    printf("====================================================\n");
    printf("Default Ciphertext: \"%s\"\n\n", default_ct);

    printf("Enter number of top plaintexts to display (e.g. 10): ");
    if (scanf("%d", &top_n) != 1 || top_n <= 0) top_n = 10;
    strcpy(ciphertext, default_ct);

    /* Step 1: Count letter frequencies */
    FreqPair pairs[26];
    for (int i = 0; i < 26; i++) {
        pairs[i].letter = (char)('A' + i);
        pairs[i].count = 0;
    }

    for (int i = 0; ciphertext[i] != '\0'; i++) {
        if (isalpha(ciphertext[i])) {
            pairs[toupper(ciphertext[i]) - 'A'].count++;
        }
    }

    qsort(pairs, 26, sizeof(FreqPair), compare_freq);

    /* Step 2: Base key mapping sorted ciphertext letters to English frequency order */
    char base_key[26];
    for (int i = 0; i < 26; i++) {
        base_key[pairs[i].letter - 'A'] = ENG_ORDER[i];
    }

    /* Step 3: Generate candidates by perturbing high-frequency letter mappings */
    Candidate pool[100];
    int pool_size = 0;

    /* Base candidate */
    memcpy(pool[pool_size].key, base_key, 26);
    apply_key(ciphertext, base_key, pool[pool_size].plaintext);
    pool[pool_size].score = evaluate_fitness(pool[pool_size].plaintext);
    pool_size++;

    /* Generate perturbations */
    int swap_pairs[][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5},
        {0, 2}, {1, 3}, {2, 4}, {3, 5}, {4, 6},
        {5, 6}, {6, 7}, {7, 8}, {8, 9}, {0, 3}
    };
    int num_swaps = sizeof(swap_pairs) / sizeof(swap_pairs[0]);

    for (int s = 0; s < num_swaps && pool_size < 100; s++) {
        char cand_key[26];
        memcpy(cand_key, base_key, 26);

        /* Swap letters in candidate key */
        int idx1 = pairs[swap_pairs[s][0]].letter - 'A';
        int idx2 = pairs[swap_pairs[s][1]].letter - 'A';
        char tmp = cand_key[idx1];
        cand_key[idx1] = cand_key[idx2];
        cand_key[idx2] = tmp;

        memcpy(pool[pool_size].key, cand_key, 26);
        apply_key(ciphertext, cand_key, pool[pool_size].plaintext);
        pool[pool_size].score = evaluate_fitness(pool[pool_size].plaintext);
        pool_size++;
    }

    /* Also evaluate simple shifts (Caesar variants within substitution) */
    for (int shift = 1; shift < 26 && pool_size < 100; shift++) {
        char cand_key[26];
        for (int i = 0; i < 26; i++) {
            cand_key[i] = (char)('A' + (i - shift + 26) % 26);
        }
        memcpy(pool[pool_size].key, cand_key, 26);
        apply_key(ciphertext, cand_key, pool[pool_size].plaintext);
        pool[pool_size].score = evaluate_fitness(pool[pool_size].plaintext);
        pool_size++;
    }

    /* Step 4: Sort candidates by score */
    qsort(pool, pool_size, sizeof(Candidate), compare_candidates);

    /* Step 5: Output top N results */
    printf("\n--- TOP %d MOST LIKELY PLAINTEXTS ---\n", top_n);
    for (int i = 0; i < top_n && i < pool_size; i++) {
        printf("[Rank %2d | Chi-Sq Score: %8.2f]\n%s\n\n",
               i + 1, pool[i].score, pool[i].plaintext);
    }

    return 0;
}
