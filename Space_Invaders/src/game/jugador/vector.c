#include "vector.h"
#include <stdlib.h>
#include <string.h>
#define VERDADERO 1
#define FALSO 0
#define ERROR -1
#define MINTAM 3

int Vector_init(Vector *self, unsigned tam)
{
    self->vec=malloc(tam*MINTAM);
    if(!self->vec)
        return 0;
    self->ce = 0;
    self->ctotal = MINTAM;
    self->tam = tam;
    return 1;
}

void Vector_destroy(Vector *self)
{
    free(self->vec);
    self->ce=0;
    self->ctotal=0;
    self->tam=0;
}

int Vector_pushback(Vector* self, const void *d)
{
    void* tmp;
    if(self->ce == self->ctotal)
    {
        tmp=malloc((self->ctotal+MINTAM)*self->tam);
        if(!tmp)
            return 0;
        memcpy(tmp,self->vec,(self->ce*self->tam));
        free(self->vec);
        self->vec=tmp;
        self->ctotal=self->ctotal+MINTAM;
    }
    memcpy(self->vec+(self->ce*self->tam),d,self->tam);
    (self->ce)++;
    return 1;
}

Vector* Vector_map(Vector* self, int action(void*))
{
    void* lvec = self->vec;
    for(unsigned i=0;i<self->ce;i++)
    {
        action(lvec);
        lvec+=self->tam;
    }
    return self;
}

void swap(void* a, void* b, unsigned tam)
{
    char aux[tam];

    memcpy(aux, a, tam);
    memcpy(a, b, tam);
    memcpy(b, aux, tam);
}

void Vector_ssort(Vector* self, int cmp(const void*, const void*))
{
    char* ini = self->vec;
    char* menor = self->vec;
    char* j = self->vec;
    for(unsigned i=0;i<self->ce;i++)
    {
        menor=ini;
        for(j=ini;j<(char*)self->vec+self->ce*self->tam;j+=self->tam)
        {
            if(cmp(j,menor))
                menor=j;
        }
        swap(ini,menor,self->tam);
        ini += self->tam;
    }
}

void Vector_qsort(Vector* self, int cmp(const void*, const void*))
{
    qsort(self->vec, self->ce, self->tam, cmp);
}

int Vector_reduce(Vector *self)
{
    if((self->ctotal - self->ce) * 100 < self->ctotal * 20)
        return 1;

    void* tmp;

    tmp = realloc(self->vec, self->ce * self->tam);
    if(!tmp)
        return 0;

    self->vec = tmp;
    self->ctotal = self->ce;

    return 1;
}

void Vector_filter(Vector *self, int filt(const void *))
{
    void* pl = self->vec;
    void* pe = self->vec;
    void* fin = self->vec + self->ce * self->tam;

    while(pl < fin)
    {
        if(!filt(pl))
        {
            while(pl < fin && !filt(pl))
                pl += self->tam;

            if(pl < fin)
                swap(pe, pl, self->tam);
        }

        pe += self->tam;
        pl += self->tam;
    }
}


int Vector_ssearch(Vector* self, void* d, int cmp(const void*, const void*))
{
    void* ini = self->vec;
    void* fin = self->vec + self->ce * self->tam;
    int encontrado = FALSO;
    while(ini < fin && !encontrado)
    {
        if(cmp(ini,d))
        {
            memcpy(d,ini,self->tam);
            encontrado=VERDADERO;
        }
        ini+=self->tam;
    }

    return encontrado;
}

void Vector_update(Vector* self, void* d, int cmp(const void*, const void*))
{
    void* ini = self->vec;
    void* fin = self->vec + self->ce * self->tam;
    int encontrado=FALSO;
    while(ini < fin && !encontrado)
    {
        if(cmp(ini,d))
        {
            memcpy(ini, d, self->tam);
            encontrado=VERDADERO;
        }
        ini+=self->tam;
    }
}
