#include "vector.h"

int Vector_create(Vector *v, size_t capacidad, size_t tamElem)
{
    v->vec = malloc(tamElem * capacidad);
    if(!v->vec)
        return 0;

    v->capacidad = capacidad;
    v->tamElem = tamElem;
    v->ce = 0;

    return 1;
}

void Vector_destroy(Vector *v)
{
    free(v->vec);
}

int Vector_length(Vector *v)
{
    return v->ce;
}

int vector_resize(Vector *v, size_t nuevaCap)
{
    void *tmp = realloc(v->vec, v->tamElem * nuevaCap);
    if(tmp == NULL)
        return 0;

    v->vec = tmp;
    v->capacidad = nuevaCap;

    return 1;
}

int Vector_pushback(Vector *v, const void *d)
{
    if(v->ce == v->capacidad)
    {
        if(!vector_resize(v, v->capacidad * 2))
            return SIN_MEM;
    }

    void *ult = v->vec + v->ce * v->tamElem;
    memcpy(ult, d, v->tamElem);
    (v->ce)++;

    return TODO_OK;
}

int Vector_SortInsert(Vector *v, const void *elem, int cmp(const void*, const void *))
{
    if(v->ce == v->capacidad)
    {
        if(!vector_resize(v, v->capacidad * 2))
            return SIN_MEM;
    }

    void *fin = v->vec + v->ce * v->tamElem;
    void *i = v->vec;

    while(i < fin && cmp(elem, fin - v->tamElem) < 0)
    {
        memmove(fin, fin - v->tamElem, v->tamElem);
        fin -= v->tamElem;
    }

    memcpy(fin, elem, v->tamElem);
    (v->ce)++;

    return TODO_OK;
}

int Vector_ssearch(Vector *v, void *elem, int cmp(const void*, const void *))
{
    void *i = v->vec;
    void *ult = v->vec + (v->ce - 1) * v->tamElem;

    while(i <= ult && cmp(elem, i) != 0)
    {
        i += v->tamElem;
    }

    if(i > ult)
        return 0;

    memcpy(elem, i, v->tamElem);

    return 1;
}

int vector_update(Vector *v, void *elem, int cmp(const void*, const void *))
{
    void *i = v->vec;
    void *ult = v->vec + (v->ce - 1) * v->tamElem;

    while(i <= ult && cmp(elem, i) != 0)
    {
        i += v->tamElem;
    }

    if(i > ult)
        return 0;

    memcpy(i, elem, v->tamElem);

    return 1;
}

Vector *Vector_map(Vector *v, int action(void *elem, void *ctx), void *contexto)
{
    void *lvec = v->vec;
    unsigned i;

    for(i = 0; i < v->ce; i++)
    {
        action(lvec, contexto);
        lvec += v->tamElem;
    }

    return v;
}

int Vector_filter(Vector *v, int ffiltro(const void *elem, void *ctx), void *contexto)
{
    char *pl = (char *)v->vec;
    char *pe = (char *)v->vec;
    char *fin = (char *)v->vec + (v->ce * v->tamElem);

    while(pl < fin)
    {
        if(ffiltro(pl, contexto))
        {
            memmove(pe, pl, v->tamElem);
            pl += v->tamElem;
            pe += v->tamElem;
        }
        else
        {
            pl += v->tamElem;
            (v->ce)--;
        }
    }

    return TODO_OK;
}