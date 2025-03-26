#include <stdio.h>
#include <gmp.h>

// Function to compute modular inverse
void mod_inverse(mpz_t result, mpz_t value, mpz_t p) {
    if (mpz_invert(result, value, p) == 0) {
        printf("Modular inverse does not exist!\n");
        mpz_set_ui(result, 0);
    }
}

// Function to perform point addition (P + Q)
void point_add(mpz_t x_res, mpz_t y_res, mpz_t x1, mpz_t y1, mpz_t x2, mpz_t y2, mpz_t a, mpz_t p) {
    if (mpz_cmp(x1, x2) == 0 && mpz_cmp(y1, y2) == 0) {
        // Point doubling case
        mpz_t lambda, num, denom, denom_inv;
        mpz_inits(lambda, num, denom, denom_inv, NULL);

        // num = 3*x^2 + a
        mpz_mul(num, x1, x1);      // x^2
        mpz_mul_ui(num, num, 3);   // 3*x^2
        mpz_add(num, num, a);      // 3*x^2 + a
        mpz_mod(num, num, p);      // num mod p

        // denom = 2*y
        mpz_mul_ui(denom, y1, 2);
        mpz_mod(denom, denom, p);

        // Compute modular inverse of denom
        mod_inverse(denom_inv, denom, p);

        // lambda = num * denom^-1 mod p
        mpz_mul(lambda, num, denom_inv);
        mpz_mod(lambda, lambda, p);

        // x3 = lambda^2 - 2*x1 mod p
        mpz_mul(x_res, lambda, lambda);
        mpz_sub(x_res, x_res, x1);
        mpz_sub(x_res, x_res, x1);
        mpz_mod(x_res, x_res, p);

        // y3 = lambda * (x1 - x3) - y1 mod p
        mpz_sub(y_res, x1, x_res);
        mpz_mul(y_res, lambda, y_res);
        mpz_sub(y_res, y_res, y1);
        mpz_mod(y_res, y_res, p);

        mpz_clears(lambda, num, denom, denom_inv, NULL);
    } else {
        // Regular point addition case
        mpz_t lambda, num, denom, denom_inv;
        mpz_inits(lambda, num, denom, denom_inv, NULL);

        // num = y2 - y1
        mpz_sub(num, y2, y1);
        mpz_mod(num, num, p);

        // denom = x2 - x1
        mpz_sub(denom, x2, x1);
        mpz_mod(denom, denom, p);

        // Compute modular inverse of denom
        mod_inverse(denom_inv, denom, p);

        // lambda = num * denom^-1 mod p
        mpz_mul(lambda, num, denom_inv);
        mpz_mod(lambda, lambda, p);

        // x3 = lambda^2 - x1 - x2 mod p
        mpz_mul(x_res, lambda, lambda);
        mpz_sub(x_res, x_res, x1);
        mpz_sub(x_res, x_res, x2);
        mpz_mod(x_res, x_res, p);

        // y3 = lambda * (x1 - x3) - y1 mod p
        mpz_sub(y_res, x1, x_res);
        mpz_mul(y_res, lambda, y_res);
        mpz_sub(y_res, y_res, y1);
        mpz_mod(y_res, y_res, p);

        mpz_clears(lambda, num, denom, denom_inv, NULL);
    }
}

// Function for scalar multiplication kP
// Function for scalar multiplication kP using double-and-add method
void scalar_mult(mpz_t x_res, mpz_t y_res, mpz_t x1, mpz_t y1, mpz_t k, mpz_t a, mpz_t p) {
    // Initialize temporary variables
    mpz_t x_temp, y_temp, x_Q, y_Q;
    mpz_inits(x_temp, y_temp, x_Q, y_Q, NULL);

    // Set Q = P (the initial point)
    mpz_set(x_Q, x1);
    mpz_set(y_Q, y1);

    // Set result to point at infinity initially
    int is_infinity = 1;

    // Iterate through each bit of scalar k (MSB to LSB)
    for (int i = mpz_sizeinbase(k, 2) - 1; i >= 0; i--) {
        if (!is_infinity) {
            point_add(x_temp, y_temp, x_res, y_res, x_res, y_res, a, p); // R = 2R
            mpz_set(x_res, x_temp);
            mpz_set(y_res, y_temp);
        }

        if (mpz_tstbit(k, i)) {
            if (is_infinity) {  // First nonzero bit, initialize R = Q
                mpz_set(x_res, x_Q);
                mpz_set(y_res, y_Q);
                is_infinity = 0;
            } else {  // Add Q to the current result
                point_add(x_temp, y_temp, x_res, y_res, x_Q, y_Q, a, p);
                mpz_set(x_res, x_temp);
                mpz_set(y_res, y_temp);
            }
        }
    }

    // Clear temporary variables
    mpz_clears(x_temp, y_temp, x_Q, y_Q, NULL);
}

int main() {
    mpz_t a, b, p, x1, y1, x2, y2, x_res, y_res, k;
    mpz_inits(a, b, p, x1, y1, x2, y2, x_res, y_res, k, NULL);

    // Input: a, b, p
    printf("Enter elliptic curve parameters (a, b, prime p in HEX): ");
    gmp_scanf("%Zx %Zx %Zx", a, b, p);

    // Input: First point P (x1, y1)
    printf("Enter first point P (x1, y1) in HEX: ");
    gmp_scanf("%Zx %Zx", x1, y1);

    // Input: Second point Q (x2, y2)
    printf("Enter second point Q (x2, y2) in HEX: ");
    gmp_scanf("%Zx %Zx", x2, y2);

    // Point addition P + Q
    point_add(x_res, y_res, x1, y1, x2, y2, a, p);
    gmp_printf("P + Q = (%Zx, %Zx)\n", x_res, y_res);

    // Input: Scalar k
    printf("Enter scalar k for multiplication (HEX): ");
    gmp_scanf("%Zx", k);

    // Scalar multiplication kP
    scalar_mult(x_res, y_res, x1, y1, k, a, p);
    gmp_printf("Scalar multiplication kP = (%Zx, %Zx)\n", x_res, y_res);

    mpz_clears(a, b, p, x1, y1, x2, y2, x_res, y_res, k, NULL);
    return 0;
}

