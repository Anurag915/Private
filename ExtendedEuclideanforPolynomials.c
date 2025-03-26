#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int degree;
    int *coefficients;
} Poly;

Poly* create_poly(int degree) {
    Poly *p = (Poly*)malloc(sizeof(Poly));
    p->degree = degree;
    p->coefficients = (int*)calloc((degree + 1), sizeof(int));
    return p;
}

void free_poly(Poly *p) {
    if (p) {
        free(p->coefficients);
        free(p);
    }
}

int compute_degree(const Poly *p) {
    int degree = -1;
    for (int i = 0; i <= p->degree; i++) {
        if (p->coefficients[i] != 0) {
            degree = i;
        }
    }
    return degree;
}

Poly* copy_poly(const Poly *src) {
    Poly *dest = create_poly(src->degree);
    memcpy(dest->coefficients, src->coefficients, (src->degree + 1) * sizeof(int));
    dest->degree = compute_degree(dest);
    return dest;
}

Poly* shift_poly(const Poly *p, int k) {
    if (k < 0) return NULL;

    int p_degree = compute_degree(p);
    if (p_degree == -1) return create_poly(0);

    Poly *result = create_poly(p_degree + k);
    for (int i = 0; i <= p_degree; i++) {
        result->coefficients[i + k] = p->coefficients[i];
    }
    result->degree = compute_degree(result);
    return result;
}

void subtract_poly(Poly *a, const Poly *b) {
    int max_degree = (a->degree > b->degree) ? a->degree : b->degree;
    for (int i = 0; i <= max_degree; i++) {
        a->coefficients[i] ^= (i <= b->degree) ? b->coefficients[i] : 0;
    }
    a->degree = compute_degree(a);
}

Poly* multiply_poly(const Poly *a, const Poly *b) {
    int a_degree = compute_degree(a);
    int b_degree = compute_degree(b);
    
    if (a_degree == -1 || b_degree == -1) return create_poly(0);

    Poly *result = create_poly(a_degree + b_degree);
    for (int i = 0; i <= a_degree; i++) {
        if (a->coefficients[i] == 0) continue;
        for (int j = 0; j <= b_degree; j++) {
            if (b->coefficients[j] == 0) continue;
            result->coefficients[i + j] ^= 1;
        }
    }
    result->degree = compute_degree(result);
    return result;
}

Poly* add_poly(const Poly *a, const Poly *b) {
    int max_degree = (a->degree > b->degree) ? a->degree : b->degree;
    Poly *result = create_poly(max_degree);

    for (int i = 0; i <= max_degree; i++) {
        int coeff_a = (i <= a->degree) ? a->coefficients[i] : 0;
        int coeff_b = (i <= b->degree) ? b->coefficients[i] : 0;
        result->coefficients[i] = coeff_a ^ coeff_b;
    }
    result->degree = compute_degree(result);
    return result;
}

void divide_poly(const Poly *f, const Poly *g, Poly **q, Poly **r) {
    int f_degree = compute_degree(f);
    int g_degree = compute_degree(g);

    if (g_degree == -1) {
        fprintf(stderr, "Error: Division by zero polynomial\n");
        exit(1);
    }

    if (f_degree < g_degree) {
        *q = create_poly(0);
        *r = copy_poly(f);
        return;
    }

    *r = copy_poly(f);
    *q = create_poly(f_degree - g_degree);

    while (compute_degree(*r) >= g_degree) {
        int deg_diff = compute_degree(*r) - g_degree;
        (*q)->coefficients[deg_diff] ^= 1;

        Poly *term = shift_poly(g, deg_diff);
        subtract_poly(*r, term);
        free_poly(term);
    }

    (*q)->degree = compute_degree(*q);
    (*r)->degree = compute_degree(*r);
}

Poly* extended_gcd_poly(const Poly *a, const Poly *m) {
    Poly *old_r = copy_poly(a);
    Poly *r = copy_poly(m);
    Poly *old_s = create_poly(0);
    old_s->coefficients[0] = 1;
    old_s->degree = compute_degree(old_s);
    Poly *s = create_poly(0);
    Poly *old_t = create_poly(0);
    Poly *t = create_poly(0);
    t->coefficients[0] = 1;
    t->degree = compute_degree(t);

    while (compute_degree(r) != -1) {
        Poly *quotient = NULL;
        Poly *remainder = NULL;
        divide_poly(old_r, r, &quotient, &remainder);

        Poly *temp_r = old_r;
        old_r = r;
        r = remainder;

        Poly *temp_s = old_s;
        Poly *prod = multiply_poly(quotient, s);
        old_s = s;
        s = add_poly(temp_s, prod);

        Poly *temp_t = old_t;
        Poly *prod_t = multiply_poly(quotient, t);
        old_t = t;
        t = add_poly(temp_t, prod_t);

        free_poly(temp_r);
        free_poly(temp_s);
        free_poly(temp_t);
        free_poly(prod);
        free_poly(prod_t);
        free_poly(quotient);
    }

    Poly *inverse = NULL;
    if (compute_degree(old_r) == 0 && old_r->coefficients[0] == 1) {
        inverse = copy_poly(old_s);
    } else {
        printf("Inverse does not exist\n");
        exit(1);
    }

    free_poly(old_r);
    free_poly(r);
    free_poly(old_s);
    free_poly(s);
    free_poly(old_t);
    free_poly(t);

    return inverse;
}

void input_poly(Poly *p) {
    printf("Enter degree of polynomial: ");
    scanf("%d", &p->degree);

    p->coefficients = (int*)calloc((p->degree + 1), sizeof(int));
    printf("Enter coefficients (space-separated): ");
    for (int i = 0; i <= p->degree; i++) {
        scanf("%d", &p->coefficients[i]);
    }
    p->degree = compute_degree(p);
}

void print_poly(const Poly *p) {
    for (int i = 0; i <= p->degree; i++) {
        printf("%d ", p->coefficients[i]);
    }
    printf("\n");
}

int main() {
    Poly *a = create_poly(0);
    Poly *m = create_poly(0);

    printf("Enter polynomial a:\n");
    input_poly(a);

    printf("Enter modulus polynomial m:\n");
    input_poly(m);

    Poly *inverse = extended_gcd_poly(a, m);

    printf("Modular Inverse: ");
    print_poly(inverse);

    free_poly(a);
    free_poly(m);
    free_poly(inverse);

    return 0;
}
