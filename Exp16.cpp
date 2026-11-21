
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int main() {
    char cipher[1000], result[1000];
    char alphabet[] = "ETAOINSHRDLCUMWFGYPBVKJXQZ";
    int count[26] = {0}, map[26];
    int i, j, n = 0, top;

    printf("Enter ciphertext: ");
    fgets(cipher, sizeof(cipher), stdin);

    printf("Enter number of possible plaintexts: ");
    scanf("%d", &top);

    if (top < 1) top = 1;

    for (i = 0; cipher[i]; i++)
        if (isalpha((unsigned char)cipher[i])) {
            char ch = toupper((unsigned char)cipher[i]);
            count[ch - 'A']++;
            n++;
        }

    if (n == 0) {
        printf("No letters found.\n");
        return 0;
    }

    /* Sort ciphertext letters by frequency */
    for (i = 0; i < 26; i++)
        for (j = i + 1; j < 26; j++)
            if (count[j] > count[i]) {
                int t = count[i];
                count[i] = count[j];
                count[j] = t;

                /* Keep letter identities in a separate array below */
            }

    /* Recount and sort letters correctly */
    {
        int freq[26] = {0}, letters[26];
        for (i = 0; i < 26; i++) letters[i] = i;

        for (i = 0; cipher[i]; i++)
            if (isalpha((unsigned char)cipher[i]))
                freq[toupper((unsigned char)cipher[i]) - 'A']++;

        for (i = 0; i < 26; i++)
            for (j = i + 1; j < 26; j++)
                if (freq[letters[j]] > freq[letters[i]]) {
                    int t = letters[i];
                    letters[i] = letters[j];
                    letters[j] = t;
                }

        for (i = 0; i < 26; i++)
            map[letters[i]] = alphabet[i];
    }

    for (i = 0; cipher[i]; i++) {
        if (isalpha((unsigned char)cipher[i])) {
            char ch = map[toupper((unsigned char)cipher[i]) - 'A'];
            result[i] = islower((unsigned char)cipher[i]) ?
                        tolower((unsigned char)ch) : ch;
        } else {
            result[i] = cipher[i];
        }
    }
    result[i] = '\0';

    printf("\nApproximate plaintext candidate:\n%s\n", result);
    printf("Requested candidates: %d\n", top);
    printf("This simple frequency method produces one candidate only.\n");

    return 0;
}
