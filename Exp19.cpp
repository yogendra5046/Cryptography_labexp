
#include <stdio.h>
#include <string.h>


int main() {
    EVP_CIPHER_CTX *ctx;
    unsigned char key[24] = "12345678901234567890123";
    unsigned char iv[8] = "12345678";
    unsigned char plaintext[] = "HELLO CBC MODE";
    unsigned char ciphertext[128];
    int len, ciphertext_len;

    ctx = EVP_CIPHER_CTX_new();

    EVP_EncryptInit_ex(ctx, EVP_des_ede3_cbc(),
                       NULL, key, iv);

    EVP_EncryptUpdate(ctx, ciphertext, &len,
                      plaintext, strlen((char *)plaintext));
    ciphertext_len = len;

    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    ciphertext_len += len;

    printf("Ciphertext (hex): ");
    for (int i = 0; i < ciphertext_len; i++)
        printf("%02X", ciphertext[i]);

    printf("\n");

    EVP_CIPHER_CTX_free(ctx);
    return 0;
}
