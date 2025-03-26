#include <stdio.h>
#include <math.h> 
int determinant_2x2_mod26(int matrix[2][2]) {
    int det = (matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0]) % 26;
    if (det < 0) det += 26; 
    return det;
}

int determinant_3x3_mod26(int matrix[3][3]) {
    int det = (matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) -
               matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) +
               matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0])) % 26;

    if (det < 0) det += 26;
    return det;
}

int modular_inverse(int a, int mod) {
    for (int i = 1; i < mod; i++) {
        if ((a * i) % mod == 1) {
            return i;
        }
    }
    return -1; 
}


void cofactor_3x3_matrix(int matrix[3][3], int cofactor[3][3]) {
    cofactor[0][0] = (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) % 26;
    cofactor[0][1] = -(matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) % 26;
    cofactor[0][2] = (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]) % 26;

    cofactor[1][0] = -(matrix[0][1] * matrix[2][2] - matrix[0][2] * matrix[2][1]) % 26;
    cofactor[1][1] = (matrix[0][0] * matrix[2][2] - matrix[0][2] * matrix[2][0]) % 26;
    cofactor[1][2] = -(matrix[0][0] * matrix[2][1] - matrix[0][1] * matrix[2][0]) % 26;

    cofactor[2][0] = (matrix[0][1] * matrix[1][2] - matrix[0][2] * matrix[1][1]) % 26;
    cofactor[2][1] = -(matrix[0][0] * matrix[1][2] - matrix[0][2] * matrix[1][0]) % 26;
    cofactor[2][2] = (matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0]) % 26;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (cofactor[i][j] < 0) cofactor[i][j] += 26; 
        }
    }
}


void transpose(int size, int matrix[size][size], int transpose[size][size]) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            transpose[i][j] = matrix[j][i];
        }
    }
}

int main() {

    int input[100], count = 0;
    while (scanf("%d", &input[count]) == 1) {
        count++;
    }

    int size = sqrt(count);
    if (size * size != count || (size != 2 && size != 3)) {
        
        return 1;
    }

    int matrix[size][size];
    int index = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i][j] = input[index++];
        }
    }

    if (size == 2) {
        int det = determinant_2x2_mod26(matrix);
        int det_inv = modular_inverse(det, 26);

        if (det_inv == -1) {
           
            return 1;
        }

        int inverse[2][2];
        inverse[0][0] = (matrix[1][1] * det_inv) % 26;
        inverse[0][1] = (-matrix[0][1] * det_inv) % 26;
        inverse[1][0] = (-matrix[1][0] * det_inv) % 26;
        inverse[1][1] = (matrix[0][0] * det_inv) % 26;

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                if (inverse[i][j] < 0) inverse[i][j] += 26;
            }
        }


        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                printf("%d ", inverse[i][j]);
            }
            printf("\n");
        }
    } else if (size == 3) {
        int det = determinant_3x3_mod26(matrix);
        int det_inv = modular_inverse(det, 26);

        if (det_inv == -1) {
           
            return 1;
        }

        int cofactor[3][3];
        cofactor_3x3_matrix(matrix, cofactor);

        int adjugate[3][3];
        transpose(3, cofactor, adjugate);

        int inverse[3][3];
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                inverse[i][j] = (adjugate[i][j] * det_inv) % 26;
                if (inverse[i][j] < 0) inverse[i][j] += 26;
            }
        }


        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                printf("%d ", inverse[i][j]);
            }
            printf("\n");
        }
    }

    return 0;
}