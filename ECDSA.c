#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <stdbool.h>
#include <string.h>

// Structure to represent a point on the elliptic curve
typedef struct {
    mpz_t x;
    mpz_t y;
    bool is_infinity;
} ECPoint;

// Structure for the secp256k1 curve parameters
typedef struct {
    mpz_t p;  // Prime field modulus
    mpz_t a;  // Coefficient a
    mpz_t b;  // Coefficient b
    ECPoint G; // Generator point
    mpz_t n;  // Order of the generator
    mpz_t h;  // Cofactor
} Secp256k1;

// Function prototypes (from original code)
void init_curve(Secp256k1 *curve);
void clear_curve(Secp256k1 *curve);
void init_point(ECPoint *point);
void clear_point(ECPoint *point);
void set_point(ECPoint *result, const ECPoint *point);
void print_point(const ECPoint *point);
bool is_on_curve(const Secp256k1 *curve, const ECPoint *point);
void point_add(const Secp256k1 *curve, ECPoint *result, const ECPoint *p1, const ECPoint *p2);
void point_double(const Secp256k1 *curve, ECPoint *result, const ECPoint *p);
void scalar_multiply(const Secp256k1 *curve, ECPoint *result, const mpz_t k, const ECPoint *p);
void negate_point(const Secp256k1 *curve, ECPoint *result, const ECPoint *p);
void generate_keypair(const Secp256k1 *curve, mpz_t private_key, ECPoint *public_key);
void mod_inverse(mpz_t result, const mpz_t a, const mpz_t m);

// New ECDSA function prototypes
void mock_hash(mpz_t result, const char *message, const mpz_t n);
void ecdsa_sign(const Secp256k1 *curve, const mpz_t private_key, const char *message, mpz_t r, mpz_t s);
bool ecdsa_verify(const Secp256k1 *curve, const ECPoint *public_key, const char *message, const mpz_t r, const mpz_t s);

int main() {
    // Initialize the curve
    Secp256k1 curve;
    init_curve(&curve);
    
    // Generate a key pair
    mpz_t private_key;
    ECPoint public_key;
    mpz_init(private_key);
    init_point(&public_key);
    
    // Example private key (for demo only, in practice use random)
    mpz_set_str(private_key, "1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef", 16);
    generate_keypair(&curve, private_key, &public_key);
    
    printf("Private key: %s\n", mpz_get_str(NULL, 16, private_key));
    printf("Public key: ");
    print_point(&public_key);
    
    // Message to sign
    const char *message = "IIITDM Kurnool";
    printf("Message: %s\n", message);
    
    // Sign the message
    mpz_t r, s;
    mpz_init(r);
    mpz_init(s);
    ecdsa_sign(&curve, private_key, message, r, s);
    
    printf("Signature (r, s):\n");
    printf("r = %s\n", mpz_get_str(NULL, 16, r));
    printf("s = %s\n", mpz_get_str(NULL, 16, s));
    
    // Verify the signature
    bool valid = ecdsa_verify(&curve, &public_key, message, r, s);
    printf("Signature valid: %s\n", valid ? "yes" : "no");
    
    // Test with tampered message
    const char *tampered_message = "IIITDMK";
    bool tampered_valid = ecdsa_verify(&curve, &public_key, tampered_message, r, s);
    printf("Tampered message signature valid: %s\n", tampered_valid ? "yes" : "no");
    
    // Cleanup
    mpz_clear(private_key);
    clear_point(&public_key);
    mpz_clear(r);
    mpz_clear(s);
    clear_curve(&curve);
    
    return 0;
}

// --- Original Functions (unchanged, included for completeness) ---

void init_curve(Secp256k1 *curve) {
    mpz_init(curve->p);
    mpz_init(curve->a);
    mpz_init(curve->b);
    mpz_init(curve->n);
    mpz_init(curve->h);
    init_point(&curve->G);
    
    mpz_set_str(curve->p, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F", 16);
    mpz_set_ui(curve->a, 0);
    mpz_set_ui(curve->b, 7);
    mpz_set_str(curve->G.x, "79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798", 16);
    mpz_set_str(curve->G.y, "483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8", 16);
    curve->G.is_infinity = false;
    mpz_set_str(curve->n, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141", 16);
    mpz_set_ui(curve->h, 1);
}

void clear_curve(Secp256k1 *curve) {
    mpz_clear(curve->p);
    mpz_clear(curve->a);
    mpz_clear(curve->b);
    mpz_clear(curve->n);
    mpz_clear(curve->h);
    clear_point(&curve->G);
}

void init_point(ECPoint *point) {
    mpz_init(point->x);
    mpz_init(point->y);
    point->is_infinity = true;
}

void clear_point(ECPoint *point) {
    mpz_clear(point->x);
    mpz_clear(point->y);
}

void set_point(ECPoint *result, const ECPoint *point) {
    if (point->is_infinity) {
        result->is_infinity = true;
    } else {
        mpz_set(result->x, point->x);
        mpz_set(result->y, point->y);
        result->is_infinity = false;
    }
}

void print_point(const ECPoint *point) {
    if (point->is_infinity) {
        printf("(infinity)\n");
    } else {
        char x_str[200], y_str[200];
        mpz_get_str(x_str, 16, point->x);
        mpz_get_str(y_str, 16, point->y);
        printf("(%s, %s)\n", x_str, y_str);
    }
}

bool is_on_curve(const Secp256k1 *curve, const ECPoint *point) {
    if (point->is_infinity) return true;
    mpz_t left, right, temp;
    mpz_init(left);
    mpz_init(right);
    mpz_init(temp);
    mpz_powm_ui(left, point->y, 2, curve->p);
    mpz_powm_ui(right, point->x, 3, curve->p);
    mpz_mul(temp, curve->a, point->x);
    mpz_mod(temp, temp, curve->p);
    mpz_add(right, right, temp);
    mpz_mod(right, right, curve->p);
    mpz_add(right, right, curve->b);
    mpz_mod(right, right, curve->p);
    bool result = (mpz_cmp(left, right) == 0);
    mpz_clear(left);
    mpz_clear(right);
    mpz_clear(temp);
    return result;
}

void point_add(const Secp256k1 *curve, ECPoint *result, const ECPoint *p1, const ECPoint *p2) {
    if (p1->is_infinity) { set_point(result, p2); return; }
    if (p2->is_infinity) { set_point(result, p1); return; }
    mpz_t neg_y; mpz_init(neg_y);
    mpz_sub(neg_y, curve->p, p2->y);
    mpz_mod(neg_y, neg_y, curve->p);
    if (mpz_cmp(p1->x, p2->x) == 0 && mpz_cmp(p1->y, neg_y) == 0) {
        result->is_infinity = true;
        mpz_clear(neg_y);
        return;
    }
    mpz_clear(neg_y);
    if (mpz_cmp(p1->x, p2->x) == 0 && mpz_cmp(p1->y, p2->y) == 0) {
        point_double(curve, result, p1);
        return;
    }
    mpz_t slope, temp1, temp2;
    mpz_init(slope); mpz_init(temp1); mpz_init(temp2);
    mpz_sub(temp1, p2->y, p1->y); mpz_mod(temp1, temp1, curve->p);
    mpz_sub(temp2, p2->x, p1->x); mpz_mod(temp2, temp2, curve->p);
    mod_inverse(slope, temp2, curve->p);
    mpz_mul(slope, slope, temp1); mpz_mod(slope, slope, curve->p);
    mpz_powm_ui(result->x, slope, 2, curve->p);
    mpz_sub(result->x, result->x, p1->x);
    mpz_sub(result->x, result->x, p2->x);
    mpz_mod(result->x, result->x, curve->p);
    mpz_sub(temp1, p1->x, result->x);
    mpz_mul(temp1, slope, temp1);
    mpz_sub(temp1, temp1, p1->y);
    mpz_mod(result->y, temp1, curve->p);
    result->is_infinity = false;
    mpz_clear(slope); mpz_clear(temp1); mpz_clear(temp2);
}

void point_double(const Secp256k1 *curve, ECPoint *result, const ECPoint *p) {
    if (p->is_infinity || mpz_sgn(p->y) == 0) {
        result->is_infinity = true;
        return;
    }
    mpz_t slope, temp1, temp2;
    mpz_init(slope); mpz_init(temp1); mpz_init(temp2);
    mpz_powm_ui(temp1, p->x, 2, curve->p);
    mpz_mul_ui(temp1, temp1, 3);
    mpz_add(temp1, temp1, curve->a);
    mpz_mod(temp1, temp1, curve->p);
    mpz_mul_ui(temp2, p->y, 2);
    mpz_mod(temp2, temp2, curve->p);
    mod_inverse(slope, temp2, curve->p);
    mpz_mul(slope, slope, temp1);
    mpz_mod(slope, slope, curve->p);
    mpz_powm_ui(result->x, slope, 2, curve->p);
    mpz_mul_ui(temp1, p->x, 2);
    mpz_sub(result->x, result->x, temp1);
    mpz_mod(result->x, result->x, curve->p);
    mpz_sub(temp1, p->x, result->x);
    mpz_mul(temp1, slope, temp1);
    mpz_sub(temp1, temp1, p->y);
    mpz_mod(result->y, temp1, curve->p);
    result->is_infinity = false;
    mpz_clear(slope); mpz_clear(temp1); mpz_clear(temp2);
}

void scalar_multiply(const Secp256k1 *curve, ECPoint *result, const mpz_t k, const ECPoint *p) {
    if (mpz_sgn(k) == 0 || p->is_infinity) {
        result->is_infinity = true;
        return;
    }
    if (mpz_sgn(k) < 0) {
        mpz_t positive_k; mpz_init(positive_k); mpz_neg(positive_k, k);
        ECPoint negated_p; init_point(&negated_p);
        negate_point(curve, &negated_p, p);
        scalar_multiply(curve, result, positive_k, &negated_p);
        clear_point(&negated_p);
        mpz_clear(positive_k);
        return;
    }
    result->is_infinity = true;
    ECPoint addend; init_point(&addend); set_point(&addend, p);
    for (int i = mpz_sizeinbase(k, 2) - 1; i >= 0; i--) {
        ECPoint doubled; init_point(&doubled);
        point_double(curve, &doubled, result);
        set_point(result, &doubled);
        clear_point(&doubled);
        if (mpz_tstbit(k, i)) {
            ECPoint added; init_point(&added);
            point_add(curve, &added, result, &addend);
            set_point(result, &added);
            clear_point(&added);
        }
    }
    clear_point(&addend);
}

void negate_point(const Secp256k1 *curve, ECPoint *result, const ECPoint *p) {
    if (p->is_infinity) { result->is_infinity = true; return; }
    mpz_set(result->x, p->x);
    mpz_sub(result->y, curve->p, p->y);
    mpz_mod(result->y, result->y, curve->p);
    result->is_infinity = false;
}

void mod_inverse(mpz_t result, const mpz_t a, const mpz_t m) {
    mpz_t gcd, s, t, s1, s2, t1, t2, q, temp;
    mpz_init(gcd); mpz_init(s); mpz_init(t);
    mpz_init_set_ui(s1, 1); mpz_init_set_ui(s2, 0);
    mpz_init_set_ui(t1, 0); mpz_init_set_ui(t2, 1);
    mpz_init(q); mpz_init(temp);
    mpz_t a_temp, m_temp;
    mpz_init_set(a_temp, a); mpz_init_set(m_temp, m);
    while (mpz_cmp_ui(m_temp, 0) > 0) {
        mpz_fdiv_q(q, a_temp, m_temp);
        mpz_set(temp, a_temp); mpz_set(a_temp, m_temp);
        mpz_mul(m_temp, q, m_temp); mpz_sub(m_temp, temp, m_temp);
        mpz_set(temp, s1); mpz_set(s1, s2);
        mpz_mul(s2, q, s2); mpz_sub(s2, temp, s2);
        mpz_set(temp, t1); mpz_set(t1, t2);
        mpz_mul(t2, q, t2); mpz_sub(t2, temp, t2);
    }
    mpz_set(gcd, a_temp); mpz_set(s, s1); mpz_set(t, t1);
    if (mpz_cmp_ui(gcd, 1) != 0) {
        fprintf(stderr, "Error: modular inverse does not exist\n");
        exit(EXIT_FAILURE);
    }
    if (mpz_sgn(s) < 0) mpz_add(s, s, m);
    mpz_set(result, s);
    mpz_clear(gcd); mpz_clear(s); mpz_clear(t);
    mpz_clear(s1); mpz_clear(s2); mpz_clear(t1); mpz_clear(t2);
    mpz_clear(q); mpz_clear(temp); mpz_clear(a_temp); mpz_clear(m_temp);
}

void generate_keypair(const Secp256k1 *curve, mpz_t private_key, ECPoint *public_key) {
    if (mpz_cmp_ui(private_key, 0) <= 0 || mpz_cmp(private_key, curve->n) >= 0) {
        fprintf(stderr, "Error: private key must be in [1, n-1]\n");
        exit(EXIT_FAILURE);
    }
    scalar_multiply(curve, public_key, private_key, &curve->G);
}

// --- New ECDSA Functions ---

// Mock hash function (simplified for demo, in practice use SHA-256)
void mock_hash(mpz_t result, const char *message, const mpz_t n) {
    mpz_t temp;
    mpz_init_set_ui(temp, 0); // Initialize temp to 0
    // Convert message to an integer using a more robust approach
    unsigned int hash = 0;
    for (int i = 0; message[i]; i++) {
        hash = (hash * 31) + (unsigned char)message[i];
        mpz_mul_ui(temp, temp, 31);
        mpz_add_ui(temp, temp, (unsigned char)message[i]);
    }
    mpz_mod(result, temp, n); // Ensure result is in [0, n-1]
    mpz_clear(temp);
}

// Generate an ECDSA signature (r, s)
void ecdsa_sign(const Secp256k1 *curve, const mpz_t private_key, const char *message, mpz_t r, mpz_t s) {
    mpz_t k, z, k_inv, temp;
    ECPoint kG;
    mpz_init(k); mpz_init(z); mpz_init(k_inv); mpz_init(temp);
    init_point(&kG);
    
    // Compute message hash
    mock_hash(z, message, curve->n);
    
    // In practice, k should be cryptographically random and unique per signature
    // Here, we use a fixed value for reproducibility
    mpz_set_str(k, "abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890", 16);
    if (mpz_cmp_ui(k, 0) <= 0 || mpz_cmp(k, curve->n) >= 0) {
        fprintf(stderr, "Error: k must be in [1, n-1]\n");
        exit(EXIT_FAILURE);
    }
    
    // Compute k * G
    scalar_multiply(curve, &kG, k, &curve->G);
    
    // r = x-coordinate of kG mod n
    mpz_mod(r, kG.x, curve->n);
    if (mpz_cmp_ui(r, 0) == 0) {
        fprintf(stderr, "Error: r = 0, choose a different k\n");
        exit(EXIT_FAILURE);
    }
    
    // Compute k^(-1) mod n
    mod_inverse(k_inv, k, curve->n);
    
    // s = k^(-1) * (z + r * private_key) mod n
    mpz_mul(temp, r, private_key);
    mpz_mod(temp, temp, curve->n);
    mpz_add(temp, z, temp);
    mpz_mod(temp, temp, curve->n);
    mpz_mul(s, k_inv, temp);
    mpz_mod(s, s, curve->n);
    
    if (mpz_cmp_ui(s, 0) == 0) {
        fprintf(stderr, "Error: s = 0, choose a different k\n");
        exit(EXIT_FAILURE);
    }
    
    // Cleanup
    mpz_clear(k); mpz_clear(z); mpz_clear(k_inv); mpz_clear(temp);
    clear_point(&kG);
}

// Verify an ECDSA signature (r, s)
bool ecdsa_verify(const Secp256k1 *curve, const ECPoint *public_key, const char *message, const mpz_t r, const mpz_t s) {
    // Check r and s are in [1, n-1]
    if (mpz_cmp_ui(r, 0) <= 0 || mpz_cmp(r, curve->n) >= 0 ||
        mpz_cmp_ui(s, 0) <= 0 || mpz_cmp(s, curve->n) >= 0) {
        return false;
    }
    
    mpz_t z, w, u1, u2;
    ECPoint u1G, u2Q, R;
    mpz_init(z); mpz_init(w); mpz_init(u1); mpz_init(u2);
    init_point(&u1G); init_point(&u2Q); init_point(&R);
    
    // Compute message hash
    mock_hash(z, message, curve->n);
    
    // w = s^(-1) mod n
    mod_inverse(w, s, curve->n);
    
    // u1 = z * w mod n
    mpz_mul(u1, z, w);
    mpz_mod(u1, u1, curve->n);
    
    // u2 = r * w mod n
    mpz_mul(u2, r, w);
    mpz_mod(u2, u2, curve->n);
    
    // Compute R = u1 * G + u2 * Q
    scalar_multiply(curve, &u1G, u1, &curve->G);
    scalar_multiply(curve, &u2Q, u2, public_key);
    point_add(curve, &R, &u1G, &u2Q);
    
    // If R is infinity, signature is invalid
    if (R.is_infinity) {
        mpz_clear(z); mpz_clear(w); mpz_clear(u1); mpz_clear(u2);
        clear_point(&u1G); clear_point(&u2Q); clear_point(&R);
        return false;
    }
    
    // Check if r == x-coordinate of R mod n
    mpz_mod(R.x, R.x, curve->n);
    bool valid = (mpz_cmp(r, R.x) == 0);
    
    // Cleanup
    mpz_clear(z); mpz_clear(w); mpz_clear(u1); mpz_clear(u2);
    clear_point(&u1G); clear_point(&u2Q); clear_point(&R);
    
    return valid;
}
