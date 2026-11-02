
#include <stdio.h>
#include <wchar.h>
#include <locale.h>

int main() {
    setlocale(LC_ALL, "");

    wchar_t c[] = L"53‡‡†305))6*;4826)4‡.)4‡);806*;48†8¶60))85;;]8*;:‡*8†83"
                   L"(88)5*†;46(;88*96*?;8)*‡(;485);5*†2:*‡(;4956*2(5*—4)8¶8*"
                   L";4069285);)6†8)4‡‡;1(‡9;48081;8:8‡1;48†85;4)485†528806*81"
                   L"(‡9;48;(88;4(‡?34;48)4‡;161;:188;‡?;";

    wchar_t key[] = L"53‡†0)6*;482.¶]:(9?—1";
    wchar_t val[] = L"agodlsinthebpvwyrmucf";
    int i, j;

    for (i = 0; c[i]; i++) {
        for (j = 0; key[j] && key[j] != c[i]; j++);
        if (key[j])
            putwchar(val[j]);
        else
            putwchar(c[i]);
    }

    return 0;
}
