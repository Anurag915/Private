#include <stdio.h>
#include <stdint.h>
#define AES_BLOCK_SIZE 16
#define AES_KEY_SIZE 16
#define AES_ROUNDS 10

uint8_t SBox[256], InvSBox[256];

void generateSBox()
{
    uint8_t p = 1, q = 1;
    do
    {
        p = p ^ (p << 1) ^ ((p & 0x80) ? 0x1B : 0);
        q ^= q << 1;
        q ^= q << 2;
        q ^= q << 4;
        q ^= (q & 0x80) ? 0x09 : 0;
        SBox[p] = (q ^ 0x63);
    } while (p != 1);

    SBox[0] = 0x63;

    for (int i = 0; i < 256; i++)
    {
        InvSBox[SBox[i]] = i;
    }
}

void keyExpansion(uint8_t *key, uint8_t *roundKeys)
{
    uint8_t temp[4], i, k;
    uint8_t Rcon[10] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36};

    for (i = 0; i < AES_KEY_SIZE; i++)
        roundKeys[i] = key[i];

    for (i = 16, k = 0; i < AES_BLOCK_SIZE * (AES_ROUNDS + 1); i += 4)
    {
        for (int j = 0; j < 4; j++)
            temp[j] = roundKeys[i - 4 + j];

        if (i % AES_KEY_SIZE == 0)
        {
            uint8_t t = temp[0];
            temp[0] = SBox[temp[1]] ^ Rcon[k++];
            temp[1] = SBox[temp[2]];
            temp[2] = SBox[temp[3]];
            temp[3] = SBox[t];
        }

        for (int j = 0; j < 4; j++)
            roundKeys[i + j] = roundKeys[i + j - 16] ^ temp[j];
    }
}

void addRoundKey(uint8_t state[4][4], uint8_t *roundKey, int round)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            state[j][i] ^= roundKey[round * AES_BLOCK_SIZE + i * 4 + j];
}

void subBytes(uint8_t state[4][4])
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            state[i][j] = SBox[state[i][j]];
}

void invSubBytes(uint8_t state[4][4])
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            state[i][j] = InvSBox[state[i][j]];
}

void shiftRows(uint8_t state[4][4])
{
    uint8_t temp;

    temp = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3];
    state[1][3] = temp;

    temp = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = temp;
    temp = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = temp;

    temp = state[3][0];
    state[3][0] = state[3][3];
    state[3][3] = state[3][2];
    state[3][2] = state[3][1];
    state[3][1] = temp;
}

void invShiftRows(uint8_t state[4][4])
{
    uint8_t temp;

    temp = state[1][3];
    state[1][3] = state[1][2];
    state[1][2] = state[1][1];
    state[1][1] = state[1][0];
    state[1][0] = temp;

    temp = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = temp;
    temp = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = temp;

    temp = state[3][0];
    state[3][0] = state[3][1];
    state[3][1] = state[3][2];
    state[3][2] = state[3][3];
    state[3][3] = temp;
}

uint8_t gf_mul(uint8_t a, uint8_t b)
{
    uint8_t p = 0;
    for (int i = 0; i < 8; i++)
    {
        if (b & 1)
            p ^= a;
        uint8_t hi_bit_set = a & 0x80;
        a <<= 1;
        if (hi_bit_set)
            a ^= 0x1B;
        b >>= 1;
    }
    return p;
}

void mixColumns(uint8_t state[4][4])
{
    uint8_t temp[4];
    for (int i = 0; i < 4; i++)
    {
        temp[0] = gf_mul(state[0][i], 2) ^ gf_mul(state[1][i], 3) ^ state[2][i] ^ state[3][i];
        temp[1] = state[0][i] ^ gf_mul(state[1][i], 2) ^ gf_mul(state[2][i], 3) ^ state[3][i];
        temp[2] = state[0][i] ^ state[1][i] ^ gf_mul(state[2][i], 2) ^ gf_mul(state[3][i], 3);
        temp[3] = gf_mul(state[0][i], 3) ^ state[1][i] ^ state[2][i] ^ gf_mul(state[3][i], 2);

        for (int j = 0; j < 4; j++)
            state[j][i] = temp[j];
    }
}

void invMixColumns(uint8_t state[4][4])
{
    uint8_t temp[4];
    for (int i = 0; i < 4; i++)
    {
        temp[0] = gf_mul(state[0][i], 0x0E) ^ gf_mul(state[1][i], 0x0B) ^ gf_mul(state[2][i], 0x0D) ^ gf_mul(state[3][i], 0x09);
        temp[1] = gf_mul(state[0][i], 0x09) ^ gf_mul(state[1][i], 0x0E) ^ gf_mul(state[2][i], 0x0B) ^ gf_mul(state[3][i], 0x0D);
        temp[2] = gf_mul(state[0][i], 0x0D) ^ gf_mul(state[1][i], 0x09) ^ gf_mul(state[2][i], 0x0E) ^ gf_mul(state[3][i], 0x0B);
        temp[3] = gf_mul(state[0][i], 0x0B) ^ gf_mul(state[1][i], 0x0D) ^ gf_mul(state[2][i], 0x09) ^ gf_mul(state[3][i], 0x0E);

        for (int j = 0; j < 4; j++)
            state[j][i] = temp[j];
    }
}

void printState(uint8_t state[4][4])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
            printf("%02X ", state[i][j]);
        printf("\n");
    }
    printf("\n");
}

void encrypt(uint8_t state[4][4], uint8_t *roundKeys)
{
    addRoundKey(state, roundKeys, 0);

    for (int round = 1; round < AES_ROUNDS; round++)
    {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, roundKeys, round);
    }

    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKeys, AES_ROUNDS);
}

void decrypt(uint8_t state[4][4], uint8_t *roundKeys)
{
    addRoundKey(state, roundKeys, AES_ROUNDS);

    for (int round = AES_ROUNDS - 1; round > 0; round--)
    {
        invShiftRows(state);
        invSubBytes(state);
        addRoundKey(state, roundKeys, round);
        invMixColumns(state);
    }

    invShiftRows(state);
    invSubBytes(state);
    addRoundKey(state, roundKeys, 0);
}

int main()
{
    generateSBox();

    uint8_t key[AES_KEY_SIZE] = {0x2B, 0x28, 0xAB, 0x09, 0x7E, 0xAE, 0xF7, 0xCF,
                                 0x15, 0xD2, 0x15, 0x4F, 0x16, 0xA6, 0x88, 0x3C};
    uint8_t roundKeys[176];
    keyExpansion(key, roundKeys);

    uint8_t plaintext[4][4] = {
        {0x32, 0x88, 0x31, 0xE0},
        {0x43, 0x5A, 0x31, 0x37},
        {0xF6, 0x30, 0x98, 0x07},
        {0xA8, 0x8D, 0xA2, 0x34}};

    printf("Plaintext:\n");
    printState(plaintext);

    encrypt(plaintext, roundKeys);
    printf("Encrypted Text:\n");
    printState(plaintext);

    decrypt(plaintext, roundKeys);
    printf("Decrypted Text:\n");
    printState(plaintext);

    return 0;
}