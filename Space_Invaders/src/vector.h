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
    void *vec;
    size_t ce;
    size_t tamElem;
    size_t capacidad;
} Vector;

//Puntero a funcion
typedef int (*Cmp)(const void *, const void *);
typedef int (*Accion)(void *elem, void *datos);
typedef int (*Condicion)(const void *elem, void *datos);
typedef void (*Imprimir)(const void *a);

// Ciclo vida
int vectorCrear(Vector *v, size_t capacidad, size_t tamElem);
void vectorDestruir(Vector *v);
void vectorVaciar(Vector *v);
int vectorRedimensionar(Vector *v, size_t nuevaCap);

// Busquedas y actualizaciones
int VectorSsearch(Vector *v, void *elem, int cmp(const void*, const void *));
int vectorUpdate(Vector *v, void *elem, int cmp(const void*, const void *));
int vectorUpdateByPos(Vector *v, unsigned pos, const void *elem);

// Inserciones
int vectorInsertarAlFinal(Vector *v, const void *d);
int vectorInsertarOrdenado(Vector *v, const void *elem, int cmp(const void*, const void *));

// Recorridos
Vector *vectorMap(Vector *v, Accion accion, void *dato);
int vectorFilter(Vector *v, int ffiltro(const void *elem, void *dato), void *dato);
int vectorGetByPos(Vector *v, unsigned pos, void *elem);
int vectorRemoveByPos(Vector *v, unsigned pos);

// Ordenamiento
void vectorQsort(Vector *v, int cmp(const void*, const void*));
void vectorOrdenarSeleccion(Vector *v, int cmp(const void *, const void*));
void *buscarMenor(void *ini, void *fin, size_t tamElem, int cmp(const void*, const void*));


// Varios
int vectorVacio(Vector *v);
int VectorCE(Vector *v);
void intercambiar(void *a, void *b, size_t tamElem);

#endif // VECTOR_H_INCLUDED
