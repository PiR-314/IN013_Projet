#ifndef VECT_H
#define VECT_H

#include <immintrin.h>

typedef unsigned int Uint;

__m256i mod_x(Uint *res, __m256i x, Uint i, Uint p);
void vect_add(Uint *res, Uint *tab1, Uint *tab2, Uint taille, Uint p);
void vect_sub(Uint *res, Uint *tab1, Uint *tab2, Uint taille, Uint p);
void vect_add_sub(Uint *res1, Uint *res2, Uint *tab1, Uint *tab2, Uint taille, Uint p);

void vect_mod_add(Uint *res1, Uint *tab1, Uint *tab2);
void vect_mod_sub(Uint *res1, Uint *tab1, Uint *tab2);
void vect_mod_add_sub(Uint *res_add, Uint *res_sub, Uint *tab1, Uint *tab2);
#endif
