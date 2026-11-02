
#include <stdio.h>
#include <string.h>
#include <ctype.h>

double freq[26] = {
    8.2, 1.5, 2.8, 4.3, 12.7, 2.2, 2.0, 6.1, 7.0,
    0.15, 0.77, 4.0, 2.4, 6.7, 7.5, 1.9, 0.095, 6.0,
    6.3, 9.1, 2.8, 0.98, 2.4, 0.15, 2.0, 0.074
};

int main() {
    char cipher[500], plain[500][26];
    double score[26];
    int shift, i, j, top, temp;
    double ts;

    printf("Enter ciphertext: ");
    fgets(cipher, sizeof(cipher), stdin);

    printf("How many possible plaintexts? ");
    scanf("%d", &top);
    if (top < 1) top = 1;
    if (top > 26) top = 26;

    for (shift = 0; shift < 26; shift++) {
        int count[26] = {0}, n = 0;
        strcpy(plain[shift], cipher);

        for (i = 0; cipher[i]; i++) {
            if (isalpha((unsigned char)cipher[i])) {
                char ch = toupper((unsigned char)cipher[i]);
                plain[shift][i] =
                    (ch - 'A' - shift + 26) % 26 + 'A';
                count[plain[shift][i] - 'A']++;
                n++;
            }
        }

        score[shift] = 0;
        for (j = 0; j < 26; j++)
            score[shift] += count[j] * freq[j];

        if (n > 0)
            score[shift] /= n;
    }

    /* Sort shifts by descending frequency score */
    for (i = 0; i < 26; i++)
        for (j = i + 1; j < 26; j++)
            if (score[j] > score[i]) {
                ts = score[i];
                score[i] = score[j];
                score[j] = ts;

                for (temp = 0; temp < (int)strlen(cipher); temp++) {
                    char ch = plain[i][temp];
                    plain[i][temp] = plain[j][temp];
                    plain[j][temp] = ch;
                }
            }

    printf("\nTop %d possible plaintexts:\n", top);
    for (i = 0; i < top; i++)
        printf("%d. %s\n", i + 1, plain[i]);

    return 0;
}
