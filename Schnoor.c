#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <string.h>
#include <time.h>

#define MAX_MESSAGE_LEN 256

void SHA_256_hash(const char *input, mpz_t output, mpz_t n) {
    mpz_set_ui(output, 0);
    for (size_t i = 0; input[i] != '\0'; i++) {
        mpz_mul_ui(output, output, 37);   // Another small prime multiplier
        mpz_add_ui(output, output, (unsigned char)input[i]);  // Add character value
        mpz_mod(output, output, n);       // Ensure result fits within curve order n
    }
}

// Generate a random 256-bit hexadecimal number
void random_256bit_hex(mpz_t num) {
    char hex_str[65]; // 256 bits = 64 hex digits + 1 null terminator
    for (int i = 0; i < 64; i++) {
        sprintf(&hex_str[i], "%X", rand() % 16); // Random hex digit
    }
    hex_str[64] = '\0';
    mpz_set_str(num, hex_str, 16); // Convert hex string to GMP number
}

// Schnorr Signature Generation
void schnorr_sign(mpz_t R, mpz_t s, const char *message, mpz_t d, mpz_t n) {
    mpz_t k, e;
    mpz_inits(k, e, NULL);

    random_256bit_hex(k); // Generate random k
    mpz_mod(R, k, n);     // Compute R = k mod n

    SHA_256_hash(message, e, n); // Compute e = Hash(message)

    // Compute s = k + e * d (mod n)
    mpz_mul(s, e, d);
    mpz_add(s, s, k);
    mpz_mod(s, s, n);

    mpz_clears(k, e, NULL);
}

// Schnorr Signature Verification
int schnorr_verify(mpz_t R, mpz_t s, const char *message, mpz_t Q, mpz_t n) {
    mpz_t e, R_prime;
    mpz_inits(e, R_prime, NULL);

    SHA_256_hash(message, e, n); // Compute e = Hash(message)

    // Compute R' = s - e * Q (mod n)
    mpz_mul(R_prime, e, Q);
    mpz_sub(R_prime, s, R_prime);
    mpz_mod(R_prime, R_prime, n);

    int valid = (mpz_cmp(R_prime, R) == 0); // Check if R == R'

    mpz_clears(e, R_prime, NULL);
    return valid;
}

// Key generation
void schnorr_keygen(mpz_t d, mpz_t Q, mpz_t n) {
    random_256bit_hex(d); // Private key (256-bit random hex)
    mpz_mod(Q, d, n);     // Public key Q = d mod n (simplified)
}


int main() {
    srand(time(NULL)); // Seed random generator

    mpz_t n, d, Q, R, s;
    mpz_inits(n, d, Q, R, s, NULL);
    
    mpz_set_str(n, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F", 16);

    // Key generation
    schnorr_keygen(d, Q, n);

    char message[MAX_MESSAGE_LEN];
    printf("Enter the message for which you want to create Schnorr signature: ");
    fgets(message, MAX_MESSAGE_LEN, stdin);
    message[strcspn(message, "\n")] = 0; // Remove newline

    printf("\nPrivate Key: ");
    gmp_printf("%Zx\n", d);
    printf("Public Key: ");
    gmp_printf("%Zx\n", Q);

    // Sign the message
    schnorr_sign(R, s, message, d, n);
    printf("\nSignature (R): ");
    gmp_printf("%Zx\n", R);
    printf("Signature (s): ");
    gmp_printf("%Zx\n", s);

    // Verify the signature
    if (schnorr_verify(R, s, message, Q, n)) {
        printf("\nSignature is VALID \n");
    } else {
        printf("\nSignature is INVALID \n");
    }

    mpz_clears(n, d, Q, R, s, NULL);
    return 0;
}