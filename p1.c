#include <stdio.h>
#include <stdlib.h>

void xor(int a[], int b[], int result[], int length) {
    for (int i = 0; i < length; i++) {
        result[i] = a[i] ^ b[i];
    }
}

void print_array(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int coeff(int keystream[], int l, int keystream_size, int coeffs[]) {
    int total = 1 << l;  
    for (int i = 0; i < total; i++) {
        for (int j = 0; j < l; j++) {
            coeffs[j] = (i >> j) & 1;  
        }

        int generated_keystream[keystream_size];
        generate_keystream(coeffs, l, keystream_size, generated_keystream);
        
        int match = 1;    
        for (int i = 0; i < keystream_size; i++) {
            if (generated_keystream[i] != keystream[i]) {
                match = 0;
                break;
            }
        }
        if (match) {
            return 1; 
        }
    }
    return 0;
}

int main() {
    int l = 5;
    int n = 10;
    int plaintext[] = {1, 0, 0, 1, 1, 0, 1, 1, 0, 0};
    int ciphertext[] = {0, 1, 1, 0, 0, 1, 0, 0, 1, 1};
    
    int keystream[n];
    xor(plaintext, ciphertext, keystream, n);
    
    printf("Keystream calculated : ");
    print_array(keystream, n);
    
    int coeffs[l];
    if (coeff(keystream, l, n, coeffs)) {
        printf("Recovered Coefficients: ");
        print_array(coeffs, l);
    } else {
        printf("Coefficients not found.\n");
    }
  
    return 0;
}