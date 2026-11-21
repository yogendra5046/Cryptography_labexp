
#include <stdio.h>

int PC1[56] = {
  57,49,41,33,25,17,9,1,58,50,42,34,26,18,
  10,2,59,51,43,35,27,19,11,3,60,52,44,36,
  63,55,47,39,31,23,15,7,62,54,46,38,30,22,
  14,6,61,53,45,37,29,21,13,5,28,20,12,4
};

int PC2[48] = {
  14,17,11,24,1,5,3,28,15,6,21,10,
  23,19,12,4,26,8,16,7,27,20,13,2,
  41,52,31,37,47,55,30,40,51,45,33,48,
  44,49,39,56,34,53,46,42,50,36,29,32
};

int shifts[16] = {
  1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};

void leftShift(int *a, int n) {
    int t[28], i;
    for (i = 0; i < 28; i++)
        t[i] = a[(i + n) % 28];
    for (i = 0; i < 28; i++)
        a[i] = t[i];
}

int main() {
    int key[64], perm[56], C[28], D[28];
    int roundKey[16][48];
    int i, r;

    printf("Enter 64-bit key (0 or 1 bits):\n");
    for (i = 0; i < 64; i++)
        scanf("%1d", &key[i]);

    for (i = 0; i < 56; i++)
        perm[i] = key[PC1[i] - 1];

    for (i = 0; i < 28; i++) {
        C[i] = perm[i];
        D[i] = perm[i + 28];
    }

    for (r = 0; r < 16; r++) {
        leftShift(C, shifts[r]);
        leftShift(D, shifts[r]);

        for (i = 0; i < 48; i++) {
            int p = PC2[i] - 1;
            roundKey[r][i] =
                p < 28 ? C[p] : D[p - 28];
        }
    }

    printf("\nDES decryption round keys:\n");

    for (r = 15; r >= 0; r--) {
        printf("K%d: ", r + 1);
        for (i = 0; i < 48; i++)
            printf("%d", roundKey[r][i]);
        printf("\n");
    }

    return 0;
}
