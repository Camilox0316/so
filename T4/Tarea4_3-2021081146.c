/*
Modifique el código de malloc para aplicar la estrategia del "siguiente ajuste" (next fit), el cuál no se devuelve al inicio 
para realizar la búsqueda del bloque. Escriba un programa de ejemplo para mostrar la ejecución correcta de su modificación.
*/
#include <stdio.h>
#include <unistd.h>

// Definición de estructuras y variables globales
typedef long Align; // alineamiento al límite superior
union header {      // encabezado de bloque
  struct {
    union header *ptr; // siguiente bloque
    unsigned size;     // tamaño del bloque
  } s;
  Align x; // alineamiento de bloque
};

typedef union header Header;
static Header base;          // lista vacía al inicio
static Header *freep = NULL; // lista libre
static Header *last_allocp = NULL; // Puntero para recordar la última posición

#define NALLOC 1024 // mínimo de unidades por requerir

// Funciones auxiliares
static void print_memoria_libre();
static Header *morecore(unsigned nu);
static void insert_block(Header *bp, Header *p);
static Header *find_block(unsigned nunits);

// Implementación de funciones

void free(void *ap) {
    Header *bp, *p;

    bp = (Header *)ap - 1; // Apunta al encabezado de un bloque
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break; // libera bloque al inicio o al final

    if (bp + bp->s.size == p->s.ptr) { // fusiona al vecino superior
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    } else
        bp->s.ptr = p->s.ptr; // fusiona al vecino inferior
    if (p + p->s.size == bp) {
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    } else
        p->s.ptr = bp;
    freep = p;
}

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

void *malloc(unsigned long nbytes) {
    Header *p, *prevp;
    unsigned nunits;

    nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
    if ((prevp = last_allocp) == NULL) { // Si es la primera llamada
        base.s.ptr = freep = prevp = last_allocp = &base;
        base.s.size = 0;
    }

    for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) { // Bloque suficientemente grande
            if (p->s.size == nunits) { // Exactamente del tamaño necesario
                prevp->s.ptr = p->s.ptr;
            } else { // Asignar al final del bloque
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            last_allocp = prevp; // Actualizar el último punto visitado
            return (void *)(p + 1);
        }
        if (p == last_allocp) // Si hemos dado la vuelta completa
            if ((p = morecore(nunits)) == NULL)
                return NULL; // No hay espacio
    }
}

static void print_memoria_libre() {
    Header *p = freep;
    printf("╔═══════════════════════════════════════════╗\n");
    printf("║         BLOQUES DE MEMORIA LIBRES         ║\n");
    printf("╚═══════════════════════════════════════════╝\n");
    printf("╔═════════════╦═════════════╦═══════════════╗\n");
    printf("║   Bloque    ║   Dirección  ║   Tamaño     ║\n");
    printf("╚═════════════╩═════════════╩═══════════════╝\n");

    do {
        printf("╔═════════════╦═════════════╦═══════════════╗\n");
        printf("║   %p   ║   %p   ║   %u   ║\n", p, (void *)(p + 1), p->s.size);
        printf("╚═════════════╩═════════════╩═══════════════╝\n");
        p = p->s.ptr;
    } while (p != freep);
}

int main() {
    void *p1, *p2, *p3, *p4, *p5, *p6, *p7;

    // Primeras asignaciones de memoria
    p1 = malloc(50);
    p2 = malloc(120);
    p3 = malloc(300);

    printf("Primeras asignaciones:\n");
    print_memoria_libre();

    // Liberar algunos bloques
    free(p2);
    free(p1);

    printf("Después de liberar p1 y p2:\n");
    print_memoria_libre();

    // Más asignaciones de memoria
    p4 = malloc(80);
    p5 = malloc(60);

    printf("Después de asignar p4 y p5:\n");
    print_memoria_libre();

    // Liberar y asignar más memoria
    free(p3);
    p6 = malloc(100);
    p7 = malloc(180);

    printf("Después de liberar p3 y asignar p6 y p7:\n");
    print_memoria_libre();

    // Asignar más memoria después de liberar todo
    free(p4);
    free(p5);
    free(p6);
    free(p7);

    printf("Después de liberar p4, p5, p6 y p7:\n");
    print_memoria_libre();

    void *p8 = malloc(400);
    printf("Después de asignar p8 (después de liberar todo):\n");
    print_memoria_libre();

    return 0;
}

