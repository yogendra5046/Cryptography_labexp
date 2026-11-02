
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main() {
    char msg[] = "meet me at the usual place at ten rather than eight oclock";
    char p[100], c[100], d[100];
    int i, n = 0, x, y;

    int k[2][2] = {{9, 4}, {5, 7}};
    int inv[2][2] = {{5, 12}, {15, 25}};

    for (i = 0; msg[i]; i++)
        if (isalpha((unsigned char)msg[i]))
            p[n++] = toupper((unsigned char)msg[i]);

    if (n % 2)
        p[n++] = 'X';
    p[n] = '\0';

    printf("Plaintext: %s\n", p);

    for (i = 0; i < n; i += 2) {
        x = p[i] - 'A';
        y = p[i + 1] - 'A';

        c[i]     = (9 * x + 4 * y) % 26 + 'A';
        c[i + 1] = (5 * x + 7 * y) % 26 + 'A';
    }
    c[n] = '\0';

    printf("Ciphertext: %s\n", c);

    for (i = 0; i < n; i += 2) {
        x = c[i] - 'A';
        y = c[i + 1] - 'A';

        d[i]     = (5 * x + 12 * y) % 26 + 'A';
        d[i + 1] = (15 * x + 25 * y) % 26 + 'A';
    }
    d[n] = '\0';

    printf("Decrypted text: %s\n", d);

    return 0;
}
