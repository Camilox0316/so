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


