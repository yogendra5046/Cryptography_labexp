/*
 * Experiment 24: RSA Private Key Calculation
 * 
 * Public Key: e = 31, n = 3599
 * Step 1: Trial-and-error factorization of n = p * q
 * Step 2: Calculate phi(n) = (p - 1) * (q - 1)
 * Step 3: Extended Euclidean Algorithm to find private key d = e^(-1) mod phi(n)
 * Step 4: Verification via Encryption & Decryption
 */

#include <stdio.h>
#include <math.h>

/* Extended Euclidean Algorithm: returns gcd and finds x, y such that a*x + b*y = gcd */
long long extended_gcd(long long a, long long b, long long *x, long long *y) {
    if (b == 0) {
        *x = 1;
        *y = 0;
        return a;
    }
    long long x1, y1;
    long long gcd = extended_gcd(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (a / b) * y1;
    return gcd;
}

/* Multiplicative inverse of e modulo m */
long long mod_inverse(long long e, long long m) {
    long long x, y;
    long long g = extended_gcd(e, m, &x, &y);
    if (g != 1) return -1; /* Inverse doesn't exist */
    return (x % m + m) % m;
}

/* Modular exponentiation: (base^exp) % mod */
long long mod_pow(long long base, long long exp, long long mod) {
    long long res = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp % 2 == 1) res = (res * base) % mod;
        base = (base * base) % mod;
        exp /= 2;
    }
    return res;
}

int main() {
    long long e = 31;
    long long n = 3599;
    long long p = 0, q = 0;

    printf("====================================================\n");
    printf("   EXPERIMENT 24: RSA PRIVATE KEY CALCULATION       \n");
    printf("====================================================\n");
    printf("Given Public Key: e = %lld, n = %lld\n\n", e, n);

    /* Step 1: Factor n using trial division */
    printf("Step 1: Factoring n = %lld via trial division...\n", n);
    long long limit = (long long)sqrt((double)n);
    for (long long i = 2; i <= limit + 1; i++) {
        if (n % i == 0) {
            p = i;
            q = n / i;
            break;
        }
    }

    if (p == 0) {
        printf("Error: Could not factor n.\n");
        return 1;
    }

    printf("Found Prime factors: p = %lld, q = %lld\n", p, q);
    printf("Verification: %lld * %lld = %lld\n\n", p, q, p * q);

    /* Step 2: Compute phi(n) */
    long long phi = (p - 1) * (q - 1);
    printf("Step 2: Euler's totient phi(n) = (p-1)*(q-1) = %lld * %lld = %lld\n\n", p - 1, q - 1, phi);

    /* Step 3: Find d such that (d * e) % phi == 1 */
    long long d = mod_inverse(e, phi);
    printf("Step 3: Multiplicative inverse of e = %lld mod %lld:\n", e, phi);
    printf("Private key d = %lld\n", d);
    printf("Verification: (%lld * %lld) %% %lld = %lld\n\n", e, d, phi, (e * d) % phi);

    /* Step 4: Verification through encryption and decryption */
    long long msg = 1234;
    long long ciphertext = mod_pow(msg, e, n);
    long long decrypted  = mod_pow(ciphertext, d, n);

    printf("Step 4: Encryption and Decryption Demo:\n");
    printf("Original Message:  %lld\n", msg);
    printf("Ciphertext:        C = (%lld^%lld) mod %lld = %lld\n", msg, e, n, ciphertext);
    printf("Decrypted Message: M = (%lld^%lld) mod %lld = %lld\n", ciphertext, d, n, decrypted);

    if (decrypted == msg) {
        printf("\nSUCCESS: Decrypted message matches original message!\n");
    }

    return 0;
}
