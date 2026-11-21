/*
 * Experiment 35: One-Time Pad (OTP) Version of Vigenère Cipher
 * 
 * Scheme:
 * Plaintext letters are encrypted by adding a stream of random shift keys (0..25):
 * C_i = (P_i + K_i) mod 26
 * Decryption subtracts the same key stream:
 * P_i = (C_i - K_i + 26) mod 26
 * 
 * Perfect Secrecy:
 * If each key value is chosen uniformly at random, is as long as the plaintext,
 * and is used only once (never reused), ciphertext reveals ZERO information
 * about the plaintext.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

void otp_encrypt(const char *pt, const int *key, char *ct, int len) {
    for (int i = 0; i < len; i++) {
        if (isalpha(pt[i])) {
            char base = isupper(pt[i]) ? 'A' : 'a';
            ct[i] = (char)((pt[i] - base + key[i]) % 26 + base);
        } else {
            ct[i] = pt[i];
        }
    }
    ct[len] = '\0';
}

void otp_decrypt(const char *ct, const int *key, char *pt, int len) {
    for (int i = 0; i < len; i++) {
        if (isalpha(ct[i])) {
            char base = isupper(ct[i]) ? 'A' : 'a';
            pt[i] = (char)((ct[i] - base - key[i] + 26) % 26 + base);
        } else {
            pt[i] = ct[i];
        }
    }
    pt[len] = '\0';
}

int main() {
    printf("====================================================\n");
    printf("   EXPERIMENT 35: ONE-TIME PAD (OTP) VIGENERE CIPHER\n");
    printf("====================================================\n\n");

    const char *plaintext = "HELLO ONE TIME PAD";
    int len = (int)strlen(plaintext);
    int key[100];
    char ciphertext[100];
    char decrypted[100];

    /* Sample random key stream as described in prompt (e.g., 3, 19, 5, ...) */
    int sample_key[] = {3, 19, 5, 12, 8, 21, 14, 2, 7, 25, 4, 16, 11, 1, 9, 22, 15, 6};

    for (int i = 0; i < len; i++) {
        if (i < (int)(sizeof(sample_key)/sizeof(sample_key[0]))) {
            key[i] = sample_key[i];
        } else {
            key[i] = rand() % 26;
        }
    }

    printf("Plaintext:  \"%s\"\n", plaintext);

    printf("Key Stream: ");
    for (int i = 0; i < len; i++) {
        printf("%d ", key[i]);
    }
    printf("\n\n");

    /* Encryption */
    otp_encrypt(plaintext, key, ciphertext, len);
    printf("Ciphertext: \"%s\"\n\n", ciphertext);

    /* Decryption */
    otp_decrypt(ciphertext, key, decrypted, len);
    printf("Decrypted:  \"%s\"\n\n", decrypted);

    /* Demonstration of Perfect Secrecy */
    printf("--- PERFECT SECRECY DEMONSTRATION ---\n");
    printf("Suppose an eavesdropper guesses an entirely different message of same length:\n");
    const char *alternate_msg = "ATTACK AT MIDNIGHT";
    int forged_key[100];
    for (int i = 0; i < len; i++) {
        if (isalpha(ciphertext[i]) && isalpha(alternate_msg[i])) {
            forged_key[i] = (toupper(ciphertext[i]) - toupper(alternate_msg[i]) + 26) % 26;
        } else {
            forged_key[i] = 0;
        }
    }
    printf("Alternate Message:  \"%s\"\n", alternate_msg);
    printf("Corresponding Key:  ");
    for (int i = 0; i < len; i++) printf("%d ", forged_key[i]);
    printf("\nEvery possible plaintext is equally likely without the exact key!\n");
    printf("----------------------------------------------------\n");

    return 0;
}
