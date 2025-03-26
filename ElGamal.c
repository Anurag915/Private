#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include <time.h>


typedef struct {
    mpz_t p, a, b, Gx, Gy, n;
} CurveParams;


void init_secp256k1(CurveParams *curve) {
    mpz_inits(curve->p, curve->a, curve->b, curve->Gx, curve->Gy, curve->n, NULL);
    mpz_set_str(curve->p, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F", 16);
    mpz_set_str(curve->a, "0", 16);
    mpz_set_str(curve->b, "7", 16);
    mpz_set_str(curve->Gx, "79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798", 16);
    mpz_set_str(curve->Gy, "483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8", 16);
    mpz_set_str(curve->n, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141", 16);
}


void mod_inverse(mpz_t result, mpz_t value, mpz_t p) {
    if (mpz_invert(result, value, p) == 0) {
        mpz_set_ui(result, 0);
    }
}


void point_add(mpz_t x_res, mpz_t y_res, mpz_t x1, mpz_t y1, mpz_t x2, mpz_t y2, mpz_t a, mpz_t p){
if (mpz_cmp(x1, x2) == 0 && mpz_cmp(y1, y2) == 0) {

        mpz_t lambda, num, denom, denom_inv;
        mpz_inits(lambda, num, denom, denom_inv, NULL);


        mpz_mul(num, x1, x1);      // x^2
        mpz_mul_ui(num, num, 3);   // 3*x^2
        mpz_add(num, num, a);      // 3*x^2 + a
        mpz_mod(num, num, p);      // num mod p


        mpz_mul_ui(denom, y1, 2);
        mpz_mod(denom, denom, p);


        mod_inverse(denom_inv, denom, p);

        // lambda = num * denom^-1 mod p
        mpz_mul(lambda, num, denom_inv);
        mpz_mod(lambda, lambda, p);


        mpz_mul(x_res, lambda, lambda);
        mpz_sub(x_res, x_res, x1);
        mpz_sub(x_res, x_res, x1);
        mpz_mod(x_res, x_res, p);


        mpz_sub(y_res, x1, x_res);
        mpz_mul(y_res, lambda, y_res);
        mpz_sub(y_res, y_res, y1);
        mpz_mod(y_res, y_res, p);

        mpz_clears(lambda, num, denom, denom_inv, NULL);
    } else {

        mpz_t lambda, num, denom, denom_inv;
        mpz_inits(lambda, num, denom, denom_inv, NULL);


        mpz_sub(num, y2, y1);
        mpz_mod(num, num, p);


        mpz_sub(denom, x2, x1);
        mpz_mod(denom, denom, p);


        mod_inverse(denom_inv, denom, p);


        mpz_mul(lambda, num, denom_inv);
        mpz_mod(lambda, lambda, p);


        mpz_mul(x_res, lambda, lambda);
        mpz_sub(x_res, x_res, x1);
        mpz_sub(x_res, x_res, x2);
        mpz_mod(x_res, x_res, p);


        mpz_sub(y_res, x1, x_res);
        mpz_mul(y_res, lambda, y_res);
        mpz_sub(y_res, y_res, y1);
        mpz_mod(y_res, y_res, p);

        mpz_clears(lambda, num, denom, denom_inv, NULL);
    }}


void scalar_mult(mpz_t x_res, mpz_t y_res, mpz_t x1, mpz_t y1, mpz_t k, mpz_t a, mpz_t p){
  mpz_t x_temp, y_temp, x_Q, y_Q;
    mpz_inits(x_temp, y_temp, x_Q, y_Q, NULL);


    mpz_set(x_Q, x1);
    mpz_set(y_Q, y1);


    int is_infinity = 1;


    for (int i = mpz_sizeinbase(k, 2) - 1; i >= 0; i--) {
        if (!is_infinity) {
            point_add(x_temp, y_temp, x_res, y_res, x_res, y_res, a, p); // R = 2R
            mpz_set(x_res, x_temp);
            mpz_set(y_res, y_temp);
        }

        if (mpz_tstbit(k, i)) {
            if (is_infinity) {  
                mpz_set(x_res, x_Q);
                mpz_set(y_res, y_Q);
                is_infinity = 0;
            } else { 
                point_add(x_temp, y_temp, x_res, y_res, x_Q, y_Q, a, p);
                mpz_set(x_res, x_temp);
                mpz_set(y_res, y_temp);
            }
        }
    }


    mpz_clears(x_temp, y_temp, x_Q, y_Q, NULL);
}


void key_generation(mpz_t priv_key, mpz_t pub_x, mpz_t pub_y, CurveParams *curve) {
    gmp_randstate_t state;
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, time(NULL));

    mpz_urandomm(priv_key, state, curve->n);    scalar_mult(pub_x, pub_y, curve->Gx, curve->Gy, priv_key, curve->a, curve->p);
}


void encrypt(mpz_t C1x, mpz_t C1y, mpz_t C2x, mpz_t C2y, mpz_t Mx, mpz_t My, mpz_t pub_x, mpz_t pub_y, CurveParams *curve) {
    mpz_t k, temp_x, temp_y;
    mpz_inits(k, temp_x, temp_y, NULL);

    gmp_randstate_t state;
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, time(NULL));
    mpz_urandomm(k, state, curve->n);

    scalar_mult(C1x, C1y, curve->Gx, curve->Gy, k, curve->a, curve->p); // C1 = kG
    scalar_mult(temp_x, temp_y, pub_x, pub_y, k, curve->a, curve->p);   // kY
    point_add(C2x, C2y, Mx, My, temp_x, temp_y, curve->a, curve->p);     // C2 = M + kY

    mpz_clears(k, temp_x, temp_y, NULL);
}

// Decryption: M = C2 - dC1
void decrypt(mpz_t Mx, mpz_t My, mpz_t C1x, mpz_t C1y, mpz_t C2x, mpz_t C2y, mpz_t priv_key, CurveParams *curve) {
    mpz_t temp_x, temp_y, neg_temp_y;
    mpz_inits(temp_x, temp_y, neg_temp_y, NULL);

    scalar_mult(temp_x, temp_y, C1x, C1y, priv_key, curve->a, curve->p); 
    mpz_neg(neg_temp_y, temp_y);  
    mpz_mod(neg_temp_y, neg_temp_y, curve->p);
    point_add(Mx, My, C2x, C2y, temp_x, neg_temp_y, curve->a, curve->p); 
    mpz_clears(temp_x, temp_y, neg_temp_y, NULL);
}

int main() {
    CurveParams curve;
    init_secp256k1(&curve);
    
    mpz_t priv_key, pub_x, pub_y;
    mpz_inits(priv_key, pub_x, pub_y, NULL);

  
    key_generation(priv_key, pub_x, pub_y, &curve);
    gmp_printf("Private Key: %Zx\n", priv_key);
    gmp_printf("Public Key: (%Zx, %Zx)\n", pub_x, pub_y);


    mpz_t Mx, My;
    mpz_init_set(Mx, curve.Gx);
    mpz_init_set(My, curve.Gy);


    mpz_t C1x, C1y, C2x, C2y;
    mpz_inits(C1x, C1y, C2x, C2y, NULL);
    encrypt(C1x, C1y, C2x, C2y, Mx, My, pub_x, pub_y, &curve);
    gmp_printf("Encrypted: C1 = (%Zx, %Zx), C2 = (%Zx, %Zx)\n", C1x, C1y, C2x, C2y);


    mpz_t Mx_dec, My_dec;
    mpz_inits(Mx_dec, My_dec, NULL);
    decrypt(Mx_dec, My_dec, C1x, C1y, C2x, C2y, priv_key, &curve);
    gmp_printf("Decrypted Message: (%Zx, %Zx)\n", Mx_dec, My_dec);

    mpz_clears(priv_key, pub_x, pub_y, Mx, My, C1x, C1y, C2x, C2y, Mx_dec, My_dec, NULL);
    return 0;
}

