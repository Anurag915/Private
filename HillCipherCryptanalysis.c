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

// Function to calculate determinant
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

// Function to find the inverse of a matrix
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

// Function to deduce the key matrix
int deduceKeyMatrix(char *plaintext, char *ciphertext, int keyMatrix[3][3], int size) {
    int plainMatrix[3][3], cipherMatrix[3][3], inversePlain[3][3];

    int len = strlen(plaintext);
    if (len < size) {
        printf("Insufficient length of plaintext and ciphertext for the given size.\n");
        return 0;
    }

    // Convert plaintext and ciphertext to numeric matrices
    for (int i = 0, idx = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            plainMatrix[i][j] = toupper(plaintext[idx]) - 'A';
            cipherMatrix[i][j] = toupper(ciphertext[idx]) - 'A';
            idx++;
        }
    }

    // Find inverse of plaintext matrix
    if (!inverseMatrix(plainMatrix, inversePlain, size)) {
        printf("Plaintext matrix is not invertible. Cannot deduce key.\n");
        return 0;
    }

    // Calculate the key matrix
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            keyMatrix[i][j] = 0;
            for (int k = 0; k < size; k++) {
                keyMatrix[i][j] += (inversePlain[i][k] * cipherMatrix[k][j]) % 26;
            }
            keyMatrix[i][j] %= 26;
            if (keyMatrix[i][j] < 0) {
                keyMatrix[i][j] += 26;
            }
        }
    }
    return 1;
}

int main() {
    char plaintext[MAX], ciphertext[MAX];
    int keyMatrix[3][3], size;

    printf("Enter the size of the key matrix (2 for 2x2 or 3 for 3x3): ");
    scanf("%d", &size);

    if (size != 2 && size != 3) {
        printf("Invalid key matrix size. Please enter 2 or 3.\n");
        return 1;
    }

    printf("Enter the plaintext (only letters A-Z): ");
    scanf("%s", plaintext);

    printf("Enter the ciphertext (only letters A-Z): ");
    scanf("%s", ciphertext);

    if (deduceKeyMatrix(plaintext, ciphertext, keyMatrix, size)) {
        printf("Deduced Key Matrix:\n");
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                printf("%d ", keyMatrix[i][j]);
            }
            printf("\n");
        }
    } else {
        printf("Failed to deduce the key matrix.\n");
    }

    return 0;
}

