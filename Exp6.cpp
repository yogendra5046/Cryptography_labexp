#include <stdio.h>
#include <string.h>

int main() {
    char s[1000];
    int a, b, i, p;

    scanf("%s", s);

    for (a = 1; a < 26; a++) {
        for (b = 0; b < 26; b++) {
            if ((4*a+b)%26 == 1 && (19*a+b)%26 == 20) {
                printf("a=%d b=%d\n", a, b);

                for (i = 0; s[i]; i++) {
                    p = 0;
                    while ((a*p+b)%26 != s[i]-'A')
                        p++;
                    printf("%c", p+'A');
                }
                printf("\n");
            }
        }
    }
    return 0;
}