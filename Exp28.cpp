/*
 * Experiment 28: Flawed Diffie-Hellman Protocol (Linear Multiplicative Exchange)
 * 
 * Scenario:
 * Instead of Diffie-Hellman with exponentiation (a^x mod q),
 * participants send each other (x * a) mod q for public numbers a and prime q.
 * 
 * Questions & Answers:
 * 1. Method Alice and Bob use to agree on a key:
 *    Alice picks secret x_A, transmits Y_A = (x_A * a) mod q.
 *    Bob picks secret x_B, transmits Y_B = (x_B * a) mod q.
 *    Alice calculates Key = (Y_B * x_A) mod q = (x_B * a * x_A) mod q.
 *    Bob calculates Key = (Y_A * x_B) mod q = (x_A * a * x_B) mod q.
 *    Both share Key = (x_A * x_B * a) mod q.
 * 
 * 2. Can Eve find the secret numbers?
 *    YES! Since 'a' and 'q' are public, Eve calculates the modular inverse of a:
 *    a_inv = a^(-1) mod q.
 *    Then: x_A = (Y_A * a_inv) mod q, and x_B = (Y_B * a_inv) mod q.
 *    Secret numbers are trivially revealed in O(log q) time!
 * 
 * 3. Can Eve break the system without finding secret numbers?
 *    YES! Eve can directly compute the shared key:
 *    Key = (Y_A * Y_B * a_inv) mod q
 *    = ((x_A * a) * (x_B * a) * a^(-1)) mod q = (x_A * x_B * a) mod q!
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

int main() {
    long long q = 3571; /* Public prime modulus */
    long long a = 173;  /* Public multiplier */

    printf("====================================================\n");
    printf("   EXPERIMENT 28: FLAWED DIFFIE-HELLMAN PROTOCOL    \n");
    printf("====================================================\n");
    printf("Public Parameters: q = %lld (prime), a = %lld\n\n", q, a);

    /* 1. Alice and Bob select secret numbers */
    long long x_A = 1234; /* Alice's secret */
    long long x_B = 2345; /* Bob's secret */
    printf("1. Alice chooses secret x_A = %lld\n", x_A);
    printf("   Bob chooses secret   x_B = %lld\n\n", x_B);

    /* Alice and Bob transmit linear values */
    long long Y_A = (x_A * a) % q;
    long long Y_B = (x_B * a) % q;
    printf("2. Transmitted values across public channel:\n");
    printf("   Alice sends Y_A = (x_A * a) mod q = %lld\n", Y_A);
    printf("   Bob sends   Y_B = (x_B * a) mod q = %lld\n\n", Y_B);

    /* Alice and Bob compute shared key */
    long long K_alice = (Y_B * x_A) % q;
    long long K_bob   = (Y_A * x_B) % q;
    printf("3. Alice computes shared key: K = (Y_B * x_A) mod q = %lld\n", K_alice);
    printf("   Bob computes shared key:   K = (Y_A * x_B) mod q = %lld\n\n", K_bob);

    /* 4. Eve's Attacks */
    printf("--- EVE'S ATTACKS ---\n");
    long long a_inv = mod_inverse(a, q);
    printf("Eve computes modular inverse of public number a: a^(-1) mod q = %lld\n\n", a_inv);

    /* Eve finds secret numbers */
    long long eve_xA = (Y_A * a_inv) % q;
    long long eve_xB = (Y_B * a_inv) % q;
    printf("Attack 1: Eve solves for secret numbers:\n");
    printf("   Recovered x_A = (Y_A * a_inv) mod q = %lld (Actual: %lld)\n", eve_xA, x_A);
    printf("   Recovered x_B = (Y_B * a_inv) mod q = %lld (Actual: %lld)\n\n", eve_xB, x_B);

    /* Eve computes shared key directly without secrets */
    long long eve_Key = (Y_A * Y_B % q * a_inv) % q;
    printf("Attack 2: Eve computes shared key directly without secrets:\n");
    printf("   Eve's Key = (Y_A * Y_B * a_inv) mod q = %lld (Alice/Bob Key: %lld)\n\n", eve_Key, K_alice);

    printf("----------------------------------------------------\n");
    printf("CONCLUSION:\n");
    printf("- Linear multiplication is fatally broken because modular inverse\n");
    printf("  allows trivial recovery of secrets: x = Y * a^(-1) mod q.\n");
    printf("- Eve can both recover the secret numbers AND compute the key directly!\n");
    printf("- Diffie-Hellman relies crucially on modular EXPONENTIATION (discrete log),\n");
    printf("  which is computationally one-way.\n");
    printf("----------------------------------------------------\n");

    return 0;
}
