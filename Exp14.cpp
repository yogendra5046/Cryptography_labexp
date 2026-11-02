
#include <stdio.h>

int main() {
    int P[2][2] = {{7, 11}, {4, 15}};
    int C[2][2] = {{7, 0}, {8, 19}};
    int inv[2][2], K[2][2];
    int det, d, i, j;

    det = (P[0][0]*P[1][1] - P[0][1]*P[1][0]) % 26;
    if (det < 0) det += 26;

    for (d = 1; d < 26; d++)
        if ((det*d) % 26 == 1) break;

    if (d == 26) {
        printf("Inverse does not exist");
        return 0;
    }

    inv[0][0] = P[1][1]*d % 26;
    inv[0][1] = (-P[0][1]*d % 26 + 26) % 26;
    inv[1][0] = (-P[1][0]*d % 26 + 26) % 26;
    inv[1][1] = P[0][0]*d % 26;

    for (i = 0; i < 2; i++)
        for (j = 0; j < 2; j++)
            K[i][j] = (C[i][0]*inv[0][j] +
                       C[i][1]*inv[1][j]) % 26;

    printf("Recovered key matrix:\n");
    for (i = 0; i < 2; i++)
        printf("%d %d\n", K[i][0], K[i][1]);

    return 0;
}
