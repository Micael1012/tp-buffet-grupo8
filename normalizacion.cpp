#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "common.h"

using namespace std;

int buscarProductoPorCodigo(vector<Producto>& productos, int codigo) {
    int izq = 0, der = (int)productos.size() - 1;
    while (izq <= der) {
        int medio = (izq + der) / 2;
        if (productos[medio].codigo == codigo) return medio;
        else if (productos[medio].codigo < codigo) izq = medio + 1;
        else der = medio - 1;
    }
    return -1;
}
int buscarMozoPorNombre(vector<Mozo>& mozos, const char* nombre) {
    for (size_t i = 0; i < mozos.size(); i++) {
        if (strcmp(mozos[i].nombre, nombre) == 0) return (int)i;
    }
    return -1;
}

int main() {
    FILE* fInv = fopen("inventario.dat", "rb");
    if (!fInv) { printf("No se pudo abrir inventario.dat\n"); return 1; }
    vector<Producto> productos;
    Producto p;
    while (fread(&p, sizeof(Producto), 1, fInv) == 1) productos.push_back(p);
    fclose(fInv);

    FILE* fHist = fopen("comandas_historicas.dat", "rb");
    if (!fHist) { printf("No se pudo abrir comandas_historicas.dat\n"); return 1; }
    vector<ComandaHistorica> historicas;
    ComandaHistorica c;
    while (fread(&c, sizeof(ComandaHistorica), 1, fHist) == 1) historicas.push_back(c);
    fclose(fHist);

    printf("Leidos %d productos y %d ventas historicas.\n",
           (int)productos.size(), (int)historicas.size());
    printf("sizeof(ComandaHistorica)=%d  sizeof(Producto)=%d\n",
           (int)sizeof(ComandaHistorica), (int)sizeof(Producto));

    vector<Mozo> mozos;
    for (size_t i = 0; i < historicas.size(); i++) {
        int idx = buscarMozoPorNombre(mozos, historicas[i].nombreMozo);
        if (idx == -1) {
            Mozo m;
            m.idMozo = (int)mozos.size() + 1;
            strncpy(m.nombre, historicas[i].nombreMozo, sizeof(m.nombre));
            m.nombre[sizeof(m.nombre) - 1] = '\0';

            char claveInicial[20];
            snprintf(claveInicial, sizeof(claveInicial), "%d", m.idMozo);
            cifrarPassword(m.password, claveInicial, sizeof(m.password), K_CIFRADO);

            m.totalComision = historicas[i].comision;
            mozos.push_back(m);
        } else {
            mozos[idx].totalComision += historicas[i].comision;
        }
    }

    FILE* fMozos = fopen("mozos.dat", "wb");
    for (size_t i = 0; i < mozos.size(); i++) fwrite(&mozos[i], sizeof(Mozo), 1, fMozos);
    fclose(fMozos);
    printf("mozos.dat generado con %d mozos.\n", (int)mozos.size());
    vector<string> fechas;
    for (size_t i = 0; i < historicas.size(); i++) {
        string f(historicas[i].fecha);
        bool existe = false;
        for (size_t j = 0; j < fechas.size(); j++) {
            if (fechas[j] == f) { existe = true; break; }
        }
        if (!existe) fechas.push_back(f);
    }

    for (size_t d = 0; d < fechas.size(); d++) {
        vector<Comanda> ventasDelDia;
        for (size_t i = 0; i < historicas.size(); i++) {
            if (fechas[d] == historicas[i].fecha) {
                int idxMozo = buscarMozoPorNombre(mozos, historicas[i].nombreMozo);
                Comanda cm;
                cm.idMozo = mozos[idxMozo].idMozo;
                cm.codigoProducto = historicas[i].codigoProducto;
                cm.cantidad = historicas[i].cantidad;
                cm.comision = historicas[i].comision;
                ventasDelDia.push_back(cm);
            }
        }

        for (size_t i = 1; i < ventasDelDia.size(); i++) {
            Comanda actual = ventasDelDia[i];
            int j = (int)i - 1;
            while (j >= 0 && ventasDelDia[j].idMozo > actual.idMozo) {
                ventasDelDia[j + 1] = ventasDelDia[j];
                j--;
            }
            ventasDelDia[j + 1] = actual;
        }

        char nombreArchivo[64];
        snprintf(nombreArchivo, sizeof(nombreArchivo), "comandas_%s.dat", fechas[d].c_str());
        FILE* fDia = fopen(nombreArchivo, "wb");
        for (size_t i = 0; i < ventasDelDia.size(); i++) fwrite(&ventasDelDia[i], sizeof(Comanda), 1, fDia);
        fclose(fDia);
        printf("%s generado con %d ventas.\n", nombreArchivo, (int)ventasDelDia.size());
    }

    for (size_t i = 0; i < historicas.size(); i++) {
        int idxProd = buscarProductoPorCodigo(productos, historicas[i].codigoProducto);
        if (idxProd != -1) productos[idxProd].stockActual -= historicas[i].cantidad;
        else printf("AVISO: la venta historica tiene un codigo de producto inexistente (%d)\n",
                     historicas[i].codigoProducto);
    }

    FILE* fInvOut = fopen("inventario.dat", "wb");
    for (size_t i = 0; i < productos.size(); i++) fwrite(&productos[i], sizeof(Producto), 1, fInvOut);
    fclose(fInvOut);
    printf("inventario.dat actualizado con el stock al dia.\n");

    return 0;
}