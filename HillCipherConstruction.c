#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 100

// Function to calculate modular inverse
int modInverse(int a, int m) {
    for (int x = 1; x < m; x++) {
        if ((a * x) % m == 1) {
            return x;
        }
    }
    return -1; // No modular inverse
}

// Function to get the cofactor matrix
void getCofactor(int mat[3][3], int temp[3][3], int p, int q, int n) {
    int i = 0, j = 0;
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            if (row != p && col != q) {
                temp[i][j++] = mat[row][col];
                if (j == n - 1) {
                    j = 0;
                    i++;
                }
            }
        }
    }
}

// Function to calculate the determinant
int determinant(int mat[3][3], int n) {
    int det = 0;
    if (n == 1) {
        return mat[0][0];
    }

    int temp[3][3];
    int sign = 1;

    for (int f = 0; f < n; f++) {
        getCofactor(mat, temp, 0, f, n);
        det += sign * mat[0][f] * determinant(temp, n - 1);
        sign = -sign;
    }

    return det;
}

// Function to calculate adjoint matrix
void adjoint(int mat[3][3], int adj[3][3], int n) {
    if (n == 1) {
        adj[0][0] = 1;
        return;
    }

    int temp[3][3];
    int sign = 1;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            getCofactor(mat, temp, i, j, n);
            sign = ((i + j) % 2 == 0) ? 1 : -1;
            adj[j][i] = (sign * determinant(temp, n - 1));
        }
    }
}

// Function to find the inverse matrix
int inverseMatrix(int mat[3][3], int inv[3][3], int n) {
    int det = determinant(mat, n);
    det = (det % 26 + 26) % 26;

    int detInv = modInverse(det, 26);
    if (detInv == -1) {
        return 0; // Inverse does not exist
    }

    int adj[3][3];
    adjoint(mat, adj, n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            inv[i][j] = (adj[i][j] * detInv) % 26;
            if (inv[i][j] < 0) {
                inv[i][j] += 26;
            }
        }
    }
    return 1;
}

// Function to process text: encryption or decryption
void processText(int keyMatrix[3][3], char *input, char *output, int size, int decrypt) {
    int inputNumeric[MAX], result[MAX];
    int length = strlen(input);

    int inverseKey[3][3];
    if (decrypt) {
        if (!inverseMatrix(keyMatrix, inverseKey, size)) {
            printf("Key matrix is not invertible. Decryption failed.\n");
            return;
        }
    }

    int (*matrix)[3] = decrypt ? inverseKey : keyMatrix;

    for (int i = 0, idx = 0; i < length; i++) {
        if (isalpha(input[i])) {
            inputNumeric[idx++] = toupper(input[i]) - 'A';
        } else {
            output[i] = input[i]; // Keep non-alphabetic characters as-is
        }
    }

    // Pad input if necessary
    int paddedLength = length;
    if (paddedLength % size != 0) {
        int padding = size - (paddedLength % size);
        for (int i = 0; i < padding; i++) {
            inputNumeric[paddedLength++] = 'X' - 'A';
        }
    }

    // Process blocks
    for (int i = 0, idx = 0; i < paddedLength; i += size) {
        for (int row = 0; row < size; row++) {
            result[idx + row] = 0;
            for (int col = 0; col < size; col++) {
                result[idx + row] += matrix[row][col] * inputNumeric[i + col];
            }
            result[idx + row] %= 26;
        }
        idx += size;
    }

    // Convert result to output text
    for (int i = 0, idx = 0; i < length; i++) {
        if (isalpha(input[i])) {
            output[i] = result[idx++] + 'A';
        }
    }
    output[length] = '\0';
}

// Function to input the key matrix as a single string
void getKeyMatrix(int mat[3][3], int *size) {
    char keyString[MAX];

    printf("Enter the key matrix as a single string of characters (A-Z): ");
    scanf("%s", keyString);
    int keyLength = strlen(keyString);

    // Determine matrix size from string length (2x2 or 3x3)
    if (keyLength == 9) {
        *size = 3;
    } else if (keyLength == 4) {
        *size = 2;
    } else {
        printf("Invalid key matrix size. Please enter a string of 4 or 9 characters.\n");
        return;
    }

    // Fill the matrix with values from the input string
    for (int i = 0, idx = 0; i < *size; i++) {
        for (int j = 0; j < *size; j++) {
            mat[i][j] = toupper(keyString[idx++]) - 'A'; // Convert letter to corresponding number
        }
    }
}

int main() {
    int keyMatrix[3][3];
    char plaintext[MAX], ciphertext[MAX], decryptedText[MAX];
    int size;

    // Get the key matrix
    getKeyMatrix(keyMatrix, &size);

    // Input plaintext
    printf("Enter the plaintext (can include spaces and special characters): ");
    getchar(); // Consume newline left in the buffer
    fgets(plaintext, MAX, stdin);
    plaintext[strcspn(plaintext, "\n")] = '\0'; // Remove trailing newline

    // Encrypt the plaintext
    processText(keyMatrix, plaintext, ciphertext, size, 0);
    printf("Ciphertext: %s\n", ciphertext);

    // Decrypt the ciphertext
    processText(keyMatrix, ciphertext, decryptedText, size, 1);
    printf("Decrypted text: %s\n", decryptedText);

    return 0;
}
