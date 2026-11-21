/*
 * Experiment 36: Affine Caesar Cipher & One-to-One Condition
 * 
 * Formula:
 * C = E([a, b], p) = (a * p + b) mod 26
 * 
 * Requirement:
 * The mapping must be ONE-TO-ONE (bijective).
 * This holds IF AND ONLY IF gcd(a, 26) == 1.
 * 
 * Collision Example:
 * For a = 2, b = 3 (gcd(2, 26) = 2 != 1):
 * E([2, 3], 0)  = (2 * 0 + 3) mod 26 = 3
 * E([2, 3], 13) = (2 * 13 + 3) mod 26 = 29 mod 26 = 3
 * Since p=0 ('A') and p=13 ('N') both map to 3 ('D'), unique decryption is impossible!
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int gcd(int a, int b) {
    while (b != 0) {
        int t = b;
        b = a % b;
        a = t;
    }
    return a;
}

int mod_inverse(int a, int m) {
    for (int x = 1; x < m; x++) {
        if ((a * x) % m == 1) return x;
    }
    return -1;
}

int affine_encrypt_char(int p, int a, int b) {
    return (a * p + b) % 26;
}

int affine_decrypt_char(int c, int a_inv, int b) {
    return (a_inv * (c - b + 26)) % 26;
}

int main() {
    printf("====================================================\n");
    printf("   EXPERIMENT 36: AFFINE CIPHER ONE-TO-ONE ANALYSIS \n");
    printf("====================================================\n\n");

    /* 1. Demonstrate collision for invalid 'a' */
    printf("--- PART 1: INVALID KEY COLLISION (a = 2, b = 3) ---\n");
    int bad_a = 2, bad_b = 3;
    printf("gcd(%d, 26) = %d != 1 (NOT coprime)\n", bad_a, gcd(bad_a, 26));

    int c_0  = affine_encrypt_char(0, bad_a, bad_b);
    int c_13 = affine_encrypt_char(13, bad_a, bad_b);
    printf("E([2, 3], 0)  ('A') = %d ('%c')\n", c_0, 'A' + c_0);
    printf("E([2, 3], 13) ('N') = %d ('%c')\n", c_13, 'A' + c_13);
    printf("COLLISION DETECTED: Both 'A' and 'N' encrypt to '%c'!\n", 'A' + c_0);
    printf("Decryption is impossible because inverse of %d mod 26 does not exist.\n\n", bad_a);

    /* 2. List all valid values of 'a' */
    printf("--- PART 2: ALL VALID ONE-TO-ONE VALUES OF 'a' ---\n");
    printf("Values of a in {1..25} with gcd(a, 26) == 1:\n");
    int count = 0;
    for (int a = 1; a < 26; a++) {
        if (gcd(a, 26) == 1) {
            printf("%2d (inverse = %2d)  ", a, mod_inverse(a, 26));
            count++;
            if (count % 4 == 0) printf("\n");
        }
    }
    printf("\nTotal valid values of a: %d = phi(26)\n\n", count);

    /* 3. Valid Affine Encryption & Decryption */
    printf("--- PART 3: VALID AFFINE CIPHER DEMO (a = 5, b = 8) ---\n");
    int valid_a = 5, valid_b = 8;
    int a_inv = mod_inverse(valid_a, 26);
    printf("Valid Key: a = %d, b = %d, a^(-1) mod 26 = %d\n", valid_a, valid_b, a_inv);

    const char *plaintext = "AFFINE CIPHER IS ONE TO ONE";
    char ciphertext[100];
    char decrypted[100];
    int len = (int)strlen(plaintext);

    for (int i = 0; i < len; i++) {
        if (isalpha(plaintext[i])) {
            int p = toupper(plaintext[i]) - 'A';
            ciphertext[i] = (char)('A' + affine_encrypt_char(p, valid_a, valid_b));
        } else {
            ciphertext[i] = plaintext[i];
        }
    }
    ciphertext[len] = '\0';

    for (int i = 0; i < len; i++) {
        if (isalpha(ciphertext[i])) {
            int c = ciphertext[i] - 'A';
            decrypted[i] = (char)('A' + affine_decrypt_char(c, a_inv, valid_b));
        } else {
            decrypted[i] = ciphertext[i];
        }
    }
    decrypted[len] = '\0';

    printf("Plaintext:  \"%s\"\n", plaintext);
    printf("Ciphertext: \"%s\"\n", ciphertext);
    printf("Decrypted:  \"%s\"\n", decrypted);

    return 0;
}
