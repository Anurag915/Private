#include<stdio.h>
long long gcd(long long a,long long b){
    while(b!=0){
        long long r=a%b;
        a=b;
        b=r;
    }
    return a;
}

int main(){
    long long a,b;
    scanf("%lld %lld",&a,&b);
    if(b>a){
        long long temp=b;
        b=a;
        a=temp;
    }
    long long r=gcd(a,b);
    printf("%lld",r);
    return 0;
}