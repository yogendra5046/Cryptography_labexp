/*
 * Experiment 32: DSA vs. RSA Signature Randomness & k-Reuse Catastrophe
 * 
 * RSA Signatures:
 * S = M^d mod n (Deterministic)
 * - Signing the same message M twice always produces the identical signature.
 * - Implication: Determinism allows observers to detect duplicate messages.
 * 
 * DSA Signatures:
 * r = (g^k mod p) mod q
 * s = k^(-1) * (H(M) + x * r) mod q
 * - Since k is freshly chosen for every signature, signing the same message M
 *   produces distinct (r, s) pairs every time (probabilistic/randomized).
 * - Implication 1: Hides message patterns and prevents replay analysis.
 * - Implication 2 (FATAL FLAW): If k is ever reused for two signatures,
 *   the attacker can immediately calculate k and extract the private key x!
 */

#include <stdio.h>

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

long long mod_inverse(long long a, long long m) {
    long long x, y;
    long long g = extended_gcd(a, m, &x, &y);
    if (g != 1) return -1;
    return (x % m + m) % m;
}

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
    printf("====================================================\n");
    printf("   EXPERIMENT 32: DSA VS RSA SIGNATURE IMPLICATIONS \n");
    printf("====================================================\n\n");

    /* Part 1: RSA Signature Determinism */
    printf("--- PART 1: RSA DETERMINISTIC SIGNATURES ---\n");
    long long rsa_n = 3233, rsa_d = 2753; /* RSA private key */
    long long msg = 42;

    long long rsa_sig1 = mod_pow(msg, rsa_d, rsa_n);
    long long rsa_sig2 = mod_pow(msg, rsa_d, rsa_n);

    printf("Message: %lld\n", msg);
    printf("RSA Signature 1: %lld\n", rsa_sig1);
    printf("RSA Signature 2: %lld\n", rsa_sig2);
    printf("Observation: RSA signatures are identical (Deterministic).\n\n");

    /* Part 2: DSA Randomized Signatures */
    printf("--- PART 2: DSA RANDOMIZED SIGNATURES ---\n");
    /* DSA parameters */
    long long p = 1019; /* Prime modulus */
    long long q = 101;  /* Prime divisor of p-1 (1019 - 1 = 1018, let's use valid subgroup) */
    /* Let's use simple valid parameters for demo */
    p = 7879; q = 101; /* 7879 - 1 = 78 * 101 */
    long long h = 3;
    long long g = mod_pow(h, (p - 1) / q, p); /* Generator */
    long long x = 45; /* Private key */
    long long y = mod_pow(g, x, p); /* Public key */

    long long Hm = 37; /* Hash of message */

    /* Signature 1 with random k1 */
    long long k1 = 23;
    long long r1 = mod_pow(g, k1, p) % q;
    long long k1_inv = mod_inverse(k1, q);
    long long s1 = (k1_inv * (Hm + x * r1)) % q;

    /* Signature 2 with different random k2 */
    long long k2 = 67;
    long long r2 = mod_pow(g, k2, p) % q;
    long long k2_inv = mod_inverse(k2, q);
    long long s2 = (k2_inv * (Hm + x * r2)) % q;

    printf("Same Message Hash H(M): %lld\n", Hm);
    printf("DSA Signature 1 (k = %lld): (r = %lld, s = %lld)\n", k1, r1, s1);
    printf("DSA Signature 2 (k = %lld): (r = %lld, s = %lld)\n", k2, r2, s2);
    printf("Observation: DSA signatures differ each time (Probabilistic).\n\n");

    /* Part 3: CRITICAL IMPLICATION - DSA k-Reuse Attack */
    printf("--- PART 3: CRITICAL IMPLICATION (DSA k-REUSE ATTACK) ---\n");
    printf("Signer mistakenly reuses k = %lld to sign two different messages M1, M2:\n", k1);
    long long Hm1 = 37;
    long long Hm2 = 85;

    long long r_reuse = mod_pow(g, k1, p) % q;
    long long s_reuse1 = (k1_inv * (Hm1 + x * r_reuse)) % q;
    long long s_reuse2 = (k1_inv * (Hm2 + x * r_reuse)) % q;

    printf("Message 1 Hash: %lld -> Sig1: (r = %lld, s = %lld)\n", Hm1, r_reuse, s_reuse1);
    printf("Message 2 Hash: %lld -> Sig2: (r = %lld, s = %lld)\n", Hm2, r_reuse, s_reuse2);
    printf("Notice that both signatures have identical 'r' value!\n\n");

    /* Attacker recovers k and private key x */
    printf("Attacker calculates recovered nonce k:\n");
    long long num = (Hm1 - Hm2 + q) % q;
    long long den = (s_reuse1 - s_reuse2 + q) % q;
    long long den_inv = mod_inverse(den, q);
    long long k_recovered = (num * den_inv) % q;
    printf("   Recovered k = (H1 - H2) / (s1 - s2) mod q = %lld (Actual k = %lld)\n", k_recovered, k1);

    long long r_inv = mod_inverse(r_reuse, q);
    long long x_recovered = (((s_reuse1 * k_recovered - Hm1) % q + q) % q * r_inv) % q;
    printf("Attacker recovers Private Key x:\n");
    printf("   Recovered x = (s1*k - H1)*r^(-1) mod q = %lld (Actual x = %lld)\n\n", x_recovered, x);

    printf("----------------------------------------------------\n");
    printf("CONCLUSION:\n");
    printf("The randomness of k in DSA provides semantic security and prevents\n");
    printf("signature replay/pattern analysis. However, it introduces a severe\n");
    printf("vulnerability: reusing k completely destroys security and immediately\n");
    printf("reveals the private key x!\n");
    printf("----------------------------------------------------\n");

    return 0;
}
