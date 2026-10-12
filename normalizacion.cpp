#include <iostream>
#include <cstdio>
#include <cstring>
#include "common.h"

using namespace std;

const int MAX_PRODUCTOS  = 100;
const int MAX_HISTORICAS = 1000;
const int MAX_MOZOS      = 100;
const int MAX_FECHAS     = 100;
const int MAX_VENTAS_DIA = 1000;

int buscarProductoPorCodigo(Producto productos[], int cantProductos, int codigo) {
    int izq = 0, der = cantProductos - 1;
    while (izq <= der) {
        int medio = (izq + der) / 2;
        if (productos[medio].codigo == codigo) return medio;
        else if (productos[medio].codigo < codigo) izq = medio + 1;
        else der = medio - 1;
    }
    return -1;
}

int buscarMozoPorNombre(Mozo mozos[], int cantMozos, const char nombre[]) {
    for (int i = 0; i < cantMozos; i++) {
        if (strcmp(mozos[i].nombre, nombre) == 0) return i;
    }
    return -1;
}

int buscarFecha(char fechas[][11], int cantFechas, const char fecha[]) {
    for (int i = 0; i < cantFechas; i++) {
        if (strcmp(fechas[i], fecha) == 0) return i;
    }
    return -1;
}

void numeroATexto(int n, char texto[]) {
    char invertido[12];
    int largo = 0;
    do {
        invertido[largo] = '0' + n % 10;
        largo++;
        n = n / 10;
    } while (n > 0);
    for (int i = 0; i < largo; i++) {
        texto[i] = invertido[largo - 1 - i];
    }
    texto[largo] = '\0';
}

int main() {
    Producto productos[MAX_PRODUCTOS];
    int cantProductos = 0;

    FILE* fInv = fopen("inventario.dat", "rb");
    if (fInv == NULL) { cout << "No se pudo abrir inventario.dat" << endl; return 1; }
    while (fread(&productos[cantProductos], sizeof(Producto), 1, fInv) == 1) {
        cantProductos++;
    }
    fclose(fInv);

    ComandaHistorica historicas[MAX_HISTORICAS];
    int cantHistoricas = 0;

    FILE* fHist = fopen("comandas_historicas.dat", "rb");
    if (fHist == NULL) { cout << "No se pudo abrir comandas_historicas.dat" << endl; return 1; }
    while (fread(&historicas[cantHistoricas], sizeof(ComandaHistorica), 1, fHist) == 1) {
        cantHistoricas++;
    }
    fclose(fHist);

    cout << "Leidos " << cantProductos << " productos y "
         << cantHistoricas << " ventas historicas." << endl;
    cout << "sizeof(ComandaHistorica)=" << sizeof(ComandaHistorica)
         << "  sizeof(Producto)=" << sizeof(Producto) << endl;

    Mozo mozos[MAX_MOZOS];
    int cantMozos = 0;

    for (int i = 0; i < cantHistoricas; i++) {
        int idx = buscarMozoPorNombre(mozos, cantMozos, historicas[i].nombreMozo);
        if (idx == -1) {
            mozos[cantMozos].idMozo = cantMozos + 1;
            strncpy(mozos[cantMozos].nombre, historicas[i].nombreMozo, sizeof(mozos[cantMozos].nombre));
            mozos[cantMozos].nombre[sizeof(mozos[cantMozos].nombre) - 1] = '\0';

            char claveInicial[20];
            numeroATexto(mozos[cantMozos].idMozo, claveInicial);
            cifrarPassword(mozos[cantMozos].password, claveInicial,
                           sizeof(mozos[cantMozos].password), K_CIFRADO);

            mozos[cantMozos].totalComision = historicas[i].comision;
            cantMozos++;
        } else {
            mozos[idx].totalComision += historicas[i].comision;
        }
    }

    FILE* fMozos = fopen("mozos.dat", "wb");
    for (int i = 0; i < cantMozos; i++) {
        fwrite(&mozos[i], sizeof(Mozo), 1, fMozos);
    }
    fclose(fMozos);
    cout << "mozos.dat generado con " << cantMozos << " mozos." << endl;

    char fechas[MAX_FECHAS][11];
    int cantFechas = 0;

    for (int i = 0; i < cantHistoricas; i++) {
        if (buscarFecha(fechas, cantFechas, historicas[i].fecha) == -1) {
            strcpy(fechas[cantFechas], historicas[i].fecha);
            cantFechas++;
        }
    }

    for (int d = 0; d < cantFechas; d++) {
        Comanda ventasDelDia[MAX_VENTAS_DIA];
        int cantVentas = 0;

        for (int i = 0; i < cantHistoricas; i++) {
            if (strcmp(fechas[d], historicas[i].fecha) == 0) {
                int idxMozo = buscarMozoPorNombre(mozos, cantMozos, historicas[i].nombreMozo);
                ventasDelDia[cantVentas].idMozo = mozos[idxMozo].idMozo;
                ventasDelDia[cantVentas].codigoProducto = historicas[i].codigoProducto;
                ventasDelDia[cantVentas].cantidad = historicas[i].cantidad;
                ventasDelDia[cantVentas].comision = historicas[i].comision;
                cantVentas++;
            }
        }

        for (int i = 1; i < cantVentas; i++) {
            Comanda actual = ventasDelDia[i];
            int j = i - 1;
            while (j >= 0 && ventasDelDia[j].idMozo > actual.idMozo) {
                ventasDelDia[j + 1] = ventasDelDia[j];
                j--;
            }
            ventasDelDia[j + 1] = actual;
        }

        char nombreArchivo[64];
        strcpy(nombreArchivo, "comandas_");
        strcat(nombreArchivo, fechas[d]);
        strcat(nombreArchivo, ".dat");

        FILE* fDia = fopen(nombreArchivo, "wb");
        for (int i = 0; i < cantVentas; i++) {
            fwrite(&ventasDelDia[i], sizeof(Comanda), 1, fDia);
        }
        fclose(fDia);
        cout << nombreArchivo << " generado con " << cantVentas << " ventas." << endl;
    }

    for (int i = 0; i < cantHistoricas; i++) {
        int idxProd = buscarProductoPorCodigo(productos, cantProductos, historicas[i].codigoProducto);
        if (idxProd != -1) {
            productos[idxProd].stockActual -= historicas[i].cantidad;
        } else {
            cout << "AVISO: la venta historica tiene un codigo de producto inexistente ("
                 << historicas[i].codigoProducto << ")" << endl;
        }
    }

    FILE* fInvOut = fopen("inventario.dat", "wb");
    for (int i = 0; i < cantProductos; i++) {
        fwrite(&productos[i], sizeof(Producto), 1, fInvOut);
    }
    fclose(fInvOut);
    cout << "inventario.dat actualizado con el stock al dia." << endl;

    return 0;
}
