
#include <stdio.h>

int main() {
    int P[4] = {10, 20, 30, 40};
    int C[4], D[4], IV = 5;
    int i;

    /* Simplified CBC encryption simulation */
    C[0] = P[0] ^ IV;
    for (i = 1; i < 4; i++)
        C[i] = P[i] ^ C[i - 1];

    printf("Original CBC ciphertext: ");
    for (i = 0; i < 4; i++)
        printf("%d ", C[i]);

    /* Introduce a bit error in C1 */
    C[0] ^= 1;

    D[0] = C[0] ^ IV;
    for (i = 1; i < 4; i++)
        D[i] = C[i] ^ C[i - 1];

    printf("\nAfter error in C1, decrypted blocks: ");
    for (i = 0; i < 4; i++)
        printf("%d ", D[i]);

    printf("\n");
    return 0;
}
