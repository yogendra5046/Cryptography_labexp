#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main() {
    char s[100], key[100];
    int i, n;

    printf("Enter plaintext: ");
    scanf("%99s", s);
    printf("Enter key: ");
    scanf("%99s", key);

    n = strlen(key);

    for (i = 0; s[i]; i++)
        printf("%c", (toupper(s[i]) - 'A' +
               toupper(key[i % n]) - 'A') % 26 + 'A');

    return 0;
}