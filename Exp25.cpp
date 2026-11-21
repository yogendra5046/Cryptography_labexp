/*
 * Experiment 25: RSA Plaintext Sharing a Factor with Modulus n
 * 
 * Scenario:
 * Bob's public key is (e, n) where n = p * q.
 * An attacker intercepts ciphertext C = M^e mod n.
 * The attacker is told that one of the plaintext blocks M shares a factor with n
 * (i.e., gcd(M, n) > 1 and 1 < M < n).
 * 
 * Mathematical Analysis:
 * Since n = p * q with p and q prime, if gcd(M, n) > 1, then gcd(M, n) must be p or q!
 * Furthermore, since C = M^e mod n, if p divides M, then p divides M^e, so p divides C!
 * Therefore, gcd(C, n) = p!
 * The attacker does NOT even need to know M: computing gcd(C, n) instantly factors n.
 * With p known, q = n / p, phi(n) = (p-1)*(q-1), and the private key d = e^(-1) mod phi(n)
 * is recovered, completely breaking the RSA system!
 */

#include <stdio.h>

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
    /* Setup RSA parameters */
    long long p_actual = 101;
    long long q_actual = 113;
    long long n = p_actual * q_actual; /* 11413 */
    long long phi = (p_actual - 1) * (q_actual - 1); /* 11200 */
    long long e = 65537 % phi; /* Ensure coprime */
    if (gcd(e, phi) != 1) e = 17;
    long long d_actual = mod_inverse(e, phi);

    printf("====================================================\n");
    printf("   EXPERIMENT 25: RSA FACTOR-SHARING VULNERABILITY  \n");
    printf("====================================================\n");
    printf("Bob's Public Parameters: n = %lld, e = %lld\n", n, e);
    printf("(Actual secret primes: p = %lld, q = %lld, d = %lld)\n\n", p_actual, q_actual, d_actual);

    /* Alice sends a message block M that shares a factor with n */
    /* For example, M is a multiple of p: M = 3 * p */
    long long M = 3 * p_actual; 
    long long C = mod_pow(M, e, n);

    printf("Alice encrypts plaintext block M = %lld (which shares factor with n)\n", M);
    printf("Intercepted Ciphertext C = %lld\n\n", C);

    /* Attacker Attack */
    printf("--- ATTACK EXECUTION ---\n");
    printf("Attacker knows that gcd(M, n) > 1.\n");
    printf("Because C = M^e mod n, p divides M implies p divides C.\n");
    printf("Attacker calculates gcd(C, n)...\n");

    long long p_recovered = gcd(C, n);
    printf("Result of gcd(C, n) = %lld\n", p_recovered);

    if (p_recovered > 1 && p_recovered < n) {
        long long q_recovered = n / p_recovered;
        long long phi_recovered = (p_recovered - 1) * (q_recovered - 1);
        long long d_recovered = mod_inverse(e, phi_recovered);

        printf("\nSUCCESS: Modulus completely factored!\n");
        printf("Recovered p = %lld\n", p_recovered);
        printf("Recovered q = %lld\n", q_recovered);
        printf("Computed phi(n) = %lld\n", phi_recovered);
        printf("Recovered Private Key d = %lld\n\n", d_recovered);

        /* Decrypt ciphertext using recovered d */
        long long M_decrypted = mod_pow(C, d_recovered, n);
        printf("Decrypted Message: %lld (Original was %lld)\n", M_decrypted, M);
        printf("\nCONCLUSION: Yes! Knowing that a plaintext block shares a factor\n");
        printf("with n allows an attacker to instantly factor n via gcd(C, n)\n");
        printf("and derive the entire private key, completely breaking RSA!\n");
    } else {
        printf("Attack failed.\n");
    }

    return 0;
}
