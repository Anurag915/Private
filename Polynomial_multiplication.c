#include <stdio.h>
#include <stdlib.h>
int multiply(int a, int b, int m, int deg_m) {
    int result = 0;
    while (b) {
        if (b & 1) {
            result ^= a; 
        }
        b >>= 1;
        
        
        if (a & (1 << (deg_m - 1))) {
            a = (a << 1) ^ m;
        } else {
            a <<= 1;
        }
    }
    return result;
}

int main() {
    int deg_a, deg_b, deg_m;
    int a = 0, b = 0, m = 0;
    
 
    printf("Enter degree of polynomial a(x): ");
    scanf("%d", &deg_a);
    printf("Enter degree of polynomial b(x): ");
    scanf("%d", &deg_b);
    printf("Enter degree of irreducible polynomial m(x) (5-15): ");
    scanf("%d", &deg_m);
    
    printf("Enter coefficients of a(x) from highest to lowest degree: ");
    for (int i = deg_a; i >= 0; i--) {
        int coef;
        scanf("%d", &coef);
        if (coef) a |= (1 << i);
    }

    printf("Enter coefficients of b(x) from highest to lowest degree: ");
    for (int i = deg_b; i >= 0; i--) {
        int coef;
        scanf("%d", &coef);
        if (coef) b |= (1 << i);
    }
    
    printf("Enter coefficients of m(x) from highest to lowest degree: ");
    for (int i = deg_m; i >= 0; i--) {
        int coef;
        scanf("%d", &coef);
        if (coef) m |= (1 << i);
    }
    
    int result = multiply(a, b, m, deg_m);

    printf("Resulting polynomial coefficients: ");
    for (int i = deg_m - 1; i >= 0; i--) {
        printf("%d ", (result >> i) & 1);
    }
    printf("\n");
    
    return 0;
}
