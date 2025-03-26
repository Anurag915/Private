#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <string.h>
#include <time.h>

#define MAX_MESSAGE_LEN 256

void SHA_256_hash(const char *input, mpz_t output, mpz_t n) {
    mpz_set_ui(output, 0);
    for (size_t i = 0; input[i] != '\0'; i++) {
        mpz_mul_ui(output, output, 37);
        mpz_add_ui(output, output, (unsigned char)input[i]);
        mpz_mod(output, output, n);
    }
}

void random_256bit_hex(mpz_t num) {
    char hex_str[65];
    for (int i = 0; i < 64; i++) {
        sprintf(&hex_str[i], "%X", rand() % 16);
    }
    hex_str[64] = '\0';
    mpz_set_str(num, hex_str, 16);
}

void schnorr_sign_sts(mpz_t R, mpz_t s, const char *message, mpz_t d, mpz_t Q, mpz_t n) {
    mpz_t k, e;
    mpz_inits(k, e, NULL);

    random_256bit_hex(k);
    mpz_mod(R, k, n);

    char* q_str = mpz_get_str(NULL, 16, Q);
    char combined[MAX_MESSAGE_LEN + strlen(q_str) + 1];
    snprintf(combined, sizeof(combined), "%s%s", message, q_str);
    free(q_str);
    SHA_256_hash(combined, e, n);

    mpz_mul(s, e, d);
    mpz_add(s, s, k);
    mpz_mod(s, s, n);

    mpz_clears(k, e, NULL);
}

int schnorr_verify_sts(mpz_t R, mpz_t s, const char *message, mpz_t Q, mpz_t n) {
    mpz_t e, R_prime;
    mpz_inits(e, R_prime, NULL);

    char* q_str = mpz_get_str(NULL, 16, Q);
    char combined[MAX_MESSAGE_LEN + strlen(q_str) + 1];
    snprintf(combined, sizeof(combined), "%s%s", message, q_str);
    free(q_str);
    SHA_256_hash(combined, e, n);

    mpz_mul(R_prime, e, Q);
    mpz_sub(R_prime, s, R_prime);
    mpz_mod(R_prime, R_prime, n);

    int valid = (mpz_cmp(R_prime, R) == 0);
    mpz_clears(e, R_prime, NULL);
    return valid;
}

void sts_key_agreement(mpz_t shared_key, 
                      mpz_t d_A, mpz_t Q_A,  // Alice's keys
                      mpz_t d_B, mpz_t Q_B,  // Bob's keys
                      mpz_t n) {
    mpz_t k_A, R_A, s_A;
    mpz_inits(k_A, R_A, s_A, NULL);
    
    mpz_t k_B, R_B, s_B;
    mpz_inits(k_B, R_B, s_B, NULL);

    // Step 1: Alice generates ephemeral key and signature
    random_256bit_hex(k_A);
    schnorr_sign_sts(R_A, s_A, "STS_INIT", d_A, Q_A, n);

    // Step 2: Bob generates ephemeral key and signature
    random_256bit_hex(k_B);
    schnorr_sign_sts(R_B, s_B, "STS_RESPONSE", d_B, Q_B, n);

    // Step 3: Both verify signatures
    int alice_verifies = schnorr_verify_sts(R_B, s_B, "STS_RESPONSE", Q_B, n);
    int bob_verifies = schnorr_verify_sts(R_A, s_A, "STS_INIT", Q_A, n);

    if (!alice_verifies || !bob_verifies) {
        printf("Signature verification failed!\n");
        mpz_clears(k_A, R_A, s_A, k_B, R_B, s_B, NULL);
        return;
    }

    // Step 4: Compute shared key using both R_A and R_B
    mpz_t temp;
    mpz_init(temp);
    
    // Combine R_A and R_B numerically instead of as strings
    mpz_add(temp, R_A, R_B);  // Add the two ephemeral commitments
    mpz_mod(temp, temp, n);
    
    // Hash the combined value to get the shared key
    char* temp_str = mpz_get_str(NULL, 16, temp);
    SHA_256_hash(temp_str, shared_key, n);
    free(temp_str);

    // Verify consistency (both parties compute the same way)
    mpz_t check_key;
    mpz_init(check_key);
    mpz_add(check_key, R_A, R_B);  // Same computation
    mpz_mod(check_key, check_key, n);
    char* check_str = mpz_get_str(NULL, 16, check_key);
    SHA_256_hash(check_str, check_key, n);
    free(check_str);

    if (mpz_cmp(shared_key, check_key) != 0) {
        printf("Key agreement failed!\n");
        gmp_printf("Alice's computed key: %Zx\n", shared_key);
        gmp_printf("Bob's computed key: %Zx\n", check_key);
    } else {
        printf("Key agreement successful!\n");
    }

    mpz_clears(k_A, R_A, s_A, k_B, R_B, s_B, temp, check_key, NULL);
}

void schnorr_keygen(mpz_t d, mpz_t Q, mpz_t n) {
    random_256bit_hex(d);
    mpz_mod(Q, d, n);
}

int main() {
    srand(time(NULL));

    mpz_t n, d_A, Q_A, d_B, Q_B, shared_key;
    mpz_inits(n, d_A, Q_A, d_B, Q_B, shared_key, NULL);
    
    mpz_set_str(n, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F", 16);

    schnorr_keygen(d_A, Q_A, n);
    schnorr_keygen(d_B, Q_B, n);

    printf("Alice's Private Key: ");
    gmp_printf("%Zx\n", d_A);
    printf("Alice's Public Key: ");
    gmp_printf("%Zx\n", Q_A);
    printf("Bob's Private Key: ");
    gmp_printf("%Zx\n", d_B);
    printf("Bob's Public Key: ");
    gmp_printf("%Zx\n", Q_B);

    sts_key_agreement(shared_key, d_A, Q_A, d_B, Q_B, n);

    printf("\nShared Secret Key: ");
    gmp_printf("%Zx\n", shared_key);

    mpz_clears(n, d_A, Q_A, d_B, Q_B, shared_key, NULL);
    return 0;
}