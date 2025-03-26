#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 32

typedef struct {
    int bits[MAX_SIZE];
    int size;
} BinaryVector;

void readBinaryVector(BinaryVector* vector, const char* prompt) {
    char input[MAX_SIZE + 1];  
    
    printf("%s ", prompt);
    scanf("%s", input);  
    
    vector->size = strlen(input); 
    
    if (vector->size > MAX_SIZE || vector->size <= 0) {
        printf("Error: Size must be between 1 and %d\n", MAX_SIZE);
        exit(1);
    }
        for (int i = 0; i < vector->size; i++) {
        if (input[i] == '0') {
            vector->bits[i] = 0;
        } else if (input[i] == '1') {
            vector->bits[i] = 1;
        } else {
            printf("Error: Invalid input, only 0s and 1s allowed\n");
            exit(1);
        }
    }
}

void printBinaryVector(const BinaryVector* vector, const char* label) {
    printf("%s: ", label);
    for (int i = 0; i < vector->size; i++) {
        printf("%d ", vector->bits[i]);
    }
    printf("\n");
}

void recoverKeystream(const BinaryVector* plaintext, const BinaryVector* ciphertext, BinaryVector* keystream) {
    keystream->size = plaintext->size;
    for (int i = 0; i < plaintext->size; i++) {
        keystream->bits[i] = plaintext->bits[i] ^ ciphertext->bits[i];
    }
}

void solveLinearEquations(int equations[][MAX_SIZE + 1], int n, int* solution) {
    for (int i = 0; i < n; i++) {
        int pivot_row = i;
        while (pivot_row < n && equations[pivot_row][i] == 0) {
            pivot_row++;
        }
        if (pivot_row == n) {
            printf("No unique solution exists\n");
            return;
        }

        if (pivot_row != i) {
            for (int j = 0; j <= n; j++) {
                int temp = equations[i][j];
                equations[i][j] = equations[pivot_row][j];
                equations[pivot_row][j] = temp;
            }
        }
        for (int j = i + 1; j < n; j++) {
            if (equations[j][i] == 1) {
                for (int k = i; k <= n; k++) {
                    equations[j][k] ^= equations[i][k];
                }
            }
        }
    }

    for (int i = n - 1; i >= 0; i--) {
        solution[i] = equations[i][n];
        for (int j = i + 1; j < n; j++) {
            solution[i] ^= (equations[i][j] & solution[j]);
        }
    }
}

void findLFSRConfiguration(const BinaryVector* keystream, int seedLength, BinaryVector* coefficients, BinaryVector* seed) {
    int n = seedLength;
    int equations[MAX_SIZE][MAX_SIZE + 1] = {0};
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            equations[i][j] = keystream->bits[i + j];
        }
        equations[i][n] = keystream->bits[i + n];
    }
    
    int solution[MAX_SIZE];
    solveLinearEquations(equations, n, solution);
    
    coefficients->size = n;
    for (int i = 0; i < n; i++) {
        coefficients->bits[i] = solution[i];
    }
    
    int seedEquations[MAX_SIZE][MAX_SIZE + 1] = {0};
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            seedEquations[i][j] = (j == i) ? 1 : 0;
        }
        seedEquations[i][n] = keystream->bits[i];
    }
    
    solveLinearEquations(seedEquations, n, solution);
    
    seed->size = n;
    for (int i = 0; i < n; i++) {
        seed->bits[i] = solution[i];
    }
}

int main() {
    BinaryVector plaintext, ciphertext, keystream, recoveredCoefficients, recoveredSeed;
    int seedLength;
    
    readBinaryVector(&plaintext, "Enter known plaintext");
    
    readBinaryVector(&ciphertext, "Enter corresponding ciphertext");
    
    if (plaintext.size != ciphertext.size) {
        printf("Error: Plaintext and ciphertext must have same length\n");
        return 1;
    }
    
    printf("Enter LFSR seed length: ");
    scanf("%d", &seedLength);
    
    if (seedLength > plaintext.size || seedLength <= 0) {
        printf("Error: Invalid seed length\n");
        return 1;
    }
    
    recoverKeystream(&plaintext, &ciphertext, &keystream);
    printBinaryVector(&keystream, "Keystream");
    
    findLFSRConfiguration(&keystream, seedLength, &recoveredCoefficients, &recoveredSeed);
    
    printBinaryVector(&recoveredCoefficients, "Coefficient vector");
    printBinaryVector(&recoveredSeed, "Key Vector");
    
    return 0;
}
