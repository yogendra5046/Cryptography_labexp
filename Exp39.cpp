#include <stdio.h>
#include <string.h>

int main()
{
    char cipher[200];
    int key, i;

    printf("Enter ciphertext: ");
    scanf("%s", cipher);

    printf("\nPossible plaintexts:\n");

    for(key=0; key<26; key++)
    {
        printf("Key %2d: ", key);

        for(i=0; cipher[i]!='\0'; i++)
        {
            if(cipher[i]>='A' && cipher[i]<='Z')
                printf("%c",((cipher[i]-'A'-key+26)%26)+'A');
            else
                printf("%c",cipher[i]);
        }
        printf("\n");
    }

    return 0;
}