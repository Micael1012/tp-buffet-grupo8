#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
 
#define MAX_MOZOS 100
 
struct Mozo {
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};
 
struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};
 
struct ResumenMozo {
    int idMozo;
    char nombre[50];
    int cantidadVendida;
    float comisionTotal;
};
 
// Busca el nombre del mozo en mozos.dat por su ID
int buscarNombreMozo(FILE *archivoMozos, int idBuscado, char nombreEncontrado[]) {
    struct Mozo mozo;
 
    rewind(archivoMozos);
 
    while (fread(&mozo, sizeof(struct Mozo), 1, archivoMozos) == 1) {
        if (mozo.idMozo == idBuscado) {
            strcpy(nombreEncontrado, mozo.nombre);
            return 1;
        }
    }
 
    return 0;
}
 

int buscarOCrearResumen(struct ResumenMozo resumenes[], int *cantidadResumenes,
                         int idMozo, FILE *archivoMozos) {
    int i;
 
    for (i = 0; i < *cantidadResumenes; i++) {
        if (resumenes[i].idMozo == idMozo) {
            return i;
        }
    }
 
    resumenes[*cantidadResumenes].idMozo = idMozo;
    resumenes[*cantidadResumenes].cantidadVendida = 0;
    resumenes[*cantidadResumenes].comisionTotal = 0.0f;
 
    if (!buscarNombreMozo(archivoMozos, idMozo, resumenes[*cantidadResumenes].nombre)) {
        strcpy(resumenes[*cantidadResumenes].nombre, "(mozo desconocido)");
    }
 
    (*cantidadResumenes)++;
 
    return (*cantidadResumenes) - 1;
}
 
int main() {
 
    FILE *archivoComandas;
    FILE *archivoMozos;
 
    char numeroSemana[10];
    char mes[3];
    char nombreArchivo[50];
 
    struct Comanda c;
    struct ResumenMozo resumenes[MAX_MOZOS];
    int cantidadResumenes = 0;
 
    int totalBuffetCantidad = 0;
    float totalBuffetComision = 0.0f;
 
    int i;
    int indice;
 
    printf("========================================\n");
    printf("      RESUMEN DE CIERRE SEMANAL\n");
    printf("========================================\n\n");
 
    printf("Ingrese el numero de semana (ej: 1): S");
    scanf("%9s", numeroSemana);
 
    printf("Ingrese el mes (dos digitos, ej: 06): ");
    scanf("%2s", mes);
 
    strcpy(nombreArchivo, "comandas_semana_s");
    strcat(nombreArchivo, numeroSemana);
    strcat(nombreArchivo, "-");
    strcat(nombreArchivo, mes);
    strcat(nombreArchivo, ".dat");
 
    archivoComandas = fopen(nombreArchivo, "rb");
 
    if (archivoComandas == NULL) {
        printf("ERROR: no se pudo abrir %s\n", nombreArchivo);
        return 1;
    }
 
    archivoMozos = fopen("mozos.dat", "rb");
 
    if (archivoMozos == NULL) {
        printf("ERROR: no se pudo abrir mozos.dat\n");
        fclose(archivoComandas);
        return 1;
    }
 
    while (fread(&c, sizeof(struct Comanda), 1, archivoComandas) == 1) {
        indice = buscarOCrearResumen(resumenes, &cantidadResumenes, c.idMozo, archivoMozos);
 
        resumenes[indice].cantidadVendida += c.cantidad;
        resumenes[indice].comisionTotal += c.comision;
 
        totalBuffetCantidad += c.cantidad;
        totalBuffetComision += c.comision;
    }
 
    fclose(archivoComandas);
    fclose(archivoMozos);
 
    printf("\nPlanilla: %s\n", nombreArchivo);
    printf("----------------------------------------\n");
 
    if (cantidadResumenes == 0) {
        printf("No hay ventas registradas en esta semana.\n");
        return 0;
    }
 
  
    for (i = 0; i < cantidadResumenes; i++) {
        printf("\nMozo #%d - %s\n", resumenes[i].idMozo, resumenes[i].nombre);
        printf("  Productos vendidos: %d\n", resumenes[i].cantidadVendida);
        printf("  Comision a pagar:   $%.2f\n", resumenes[i].comisionTotal);
 
        if (i < cantidadResumenes - 1) {
            printf("----------------------------------------\n");
        }
    }
 
    printf("\n========================================\n");
    printf("Total de productos vendidos por el buffet: %d\n", totalBuffetCantidad);
    printf("Total de comision generada:                $%.2f\n", totalBuffetComision);
    printf("========================================\n");
 
    return 0;
}