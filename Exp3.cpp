#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main() {
    char a[25],k[30],s[100],c;
    int i,j,n=0,x,y,p,q,r,t;
    scanf("%s%s",k,s);
    strcat(k,"ABCDEFGHIKLMNOPQRSTUVWXYZ");

    for(i=0;k[i];i++){
        c=toupper(k[i]);
        for(j=0;j<n&&a[j]!=c;j++);
        if(c>='A'&&c<='Z'&&c!='J'&&j==n)a[n++]=c;
    }

    for(i=0;s[i];i+=2){
        s[i]=toupper(s[i])=='J'?'I':toupper(s[i]);
        s[i+1]=s[i+1]?toupper(s[i+1]):'X';
        if(s[i+1]=='J')s[i+1]='I';
        for(j=0;j<25;j++){
            if(a[j]==s[i])x=j/5,y=j%5;
            if(a[j]==s[i+1])p=j/5,q=j%5;
        }
        if(x==p)s[i]=a[x*5+(y+1)%5],s[i+1]=a[p*5+(q+1)%5];
        else if(y==q)s[i]=a[((x+1)%5)*5+y],s[i+1]=a[((p+1)%5)*5+q];
        else s[i]=a[x*5+q],s[i+1]=a[p*5+y];
    }
    printf("%s",s);
}