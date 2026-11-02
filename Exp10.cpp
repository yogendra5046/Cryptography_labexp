#include <stdio.h>
#include <string.h>
#include <ctype.h>

char m[5][5] = {
    {'M','F','H','I','K'},
    {'U','N','O','P','Q'},
    {'Z','V','W','X','Y'},
    {'E','L','A','R','G'},
    {'D','S','T','B','C'}
};

void encrypt(char a, char b) {
    int r1, c1, r2, c2, i, j;

    if (a == 'J') a = 'I';
    if (b == 'J') b = 'I';

    for (i = 0; i < 5; i++)
        for (j = 0; j < 5; j++) {
            if (m[i][j] == a) r1 = i, c1 = j;
            if (m[i][j] == b) r2 = i, c2 = j;
        }

    if (r1 == r2)
        printf("%c%c", m[r1][(c1+1)%5],
                       m[r2][(c2+1)%5]);
    else if (c1 == c2)
        printf("%c%c", m[(r1+1)%5][c1],
                       m[(r2+1)%5][c2]);
    else
        printf("%c%c", m[r1][c2], m[r2][c1]);
}

int main() {
    char msg[] = "Must see you over Cadogan West Coming at once";
    char s[200];
    int i, n = 0;

    for (i = 0; msg[i]; i++)
        if (isalpha((unsigned char)msg[i]))
            s[n++] = toupper((unsigned char)msg[i]);

    s[n] = '\0';

    for (i = 0; i < n; i += 2) {
        char a = s[i];
        char b = (i+1 < n) ? s[i+1] : 'X';

        if (a == b) {
            encrypt(a, 'X');
            i--;
        } else {
            encrypt(a, b);
        }
    }

    printf("\n");
    return 0;
}