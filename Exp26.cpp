/*
 * Experiment 26: Reusing Modulus n after Private Key Leakage
 * 
 * Question: Suppose Bob leaks his private key d. Rather than generating a new
 * modulus n, he decides to generate a new public key e_new and private key d_new
 * for the SAME modulus n. Is this safe?
 * 
 * Answer: NO! It is completely unsafe.
 * When the old private key d_old and public key e_old are known, anyone can factor
 * the modulus n = p * q. Specifically, e_old * d_old - 1 is a multiple of phi(n).
 * Once n is factored, the attacker knows phi(n).
 * When Bob chooses a new public key e_new, the attacker can trivially compute:
 * d_new = (e_new)^(-1) mod phi(n)
 * Thus, Bob's new private key is completely compromised.
 */

#include <stdio.h>
#include <math.h>

long long gcd(long long a, long long b) {
    while (b != 0) {
        long long t = b;
        b = a % b;
        a = t;
    }
    return a;
}

long long extended_gcd(long long a, long long b, long long *x, long long *y) {
    if (b == 0) {
        *x = 1; *y = 0;
        return a;
    }
    long long x1, y1;
    long long g = extended_gcd(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (a / b) * y1;
    return g;
}

long long mod_inverse(long long e, long long m) {
    long long x, y;
    long long g = extended_gcd(e, m, &x, &y);
    if (g != 1) return -1;
    return (x % m + m) % m;
}

/* Factor n given n and multiple k = e*d - 1 of phi(n) */
int factor_n_from_ed(long long n, long long e, long long d, long long *p_out, long long *q_out) {
    long long k = e * d - 1;
    /* Since k = m * phi(n), and phi(n) < n, m is approximately e */
    for (long long m = 1; m <= e; m++) {
        if (k % m == 0) {
            long long cand_phi = k / m;
            long long s = n - cand_phi + 1; /* s = p + q */
            long long disc = s * s - 4 * n;
            if (disc >= 0) {
                long long root_disc = (long long)round(sqrt((double)disc));
                if (root_disc * root_disc == disc) {
                    long long p = (s + root_disc) / 2;
                    long long q = (s - root_disc) / 2;
                    if (p > 1 && q > 1 && p * q == n) {
                        *p_out = p;
                        *q_out = q;
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

int main() {
    long long p_orig = 61;
    long long q_orig = 53;
    long long n = p_orig * q_orig; /* 3233 */
    long long phi = (p_orig - 1) * (q_orig - 1); /* 3120 */

    /* Initial keys */
    long long e1 = 17;
    long long d1 = mod_inverse(e1, phi); /* 2753 */

    printf("====================================================\n");
    printf("   EXPERIMENT 26: RSA REUSING MODULUS AFTER KEY LEAK\n");
    printf("====================================================\n");
    printf("Bob's Modulus: n = %lld\n", n);
    printf("Old Public Key: e1 = %lld, Leaked Private Key: d1 = %lld\n\n", e1, d1);

    /* Bob leaks d1. Attacker factors n */
    printf("Step 1: Attacker uses (e1, d1) to factor modulus n...\n");
    long long p_recovered = 0, q_recovered = 0;
    if (factor_n_from_ed(n, e1, d1, &p_recovered, &q_recovered)) {
        printf("SUCCESS: Modulus factored! p = %lld, q = %lld\n", p_recovered, q_recovered);
        long long phi_recovered = (p_recovered - 1) * (q_recovered - 1);
        printf("Attacker calculates phi(n) = %lld\n\n", phi_recovered);

        /* Step 2: Bob generates a new key pair for the SAME n */
        long long e2 = 37; /* Bob's new public key */
        long long d2_bob = mod_inverse(e2, phi);
        printf("Step 2: Bob retains n = %lld and chooses:\n", n);
        printf("New Public Key e2 = %lld\n", e2);
        printf("Bob thinks his new private key d2 is secret.\n\n");

        /* Step 3: Attacker easily derives Bob's new private key */
        long long d2_attacker = mod_inverse(e2, phi_recovered);
        printf("Step 3: Attacker calculates d2 = e2^(-1) mod phi(n):\n");
        printf("Attacker's recovered d2 = %lld (Bob's actual d2 = %lld)\n\n", d2_attacker, d2_bob);

        printf("----------------------------------------------------\n");
        printf("CONCLUSION: It is NOT SAFE to reuse modulus n!\n");
        printf("Once a private key is leaked, the modulus n can be factored,\n");
        printf("rendering any new key pair on the same modulus completely insecure.\n");
        printf("Bob MUST generate a new modulus n (new p and q)!\n");
        printf("----------------------------------------------------\n");
    } else {
        printf("Factorization failed.\n");
    }

    return 0;
}
