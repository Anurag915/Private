#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ConvertBinaryStringToArray(const char *binaryString, int *array, int *length) {
    *length = strlen(binaryString);
    for (int i = 0; i < *length; i++) {
        array[i] = binaryString[i] - '0';
    }
}

void GenerateKeyStream(int *key, int keylength, int *coefficient, int *keystream, int messagelength) {
    int lfsr[keylength];
    memcpy(lfsr, key, keylength * sizeof(int));

    for (int i = 0; i < messagelength; i++) {
        keystream[i] = lfsr[0];

        int feedback = 0;
        for (int j = 0; j < keylength; j++) {
            feedback ^= (lfsr[j] * coefficient[j]);
        }
        feedback = feedback & 1;

        for (int j = 0; j < keylength - 1; j++) {
            lfsr[j] = lfsr[j + 1];
        }
        lfsr[keylength - 1] = feedback;
    }
}

void XORMessage(int *message, int *keystream, int *result, int length) {
    for (int i = 0; i < length; i++) {
        result[i] = message[i] ^ keystream[i];
    }
}

void PrintArray(int *array, int length) {
    for (int i = 0; i < length; i++) {
        printf("%d", array[i]);
    }
    printf("\n");
}

int main() {
    char keyStr[100], coefficientStr[100], messageStr[100];
    int keylength, coefficientlength, messagelength;

    printf("Enter the key (binary string): ");
    scanf("%s", keyStr);
    printf("Enter the coefficient (binary string): ");
    scanf("%s", coefficientStr);
    printf("Enter the message (binary string): ");
    scanf("%s", messageStr);

    keylength = strlen(keyStr);
    coefficientlength = strlen(coefficientStr);
    messagelength = strlen(messageStr);

    int key[keylength];
    int coefficient[coefficientlength];
    int message[messagelength];

    ConvertBinaryStringToArray(keyStr, key, &keylength);
    ConvertBinaryStringToArray(coefficientStr, coefficient, &coefficientlength);
    ConvertBinaryStringToArray(messageStr, message, &messagelength);

    int keystream[messagelength];
    GenerateKeyStream(key, keylength, coefficient, keystream, messagelength);

    printf("Key Stream: ");
    PrintArray(keystream, messagelength);

    int ciphertext[messagelength];
    XORMessage(message, keystream, ciphertext, messagelength);

    printf("Plaintext: ");
    PrintArray(message, messagelength);

    printf("CipherText: ");
    PrintArray(ciphertext, messagelength);

    int decryptedmessage[messagelength];
    XORMessage(ciphertext, keystream, decryptedmessage, messagelength);

    printf("Decrypted Text: ");
    PrintArray(decryptedmessage, messagelength);

    int valid = 1;
    for (int i = 0; i < messagelength; i++) {
        if (message[i] != decryptedmessage[i]) {
            valid = 0;
            break;
        }
    }

    if (valid) {
        printf("LFSR works properly\n");
    } else {
        printf("LFSR doesn't work properly\n");
    }

    return 0;
}
