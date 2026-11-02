#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main() {
    char key[] = "CIPHER";
    char map[26], s[100], c;
    int i, j, n = 0;

    for (i = 0; key[i]; i++)
        map[n++] = key[i];

    for (c = 'A'; c <= 'Z'; c++) {
        for (j = 0; j < n && map[j] != c; j++);
        if (j == n) map[n++] = c;
    }

    printf("Enter plaintext: ");
    scanf("%99s", s);

    for (i = 0; s[i]; i++) {
        c = toupper(s[i]);
        if (c >= 'A' && c <= 'Z')
            printf("%c", map[c - 'A']);
        else
            printf("%c", s[i]);
    }

    return 0;
}