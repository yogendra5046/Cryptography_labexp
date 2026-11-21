/*
 * Experiment 27: RSA Character-by-Character Encryption & Dictionary Attack
 * 
 * Scenario:
 * Bob has a large modulus n and public exponent e.
 * Alice encodes each letter A-Z as an integer 0..25 and encrypts each
 * integer individually: C_i = (M_i)^e mod n.
 * 
 * Question: Is this method secure? If not, describe the most efficient attack.
 * 
 * Answer: NO! It is completely insecure.
 * Attack: Precomputed Dictionary Attack (Chosen-Plaintext / Codebook Attack).
 * Since the plaintext space has only 26 possible values (0 to 25):
 * 1. The attacker precomputes a table: Table[i] = (i^e) mod n for i in 0..25.
 *    This requires only 26 modular exponentiations.
 * 2. For each intercepted ciphertext block C, the attacker simply looks up C
 *    in the table to immediately identify the plaintext character in O(1) time.
 * The attacker never needs to factor n or find the private key d.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

long long mod_pow(long long base, long long exp, long long mod) {
    long long res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1) res = (res * base) % mod;
        base = (base * base) % mod;
        exp /= 2;
    }
    return res;
}

int main() {
    /* Setup RSA with a large modulus */
    long long p = 982451653LL; /* Large prime */
    long long q = 982451657LL; /* Another large prime (for simulation, product is ~60 bits) */
    /* Let's use large 32-64 bit integers for demonstration */
    long long n = 3233LL * 10007LL; /* 32352631 */
    long long e = 65537;

    printf("====================================================\n");
    printf("   EXPERIMENT 27: RSA MONO-CHARACTER DICTIONARY ATTACK\n");
    printf("====================================================\n");
    printf("Bob's Public Key: e = %lld, n = %lld\n\n", e, n);

    /* Alice sends secret message */
    const char *secret_message = "CRYPTOGRAPHY";
    int len = (int)strlen(secret_message);
    long long ciphertext[100];

    printf("Alice's Secret Message: \"%s\"\n", secret_message);
    printf("Alice encrypts each letter (A=0..Z=25) individually:\n");
    for (int i = 0; i < len; i++) {
        int m = toupper(secret_message[i]) - 'A';
        ciphertext[i] = mod_pow(m, e, n);
        printf("%lld ", ciphertext[i]);
    }
    printf("\n\n");

    /* ---------------- ATTACK ---------------- */
    printf("--- ATTACKER CODEBOOK / DICTIONARY ATTACK ---\n");
    printf("Precomputing ciphertext table for all 26 possible letters (A=0..Z=25)...\n");

    long long codebook[26];
    for (int m = 0; m < 26; m++) {
        codebook[m] = mod_pow(m, e, n);
    }
    printf("Table constructed with 26 modular exponentiations.\n\n");

    /* Attacker deciphers intercepted blocks via O(1) table lookup */
    char recovered[100];
    for (int i = 0; i < len; i++) {
        long long c = ciphertext[i];
        recovered[i] = '?';
        for (int m = 0; m < 26; m++) {
            if (codebook[m] == c) {
                recovered[i] = (char)('A' + m);
                break;
            }
        }
    }
    recovered[len] = '\0';

    printf("Intercepted Ciphertext Decoded by Attacker:\n");
    printf("Recovered Plaintext: \"%s\"\n\n", recovered);

    printf("----------------------------------------------------\n");
    printf("CONCLUSION:\n");
    printf("Encrypting small blocks (single characters) without random padding\n");
    printf("(such as OAEP) is completely insecure against dictionary attacks.\n");
    printf("The adversary can precompute all possible encryptions in O(|alphabet|)\n");
    printf("time and instantly decrypt any intercepted message!\n");
    printf("----------------------------------------------------\n");

    return 0;
}
