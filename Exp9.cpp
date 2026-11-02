#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main() {
    char key[30], s[200], a[25], c;
    int i, j, n=0, x,y,p,q;

    scanf("%29s", key);
    strcpy(s,"KXJEYUREBEZWEHEWRYTUHEYFSKREHEGOYFIWTTTUOLKSYCAJPOBOTEIZONTXBYBNTGONEYCUZWRGDSONSXBOUYWRHEBAAHYUSEDQ");

    strcat(key,"ABCDEFGHIKLMNOPQRSTUVWXYZ");

    for(i=0;key[i];i++){
        c=toupper(key[i]);
        for(j=0;j<n&&a[j]!=c;j++);
        if(c>='A'&&c<='Z'&&c!='J'&&j==n)a[n++]=c;
    }

    for(i=0;s[i];i+=2){
        for(j=0;j<25;j++){
            if(a[j]==s[i])x=j/5,y=j%5;
            if(a[j]==s[i+1])p=j/5,q=j%5;
        }
        if(x==p)s[i]=a[x*5+(y+4)%5],s[i+1]=a[p*5+(q+4)%5];
        else if(y==q)s[i]=a[((x+4)%5)*5+y],s[i+1]=a[((p+4)%5)*5+q];
        else s[i]=a[x*5+q],s[i+1]=a[p*5+y];
    }

    printf("%s\n",s);
    return 0;
}