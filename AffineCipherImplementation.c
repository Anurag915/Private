#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include <ctype.h>
int gcd(int a,int b){
    while(b!=0){
        int r=a%b;
        a=b;
        b=r;
    }
    return a;
}
int numericvalue(char ch){
    return ch-'A';
}
char charactervalue(int num){
    return 'A'+num;
}
void encrypt(char *s,char *l,int a,int b){
    for(int i=0;s[i]!='\0';i++){
        char ch=s[i];
        int value=numericvalue(ch);
        int encryptedvalue=(a*value+b)%26;
        l[i]=charactervalue(encryptedvalue);
    }
}
int main(){
    int a,b;
    scanf("%d %d",&a,&b);
    if(gcd(a,26)!=1){
        printf("a should be coprime to 26");
        return 0;
    }
    char s[100],l[100];
    scanf("%s",s);
    encrypt(s,l,a,b);
    printf("%s",l);
    return 0;
}