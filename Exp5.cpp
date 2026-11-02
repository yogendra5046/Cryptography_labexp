
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int gcd(int a, int b) {
    return b == 0 ? a : gcd(b, a % b);
}

int main() {
    char s[100];
    int a, b, i;

    printf("Enter plaintext: ");
    scanf("%99s", s);
    printf("Enter a and b: ");
    scanf("%d%d", &a, &b);

    if (gcd(a, 26) != 1) {
        printf("Invalid a value");
        return 0;
    }

    for (i = 0; s[i]; i++)
        printf("%c", (a * (toupper(s[i]) - 'A') + b) % 26 + 'A');

    return 0;
}
