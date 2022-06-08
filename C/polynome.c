#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "polynome.h"
#include "vect.h"

void afficher_poly(Poly P) {
    printf("%d", (P.coeffs)[0]);
    for (int i = 1; i <= P.deg; i++) {
        printf(" + %dx^%d", (P.coeffs)[i], i);
    }
    printf("\n");
}

Poly creer_poly(Uint deg) {
    Poly P;
    P.deg = deg;
    P.coeffs = (Uint *) malloc(sizeof(Uint)*(deg+1));
    return P;
}

Poly gen_poly(Uint deg) {
    Poly P = creer_poly(deg);
    for (int i = 0; i <= deg; i++) {
        (P.coeffs)[i] = rand() % NB_P;
    }
    return P;
}

void liberer_poly(Poly P) {
    free(P.coeffs);
}

int compare_poly(Poly P, Poly Q) {
    if (!(P.deg == Q.deg)) {
        return 0;
    }
    for (int i = 0; i <= P.deg; i++) {
        if ((P.coeffs)[i] != (Q.coeffs)[i]) {
            printf("Problème : C1 = %d, C2 = %d, pow = x^%d\n", (P.coeffs)[i], (Q.coeffs)[i], i);
            return 0;
        }
    }
    return 1;
}

Poly prod_poly_naif(Poly P, Poly Q) {
    Poly R;
    R.coeffs = (Uint *) malloc(sizeof(Uint)*(P.deg + Q.deg + 1));
    R.deg = P.deg + Q.deg;
    for (int i = 0; i <= P.deg; i++) {
        for (int j = 0; j <= Q.deg; j++) {
            (R.coeffs)[i + j] = ((R.coeffs)[i+j] + (P.coeffs)[i]*(Q.coeffs)[j]) % NB_P;
        }
    }
    return R;
}

Poly copy_poly(Poly P, Uint deb, Uint fin) {
    Poly R;
    R.coeffs = (Uint *) malloc(sizeof(Uint)*(fin - deb + 1));
    R.deg = fin - deb;
    for (int i = deb; i <= fin; i++) {
        (R.coeffs)[i-deb] = (P.coeffs)[i];
    }
    return R;
}

Poly somme_poly(Poly P, Poly Q, Uint deb) {
    Poly R = copy_poly(P, 0, P.deg);
    for (int i = 0; i <= Q.deg; i++) {
        (R.coeffs)[deb + i] = ((R.coeffs)[deb + i] + (Q.coeffs)[i]) % NB_P;
    }
    return R;
}

Poly oppose_poly(Poly P) {
    Poly R = copy_poly(P, 0, P.deg);
    for (int i = 0; i <= R.deg; i++) {
        (R.coeffs)[i] = -(R.coeffs)[i] + NB_P;
    }
    return R;
}

Poly prod_poly_karatsuba(Poly P, Poly Q) {
    if (P.deg <= 130) {
        return prod_poly_naif(P, Q);
    }
    Uint k = (int) ceil((P.deg+1) / 2.0);
    Poly P1 = copy_poly(P, 0, k-1);
    Poly P2 = copy_poly(P, k, P.deg);
    Poly Q1 = copy_poly(Q, 0, k-1);
    Poly Q2 = copy_poly(Q, k, Q.deg);
    Poly E1 = prod_poly_karatsuba(P1, Q1);
    Poly E2 = prod_poly_karatsuba(P2, Q2);
    Poly E3 = prod_poly_karatsuba(somme_poly(P1, P2, 0), somme_poly(Q1, Q2, 0));

    Poly R;
    R.deg = P.deg + Q.deg;
    R.coeffs = (Uint *) calloc(sizeof(Uint), R.deg + 1);

    R = somme_poly(R, E1, 0);
    R = somme_poly(R, E2, 2*((int) ((P.deg+2)/2.0)));
    R = somme_poly(R, somme_poly(E3, oppose_poly(somme_poly(E1, E2, 0)), 0), k);
    
    return R;
}

Uint horner(Poly P, Uint x) {
    Uint res = P.coeffs[P.deg];
    for (int i = P.deg-1; i >= 0; i--) {
        res = mod_add(mod_mult(res, x, NB_P), P.coeffs[i], NB_P);
    }
    return res;
}

Uint mod_pow(Uint x, Uint n) {
    if (n == 0) {
        return 1;
    }
    if (n == 1) {
        return x;
    }
    Uint res;
    if (n % 2 == 0) {
        res = mod_pow(x, n/2);
        return mod_mult(res, res, NB_P);
    }
    res = mod_pow(x, n-1);
    return mod_mult(res, x, NB_P);
}

Uint *get_racines(Uint racine, Uint n) {
    Uint *racines = (Uint *) malloc(sizeof(Uint) * n);
    racines[0] = 1;
    for (int i = 1; i < n; i++) {
        racines[i] = mod_mult(racines[i-1], racine, NB_P);
    }
    return racines;
}

Uint mod_add(Uint a, Uint b, Uint p) {
    Uint res = a + b;
    if (res < p) {
        return res;
    }
    return res - p;
}

Uint mod_sub(Uint a, Uint b, Uint p) {
    if (a < b) {
        return p - (b - a);
    }
    return a - b;
}

Uint mod_mult(Uint a, Uint b, Uint p) {
    return ((unsigned long) a*b) % p;
}

Uint inv(Uint a, Uint p) {
    //TODO
    return 0;
}


Uint *eval_malloc(Poly P, Uint *racines) {
    if (P.deg == 0) {
        Uint *tmp = (Uint *) malloc(sizeof(Uint));
        tmp[0] = (P.coeffs)[0];
        return tmp;
    }
    Uint k = (P.deg + 1)/2;
    Poly R0 = creer_poly(k-1); // creer_poly() fait un malloc
    Poly R1 = creer_poly(k-1);

	Uint tmp;
    Uint *racines_bis = (Uint *) malloc(sizeof(Uint) * k);
    for (int i = 0; i < k; i++) {
        (R0.coeffs)[i] = mod_add((P.coeffs)[i], (P.coeffs)[i+k], NB_P);
		tmp = mod_sub((P.coeffs)[i], (P.coeffs)[i+k], NB_P);
        (R1.coeffs)[i] = mod_mult(tmp, racines[i], NB_P);
        racines_bis[i] = racines[2*i];
    }
    Uint *r0 = eval_malloc(R0, racines_bis);
    Uint *r1 = eval_malloc(R1, racines_bis);
    Uint *res = (Uint *) malloc(sizeof(Uint) * 2*k);
    for (int i = 0; i < k; i++) {
        res[2*i] = r0[i];
        res[2*i+1] = r1[i];
    }
    return res;
}

Uint *eval(Uint *coeffs, Uint deg, Uint *tmp_coeffs, Uint *racines, Uint pas_rac) {
    if (deg == 0) {
		return &coeffs[0];
    }

    Uint k = (deg + 1)/2;
    
    Uint tmp;
    for (int i = 0; i < k; i++) {
        tmp_coeffs[i] = mod_add(coeffs[i], coeffs[i+k], NB_P);
        tmp = mod_sub(coeffs[i], coeffs[i+k], NB_P);
        tmp_coeffs[i+k] = mod_mult(tmp, racines[i*pas_rac], NB_P);
    }
    Uint *r0 = eval(tmp_coeffs, k-1, coeffs, racines, pas_rac*2);
    Uint *r1 = eval(&tmp_coeffs[k], k-1, &coeffs[k], racines, pas_rac*2);
    for (int i = 0; i < k; i++) {
        coeffs[2*i] = r0[i];
        coeffs[2*i+1] = r1[i];
    }
    return coeffs;
}

Uint *vect_eval(Uint *coeffs, Uint taille, Uint *tmp_coeffs, Uint *racines, Uint pas_rac, Uint *tmp_sub) {
    if (taille == 1) return &coeffs[0];

    Uint tmp;
    Uint k = taille/2;
    
    if (k >= 8) {
        for (Uint i = 0; i < k; i += 8) {
            vect_mod_add_sub_eval(&tmp_coeffs[i], &tmp_sub[i], &coeffs[i], &coeffs[i+k]);
            vect_mod_mult_eval(&tmp_coeffs[i+k], &tmp_sub[i], racines, i, pas_rac);
        }
    } else {
        for (Uint i = 0; i < k; i++) {
            tmp_coeffs[i] = mod_add(coeffs[i], coeffs[i+k], NB_P);
            tmp = mod_sub(coeffs[i], coeffs[i+k], NB_P);
            tmp_coeffs[i+k] = mod_mult(tmp, racines[i*pas_rac], NB_P);
        }
    }
    
    tmp = pas_rac*2;
    Uint *r0 = vect_eval(tmp_coeffs, k, coeffs, racines, tmp, tmp_sub);
    Uint *r1 = vect_eval(&tmp_coeffs[k], k, &coeffs[k], racines, tmp, tmp_sub);

    for (Uint i = 0; i < k; i++) {
        tmp = 2*i;
        coeffs[tmp] = r0[i];
        coeffs[tmp+1] = r1[i];
    }
    return coeffs;
}

float *get_racines_inverse(Uint racine, Uint n) {
    float *racines_inv = (float *) malloc(sizeof(float) * n);
    float racine_inv = 1.0/racine;
    racines_inv[0] = 1;
    for (int i = 1; i < n; i++) {
        racines_inv[i] = racines_inv[i-1] * racine_inv;
    }
    float n_inv = 1.0/n;
    for (int i = 0; i < n; i++) {
        racines_inv[i] = n_inv * racines_inv[i];
    }
    return racines_inv;
}

Uint *eval_inv(Uint *coeffs, Uint deg, Uint *tmp_coeffs, float *racines, Uint pas_rac) {
    if (deg == 0) {
		return &coeffs[0];
    }

    Uint k = (deg + 1)/2;
    
    Uint tmp;
    for (int i = 0; i < k; i++) {
        tmp_coeffs[i] = mod_add(coeffs[i], coeffs[i+k], NB_P);
        tmp = mod_sub(coeffs[i], coeffs[i+k], NB_P);
        tmp_coeffs[i+k] = mod_mult(tmp, racines[i*pas_rac], NB_P);
    }
    Uint *r0 = eval_inv(tmp_coeffs, k-1, coeffs, racines, pas_rac*2);
    Uint *r1 = eval_inv(&tmp_coeffs[k], k-1, &coeffs[k], racines, pas_rac*2);
    for (int i = 0; i < k; i++) {
        coeffs[2*i] = r0[i];
        coeffs[2*i+1] = r1[i];
    }
    return coeffs;
}

Poly FFT(Poly P, Poly Q) {
    Uint racine = 11;
    Uint ordre_racine = NB_P-1;
    Uint deg = 16777215;
    Uint n = deg+1;
    Uint racine_principale = mod_pow(racine, ordre_racine/n);

    Uint *racines = get_racines(racine_principale, n);

    Uint *tmp_coeffs = (Uint *) malloc(sizeof(Uint)*n);
    Uint *tmp_sub = (Uint *) malloc(sizeof(Uint)*n);
    Uint *eval_P = vect_eval(P.coeffs, n, tmp_coeffs, racines, 1, tmp_sub);
    Uint *eval_Q = vect_eval(Q.coeffs, n, tmp_coeffs, racines, 1, tmp_sub);

    Uint *eval_R = (Uint *) malloc(sizeof(Uint)*n);
    for (Uint i = 0; i < n; i += 8) {
        vect_mod_mult(&eval_R[i], &eval_P[i], &eval_Q[i]);
    }

    float *racines_inv = get_racines_inverse(racine, n);
    Poly R = creer_poly(P.deg + Q.deg);
    R.coeffs = eval_inv(eval_R, n, tmp_coeffs, racines_inv, 1);
    return R;
}