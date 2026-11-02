
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main() {
    char p[] = "SENDMOREMONEY";
    char p2[] = "CASHNOTNEEDED";
    int key[] = {9, 0, 1, 7, 23, 15, 21, 14, 11, 11, 2, 8, 9};
    int c[13], i;

    printf("Ciphertext: ");
    for (i = 0; i < 13; i++) {
        c[i] = (p[i] - 'A' + key[i]) % 26;
        printf("%c", c[i] + 'A');
    }

    printf("\nNew key stream: ");
    for (i = 0; i < 13; i++) {
        int k = (c[i] - (p2[i] - 'A') + 26) % 26;
        printf("%d ", k);
    }

    printf("\n");
    return 0;
}
