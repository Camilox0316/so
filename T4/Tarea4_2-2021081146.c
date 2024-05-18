/*
Escriba un rutina debug que permita imprimir la información de los diferentes bloques que componen la memoria asignada y libre, 
utilizando malloc.
*/

#include <stdio.h>
#include <unistd.h>

// malloc.h

typedef long Align; // alineamiento al límite superior

union header { // encabezado de bloque
    struct {
        union header *ptr; // siguiente bloque
        unsigned size; // tamaño del bloque
    } s;
    Align x; // alineamiento de bloque
};

typedef union header Header;

static Header base; // lista vacía al inicio
static Header *freep = NULL; // lista libre

// free: coloca el bloque ap en la lista vacía
void free(void *ap) {
    Header *bp, *p;
    bp = (Header *)ap - 1; // Apunta al encabezado de un bloque
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break; // libera bloque al inicio o al final

    if (bp + bp->s.size == p->s.ptr) { // fusiona al nbr superior
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    } else
        bp->s.ptr = p->s.ptr; // fusiona al nbr inferior

    if (p + p->s.size == bp) {
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    } else
        p->s.ptr = bp;

    freep = p;
}

#define NALLOC 1024 // mínimo de unidades por requerir

// morecore: solicita más memoria del sistema
static Header *morecore(unsigned nu) {
    void *cp;
    Header *up;
    if (nu < NALLOC)
        nu = NALLOC;
    cp = sbrk(nu * sizeof(Header));
    if (cp == (char *)-1) // no hay nada de espacio
        return NULL;
    up = (Header *)cp;
    up->s.size = nu;
    free((void *)(up + 1));
    return freep;
}

// malloc: asignador de memoria
void *malloc(unsigned long nbytes) {
    Header *p, *prevp;
    Header *morecode(unsigned);
    unsigned nunits;
    nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
    if ((prevp = freep) == NULL) { // no hay lista libre
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }
    for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) { // suficiente grande
            if (p->s.size == nunits) // exacto
                prevp->s.ptr = p->s.ptr;
            else {
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            freep = prevp;
            return (void *)(p + 1);
        }
        if (p == freep) // dio la vuelta a la lista libre
            if ((p = morecore(nunits)) == NULL)
                return NULL; // nada libre
    }
}

// debug: imprime el estado de la memoria
void debug(void) {
    Header *current;
    int counter = 0;

    if (freep == NULL) {
        printf("La lista de memoria libre está vacía.\n");
        return;
    }

    // Comenzamos en el primer bloque libre
    current = freep;

    printf("╔═══════════════════════════════════════════╗\n");
    printf("║         BLOQUES DE MEMORIA LIBRES         ║\n");
    printf("╚═══════════════════════════════════════════╝\n");
    printf("╔═════════════╦═════════════╦═══════════════╗\n");
    printf("║   Bloque    ║   Dirección  ║   Tamaño     ║\n");
    printf("╚═════════════╩═════════════╩═══════════════╝\n");

    do {
        printf("║ %11d ║ %11p ║ %11u ║\n", ++counter, (void *)current, current->s.size);
        current = current->s.ptr;
    } while (current != freep); // Continúa hasta que la lista da la vuelta completa

    printf("╚═════════════╩═════════════╩═══════════════╝\n");

    if (counter == 0) {
        printf("No hay bloques libres.\n");
    }
    printf("\n");
}

int main() {
    debug();
    char *p1 = malloc(150);
    debug();
    char *p2 = malloc(300);
    debug();
    char *p3 = malloc(500);
    debug();
    free(p1);
    debug();
    char *p4 = malloc(100);
    debug();
    free(p3);
    debug();
    char *p5 = malloc(200);
    debug();
    free(p2);
    debug();
    free(p4);
    debug();
    free(p5);
    debug();
    return 0;
}
