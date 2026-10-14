#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define TODO_OK 0
#define VEC_LLENO 1
#define DUPLICADO 2
#define SIN_MEM 3
#define ERR_ARCHIVO 4

typedef struct
{
    void *vec; // puntero generico al bloque de memoria donde estan los datos
    size_t ce;// cantidad actual de elementos insertados
    size_t tamElem; // tama�o en bytes que ocupa cada elemento
    size_t capacidad; // cacidad maxima de elementos que el vector puede almacenar (sin realloc)
} Vector;

int Vector_create(Vector *v, size_t capacidad, size_t tamElem);
void Vector_destroy(Vector *v);
int Vector_length(Vector *v);
int vector_resize(Vector *v, size_t nuevaCap);
int Vector_pushback(Vector *v, const void *d);

Vector *Vector_map(Vector *v, int action(void *, void *), void *contexto);
int Vector_filter(Vector *v, int ffiltro(const void *, void *), void *contexto);

int Vector_SortInsert(Vector *v, const void *elem, int cmp(const void*, const void *));
int Vector_ssearch(Vector *v, void *elem, int cmp(const void*, const void *));
int vector_update(Vector *v, void *elem, int cmp(const void*, const void *));

#endif // VECTOR_H_INCLUDED