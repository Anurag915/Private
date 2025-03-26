#include<stdio.h>

int gcd(int a,int b){
    while(b!=0){
        int r=a%b;
        a=b;
        b=r;
    }
    return a;
}
int numVal(char ch){
    return ch-'A';
}
char charVal(int num){
    return 'A'+num;
}
int modularinverse(int a){
    for(int i=1;i<26;++i){
        if((a*i)%26==1){
            return i;
        }
    }
    return -1;
}
void decrypt(char *c,int a,int b,char *p){
    if(gcd(a,26)!=1){
        return ;
    }
    int a_inverse=modularinverse(a);
    if(a==-1){
        return;
    }
    for(int i=0;c[i]!='\0';++i){
        char ch=c[i];
        int value=numVal(ch);
        int decrypted_value=(a_inverse*(value-b+26))%26;
        p[i]=charVal(decrypted_value);
    }
}
void cryptanalysis( char *c){
    printf("Possible Plaintext\n");
    for(int a=1;a<26;a++){
        if(gcd(a,26)==1){
        for(int b=0;b<26;b++){
            char p[100];
            decrypt(c,a,b,p);
            printf("a:%d b:%d  Plaintext: %s\n",a,b,p);
            }
        }
    }
}
int main(){
    char c[100];
    scanf("%s",c);
    cryptanalysis(c);
    return 0;
}