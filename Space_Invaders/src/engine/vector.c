#include "vector.h"

// Ciclo vida vector
int vectorCrear(Vector *v, size_t capacidad, size_t tamElem)
{
    v->vec = malloc(tamElem * capacidad);
    if(!v->vec)
        return 0;

    v->capacidad = capacidad;
    v->tamElem = tamElem;
    v->ce = 0;

    return 1;
}

void vectorDestruir(Vector *v)
{
    free(v->vec);
}

void vectorVaciar(Vector *v)
{
    v->ce = 0;
}

int vectorRedimensionar(Vector *v, size_t nuevaCap)
{
    void *tmp = realloc(v->vec, v->tamElem * nuevaCap);
    if(tmp == NULL)
        return 0;

    v->vec = tmp;
    v->capacidad = nuevaCap;

    return 1;
}

// Busquedas y actualizaciones
int VectorSsearch(Vector *v, void *elem, int cmp(const void*, const void *))
{
    if(v->ce == 0)
        return 0;

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

int vectorUpdate(Vector *v, void *elem, int cmp(const void*, const void *))
{
    if(v->ce == 0)
        return 0;

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

int vectorUpdateByPos(Vector *v, unsigned pos, const void *elem)
{
    void *lvec = v->vec;

    if(pos < VectorCE(v))
    {
        lvec += pos * v->tamElem;
        memcpy(lvec, elem, v->tamElem);
        return 1;
    }

    return 0;
}

// Inserciones
int vectorInsertarAlFinal(Vector *v, const void *d)
{
    if(v->ce == v->capacidad)
    {
        if(!vectorRedimensionar(v, v->capacidad * 2))
            return SIN_MEM;
    }

    void *ult = v->vec + v->ce * v->tamElem;
    memcpy(ult, d, v->tamElem);
    (v->ce)++;

    return TODO_OK;
}

int vectorInsertarOrdenado(Vector *v, const void *elem, int cmp(const void*, const void *))
{
    if(v->ce == v->capacidad)
    {
        if(!vectorRedimensionar(v, v->capacidad * 2))
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

// Recorridos
Vector *vectorMap(Vector *v, int accion(void *e, void *d), void *datos)
{
    void *lvec = v->vec;
    unsigned i;

    for(i = 0; i < v->ce; i++)
    {
        accion(lvec, datos);
        lvec += v->tamElem;
    }

    return v;
}

int vectorFilter(Vector *v, int ffiltro(const void *elem, void *dato), void *dato)
{
    char *pl = (char *)v->vec;
    char *pe = (char *)v->vec;
    char *fin = (char *)v->vec + (v->ce * v->tamElem);

    while(pl < fin)
    {
        if(ffiltro(pl, dato))
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

int vectorGetByPos(Vector *v, unsigned pos, void *elem)
{
    void *lvec = v->vec;

    if(pos < VectorCE(v))
    {
        lvec+= pos*v->tamElem;
        memcpy(elem,lvec,v->tamElem);
        return 1;
    }

    return 0;
}

int vectorRemoveByPos(Vector *v, unsigned pos)
{
    char *lvec = (char *)v->vec;
    char *siguiente;
    size_t bytes;

    if(pos >= v->ce)
        return 0;

    lvec += pos * v->tamElem;
    siguiente = lvec + v->tamElem;

    bytes = (v->ce - pos - 1) * v->tamElem;

    if(bytes > 0)
        memmove(lvec, siguiente, bytes);

    v->ce--;

    return 1;
}

// Ordenamiento
void vectorQsort(Vector *v, int cmp(const void*, const void*))
{
    qsort(v->vec, v->ce, v->tamElem, cmp);
}

void vectorOrdenarSeleccion(Vector *v, int cmp(const void *, const void*))
{
    void *ult = (char *)v->vec + (v->ce - 1) * v->tamElem;
    void *ini = v->vec;
    void *men;

    while(ini < ult)
    {
        men = buscarMenor(ini, ult, v->tamElem, cmp);
        intercambiar(men, ini, v->tamElem);
        ini = (char *)ini + v->tamElem;
    }
}

void *buscarMenor(void *ini, void *fin, size_t tamElem, int cmp(const void*, const void*))
{
    void *men = ini;
    ini = (char *)ini + tamElem;

    while(ini <= fin)
    {
        if(cmp(ini, men) < 0)
            men = ini;

        ini = (char *)ini + tamElem;
    }

    return men;
}

void intercambiar(void *a, void *b, size_t tamElem)
{
    void *aux = malloc(tamElem);
    if(!aux)
        return;
    memcpy(aux, a, tamElem);
    memcpy(a, b, tamElem);
    memcpy(b, aux, tamElem);
    free(aux);
}

// Varios
int vectorVacio(Vector *v)
{
    return v->ce == 0;
}

int VectorCE(Vector *v)
{
    return v->ce;
}
