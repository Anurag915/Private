// C program to demonstrate working of 
// extended Euclidean Algorithm 
#include <stdio.h>

// Function for extended Euclidean Algorithm 
int gcdExtended(int a, int b, int *x, int *y) {

    // Base Case 
    if (a == 0) { 
        *x = 0; 
        *y = 1; 
        return b; 
    } 

    int x1, y1; 
    int gcd = gcdExtended(b % a, a, &x1, &y1); 

    // Update x and y using results of 
    // recursive call 
    *x = y1 - (b / a) * x1; 
    *y = x1; 
    return gcd; 
} 

int findGCD(int a, int b) {

    int x = 1, y = 1;
    return gcdExtended(a, b, &x, &y);
}

int main() { 
    int a = 35, b = 15;
    int g = findGCD(a, b); 
    printf("%d\n", g);
    return 0; 
}