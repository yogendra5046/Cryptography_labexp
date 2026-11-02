#include <stdio.h>

int main() {
    char text[1000];
    int k, i;

    printf("Enter the text: ");
    fgets(text, sizeof(text), stdin);

    printf("Enter the value of k (1-25): ");
    scanf("%d", &k);

    if (k < 1 || k > 25) {
        printf("Invalid value of k. Enter a value from 1 to 25.\n");
        return 1;
    }

    for (i = 0; text[i] != '\0'; i++) {
        if (text[i] >= 'A' && text[i] <= 'Z') {
            text[i] = (text[i] - 'A' + k) % 26 + 'A';
        }
        else if (text[i] >= 'a' && text[i] <= 'z') {
            text[i] = (text[i] - 'a' + k) % 26 + 'a';
        }
    }

    printf("Encrypted text: %s", text);

    return 0;
}