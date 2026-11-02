
#include <stdio.h>
#include <math.h>

int main() {
    double log2keys = 0;
    int i;

    for (i = 1; i <= 25; i++)
        log2keys += log(i) / log(2);

    printf("Possible keys = 25!\n");
    printf("Approximate power of 2 = 2^%.2f\n", log2keys);

    return 0;
}
