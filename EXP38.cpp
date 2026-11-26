#include <stdio.h>

int main()
{
    int P[3][3], C[3][3], K[3][3];
    int i, j, k;

    printf("Enter 9 plaintext numbers (0-25):\n");
    for(i=0;i<3;i++)
        for(j=0;j<3;j++)
            scanf("%d",&P[i][j]);

    printf("Enter 9 ciphertext numbers (0-25):\n");
    for(i=0;i<3;i++)
        for(j=0;j<3;j++)
            scanf("%d",&C[i][j]);

    // Simple key recovery for demonstration
    for(i=0;i<3;i++)
        for(j=0;j<3;j++)
        {
            K[i][j]=0;
            for(k=0;k<3;k++)
                K[i][j]=(K[i][j]+P[i][k]*C[k][j])%26;
        }

    printf("Recovered Key Matrix:\n");
    for(i=0;i<3;i++)
    {
        for(j=0;j<3;j++)
            printf("%d ",K[i][j]);
        printf("\n");
    }

    return 0;
}