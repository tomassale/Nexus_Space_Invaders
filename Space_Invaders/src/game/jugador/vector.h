#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED
typedef struct
{
    void* vec;
    unsigned ce;
    unsigned ctotal;
    unsigned tam;
}Vector;
int Vector_init(Vector* self, unsigned tam);
int Vector_pushback(Vector* self, const void *d);
void Vector_destroy(Vector* self);
void swap(void* a, void* b, unsigned tam);
int Vector_reduce(Vector *self);
void Vector_filter(Vector *self, int filt(const void *));
void Vector_qsort(Vector* self, int cmp(const void*, const void*));
void Vector_ssort(Vector* self, int cmp(const void*, const void*));
int Vector_ssearch(Vector* self, void* d, int cmp(const void*, const void*));
Vector* Vector_map(Vector* self, int action(void*));
void Vector_update(Vector* self, void* d, int cmp(const void*, const void*));
#endif // VECTOR_H_INCLUDED
