/*
Codigo fuente de alloc
// alloc.h
#define ALLOCSIZE 10000 // tamaño del espacio disponible

static char allocbuf[ALLOCSIZE]; // almacenamiento para alloc
static char *allocp = allocbuf;  // siguiente posición libre

char *alloc(int n) { // regresa un puntero a n caracteres
  if (allocbuf + ALLOCSIZE - allocp >= n) {
    allocp += n;
    return allocp - n;
  } else
    return 0;
}

void afree(char *p) { // libera la memoria apunta por p
  if (p >= allocbuf && p < allocbuf + ALLOCSIZE)
    allocp = p;
}

*/

/*
Modifique el código de alloc(n,i), para que permita dos áreas de asignación de memoria independientes, identificadas por la i, 
pero que compartan la misma pila. Un área asigna de arriba hacia abajo y la otra de abajo hacia arriba en la pila. 
Cuando ambas se unen es porque la pila está llena.
*/

#define ALLOCSIZE 10000 // tamaño del espacio disponible

static char allocbuf[ALLOCSIZE]; // almacenamiento para alloc
static char *allocp1 = allocbuf; // siguiente posición libre para el área 1 (arriba hacia abajo)
static char *allocp2 = allocbuf + ALLOCSIZE; // siguiente posición libre para el área 2 (abajo hacia arriba)

char *alloc(int n, int i) { // regresa un puntero a n caracteres en el área i
    if (i == 1) {
        if (allocp2 - allocp1 >= n) {
            allocp1 += n;
            return allocp1 - n;
        } else {
            return 0; // no hay suficiente espacio disponible
        }
    } else if (i == 2) {
        if (allocp2 - allocp1 >= n) {
            allocp2 -= n;
            return allocp2;
        } else {
            return 0; // no hay suficiente espacio disponible
        }
    } else {
        return 0; // área inválida
    }
}

void afree(char *p, int i) { // libera la memoria apuntada por p en el área i
    if (i == 1) {
        if (p >= allocbuf && p < allocp1)
            allocp1 = p;
    } else if (i == 2) {
        if (p >= allocp2 && p < allocbuf + ALLOCSIZE)
            allocp2 = p + (allocbuf + ALLOCSIZE - p);
    }
}


int main() {
    int *arreglo1, *arreglo2;

    arreglo1 = (int *)alloc(sizeof(int) * 5, 1); // asignar memoria del área 1
    arreglo2 = (int *)alloc(sizeof(int) * 5, 2); // asignar memoria del área 2

    // Llenar arreglo1
    for (int i = 0; i < 5; i++)
        arreglo1[i] = 10 * i;

    // Llenar arreglo2
    for (int i = 0; i < 5; i++)
        arreglo2[i] = 20 * i;

    // Imprimir arreglo1
    for (int i = 0; i < 5; i++)
        printf("%i\n", arreglo1[i]);

    // Imprimir arreglo2
    for (int i = 0; i < 5; i++)
        printf("%i\n", arreglo2[i]);

    afree((char *)arreglo1, 1); // liberar memoria del área 1
    afree((char *)arreglo2, 2); // liberar memoria del área 2

    return 0;
}
