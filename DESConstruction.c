#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int PERM_TABLE_1[] = {
    58,50,42,34,26,18,10,2,60,52,44,36,28,20,12,4,
    62,54,46,38,30,22,14,6,64,56,48,40,32,24,16,8,
    57,49,41,33,25,17,9,1,59,51,43,35,27,19,11,3,
    61,53,45,37,29,21,13,5,63,55,47,39,31,23,15,7
};

int PERM_TABLE_2[] = {
    40,8,48,16,56,24,64,32,39,7,47,15,55,23,63,31,
    38,6,46,14,54,22,62,30,37,5,45,13,53,21,61,29,
    36,4,44,12,52,20,60,28,35,3,43,11,51,19,59,27,
    34,2,42,10,50,18,58,26,33,1,41,9,49,17,57,25
};

int EXPAND_TABLE[] = {
    32,1,2,3,4,5,4,5,6,7,8,9,8,9,10,11,
    12,13,12,13,14,15,16,17,16,17,18,19,
    20,21,20,21,22,23,24,25,24,25,26,27,
    28,29,28,29,30,31,32,1
};

int PERM_FUNC[] = {
    16,7,20,21,29,12,28,17,1,15,23,26,5,18,31,10,
    2,8,24,14,32,27,3,9,19,13,30,6,22,11,4,25
};

int KEY_PERM_1[] = {
    57,49,41,33,25,17,9,1,58,50,42,34,26,18,
    10,2,59,51,43,35,27,19,11,3,60,52,44,36,
    63,55,47,39,31,23,15,7,62,54,46,38,30,22,
    14,6,61,53,45,37,29,21,13,5,28,20,12,4
};

int KEY_PERM_2[] = {
    14,17,11,24,1,5,3,28,15,6,21,10,23,19,12,4,
    26,8,16,7,27,20,13,2,41,52,31,37,47,55,30,40,
    51,45,33,48,44,49,39,56,34,53,46,42,50,36,29,32
};

int SHIFT_AMT[] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

int SUBSTITUTION_BOXES[8][4][16] = {
    {{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
    {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
    {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
    {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}},
    {{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
    {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
    {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
    {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}},
    {{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
    {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
    {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
    {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}},
    {{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
    {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
    {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
    {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}},
    {{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
    {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
    {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
    {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}},
    {{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
    {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
    {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
    {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}},
    {{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
    {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
    {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
    {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}},
    {{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
    {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
    {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
    {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}}
};

void show_output(const char* label, uint64_t val, int bits) {
    printf("%-25s: %0*llx\n", label, bits/4, val);
}

void flush_input() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int check_hex(const char *s) {
    for(int i=0; s[i]; i++) {
        if(!isxdigit(s[i])) return 0;
    }
    return 1;
}

uint64_t apply_perm(uint64_t in, const int *tbl, int sz, const char* lbl) {
    uint64_t out = 0;
    for(int i=0; i<sz; i++) {
        out <<= 1;
        out |= (in >> (64 - tbl[i])) & 1;
    }
    printf("  %-23s: %0*llx\n", lbl, sz/4, out);
    return out;
}

uint32_t shift_left(uint32_t val, int n) {
    return (val << n) | (val >> (28 - n));
}

void create_keys(uint64_t k, uint64_t *keys) {
    printf("\n=== Key Generation ===\n");
    uint64_t pc1 = apply_perm(k, KEY_PERM_1, 56, "PC-1 Step");
    
    uint32_t cl = (pc1 >> 28) & 0x0FFFFFFF;
    uint32_t cr = pc1 & 0x0FFFFFFF;

    for(int r=0; r<16; r++) {
        cl = shift_left(cl, SHIFT_AMT[r]);
        cr = shift_left(cr, SHIFT_AMT[r]);
        uint64_t merged = ((uint64_t)cl << 28) | cr;
        char lbl[32];
        snprintf(lbl, sizeof(lbl), "Round Key %02d", r+1);
        keys[r] = apply_perm(merged, KEY_PERM_2, 48, lbl);
    }
}

uint64_t process_block(uint64_t blk, uint64_t *keys, int rev) {
    printf("\n=== Processing Block ===\n");
    blk = apply_perm(blk, PERM_TABLE_1, 64, "Initial Permute");
    
    uint32_t lb = (blk >> 32) & 0xFFFFFFFF;
    uint32_t rb = blk & 0xFFFFFFFF;

    for(int rn=0; rn<16; rn++) {
        printf("\nRound %02d:\n", rn+1);
        uint32_t tmp = rb;
        
        uint64_t exp = apply_perm(rb, EXPAND_TABLE, 48, "Expansion Step");
        
        exp ^= keys[rev ? 15-rn : rn];
        show_output("After Key XOR", exp, 48);
        
        uint32_t s_out = 0;
        for(int s=0; s<8; s++) {
            int row = ((exp >> (42 - 6*s)) & 0x21) >> 4;
            int col = (exp >> (43 - 6*s)) & 0x0F;
            s_out = (s_out << 4) | SUBSTITUTION_BOXES[s][row][col];
        }
        show_output("S-Box Output", s_out, 32);
        
        uint32_t p_out = apply_perm(s_out, PERM_FUNC, 32, "P Permutation");
        
        rb = lb ^ p_out;
        lb = tmp;
        
        show_output("New Left Part", lb, 32);
        show_output("New Right Part", rb, 32);
    }
    
    uint64_t final = ((uint64_t)rb << 32) | lb;
    return apply_perm(final, PERM_TABLE_2, 64, "Final Permute");
}

void add_padding(uint8_t *d, int *l, int bs) {
    int pad = bs - (*l % bs);
    if(pad == 0) pad = bs;
    memset(d + *l, pad, pad);
    *l += pad;
}

void strip_padding(uint8_t *d, int *l) {
    int pad = d[*l-1];
    *l -= pad;
}

void encrypt_data(uint8_t *data, int len, uint64_t k, uint64_t iv, uint8_t *out) {
    uint64_t keys[16];
    create_keys(k, keys);
    
    uint64_t prev = iv;
    for(int i=0; i<len; i+=8) {
        printf("\n=== Encrypting Block %d/%d ===\n", (i/8)+1, len/8);
        uint64_t blk = 0;
        memcpy(&blk, data+i, 8);
        
        show_output("Input Block", blk, 64);
        show_output("XOR Step", prev, 64);
        
        blk ^= prev;
        blk = process_block(blk, keys, 0);
        memcpy(out+i, &blk, 8);
        prev = blk;
        
        show_output("Output Block", blk, 64);
    }
}

void decrypt_data(uint8_t *data, int len, uint64_t k, uint64_t iv, uint8_t *out) {
    uint64_t keys[16];
    create_keys(k, keys);
    
    uint64_t prev = iv;
    for(int i=0; i<len; i+=8) {
        printf("\n=== Decrypting Block %d/%d ===\n", (i/8)+1, len/8);
        uint64_t blk = 0;
        memcpy(&blk, data+i, 8);
        
        show_output("Cipher Block", blk, 64);
        uint64_t tmp = process_block(blk, keys, 1);
        tmp ^= prev;
        
        show_output("XOR Step", prev, 64);
        memcpy(out+i, &tmp, 8);
        prev = blk;
        
        show_output("Decrypted Block", tmp, 64);
    }
}

int main() {
    char user_input[1024];
    char key_str[17];
    char iv_str[17];
    
    printf("Enter message: ");
    fgets(user_input, sizeof(user_input), stdin);
    user_input[strcspn(user_input, "\n")] = 0;
    
    do {
        printf("Enter 16-char hex key: ");
        fgets(key_str, sizeof(key_str), stdin);
        key_str[strcspn(key_str, "\n")] = 0;
        flush_input();
        if(strlen(key_str) != 16 || !check_hex(key_str)) {
            printf("Invalid key! Must be 16 hex chars.\n");
        }
    } while(strlen(key_str) != 16 || !check_hex(key_str));
    
    do {
        printf("Enter 16-char hex IV: ");
        fgets(iv_str, sizeof(iv_str), stdin);
        iv_str[strcspn(iv_str, "\n")] = 0;
        flush_input();
        if(strlen(iv_str) != 16 || !check_hex(iv_str)) {
            printf("Invalid IV! Must be 16 hex chars.\n");
        }
    } while(strlen(iv_str) != 16 || !check_hex(iv_str));
    
    uint64_t k, iv;
    sscanf(key_str, "%16llx", &k);
    sscanf(iv_str, "%16llx", &iv);
    
    int len = strlen(user_input);
    int new_len = len + (8 - (len % 8));
    if(new_len == len) new_len += 8;
    uint8_t *padded_data = malloc(new_len);
    memcpy(padded_data, user_input, len);
    add_padding(padded_data, &len, 8);
    
    uint8_t *encrypted = malloc(new_len);
    printf("\n=== Encryption Process ===\n");
    encrypt_data(padded_data, new_len, k, iv, encrypted);
    
    printf("\nEncrypted Data (hex): ");
    for(int i=0; i<new_len; i++) printf("%02X", encrypted[i]);
    
    uint8_t *decrypted = malloc(new_len);
    printf("\n\n=== Decryption Process ===\n");
    decrypt_data(encrypted, new_len, k, iv, decrypted);
    strip_padding(decrypted, &new_len);
    
    printf("\nDecrypted Message: %.*s\n", new_len, decrypted);
    
    free(padded_data);
    free(encrypted);
    free(decrypted);
    return 0;
}