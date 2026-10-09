#include <iostream>
#include <cstdio>
#include <cstring>
#include "common.h"

using namespace std;

// ---------------------------------------------------------
// DESENCRIPTAR
// ---------------------------------------------------------
void desencriptar(char* destino, const char* origen, int n)
{
    cifrarPassword(destino, origen, n, -K_CIFRADO);
}

// ---------------------------------------------------------
// VALIDAR MOZO
// Archivo: mozos.dat
// Modo: rb -> solo lectura
// ---------------------------------------------------------
bool validarMozo(int idBuscado, char claveIngresada[])
{
    FILE* archivo = fopen("mozos.dat", "rb");

    if (archivo == NULL)
    {
        cout << "\nERROR: No se pudo abrir mozos.dat\n";
        return false;
    }

    Mozo mozo;

    while (fread(&mozo, sizeof(Mozo), 1, archivo) == 1)
    {
        if (mozo.idMozo == idBuscado)
        {
            char claveReal[20];

            desencriptar(
                claveReal,
                mozo.password,
                sizeof(claveReal)
            );

            fclose(archivo);

            if (strcmp(claveReal, claveIngresada) == 0)
            {
                return true;
            }

            cout << "\nClave incorrecta.\n";
            return false;
        }
    }

    fclose(archivo);

    cout << "\nNo existe un mozo con ese ID.\n";
    return false;
}

// ---------------------------------------------------------
// VENDER PRODUCTO
//
// Archivo: inventario.dat
// Modo: r+b -> lectura y escritura
//
// El inventario está ordenado por código, por lo que se
// utiliza búsqueda binaria.
// ---------------------------------------------------------
bool venderProducto(
    int codigoBuscado,
    int cantidad,
    float& precioProducto
)
{
    FILE* archivo = fopen("inventario.dat", "r+b");

    if (archivo == NULL)
    {
        cout << "\nERROR: No se pudo abrir inventario.dat\n";
        return false;
    }

    Producto producto;

    // -----------------------------------------------------
    // DETERMINAR CANTIDAD DE REGISTROS
    // -----------------------------------------------------
    fseek(archivo, 0, SEEK_END);

    long cantidadRegistros =
        ftell(archivo) / sizeof(Producto);

    long izquierda = 0;
    long derecha = cantidadRegistros - 1;

    bool encontrado = false;

    // -----------------------------------------------------
    // BUSQUEDA BINARIA
    // -----------------------------------------------------
    while (izquierda <= derecha)
    {
        long medio = (izquierda + derecha) / 2;

        fseek(
            archivo,
            medio * sizeof(Producto),
            SEEK_SET
        );

        if (fread(&producto, sizeof(Producto), 1, archivo) != 1)
        {
            fclose(archivo);
            return false;
        }

        if (producto.codigo == codigoBuscado)
        {
            encontrado = true;
            break;
        }

        if (producto.codigo < codigoBuscado)
        {
            izquierda = medio + 1;
        }
        else
        {
            derecha = medio - 1;
        }
    }

    // -----------------------------------------------------
    // PRODUCTO NO ENCONTRADO
    // -----------------------------------------------------
    if (!encontrado)
    {
        cout << "\nProducto no encontrado.\n";

        fclose(archivo);
        return false;
    }

    // -----------------------------------------------------
    // CONTROL DE STOCK
    // -----------------------------------------------------
    if (producto.stockActual < cantidad)
    {
        cout << "\nStock insuficiente.\n";
        cout << "Stock disponible: "
             << producto.stockActual << endl;

        fclose(archivo);
        return false;
    }

    // Guardamos el precio antes de modificar.
    precioProducto = producto.precio;

    // Descontamos la cantidad vendida.
    producto.stockActual -= cantidad;

    // -----------------------------------------------------
    // EDICION IN SITU
    //
    // fread dejó el puntero inmediatamente después del
    // registro que acabamos de leer.
    //
    // Por eso retrocedemos sizeof(Producto) bytes para
    // volver al comienzo del registro y sobrescribirlo.
    // -----------------------------------------------------
    fseek(
        archivo,
        -static_cast<long>(sizeof(Producto)),
        SEEK_CUR
    );

    fwrite(
        &producto,
        sizeof(Producto),
        1,
        archivo
    );

    fclose(archivo);

    return true;
}

// ---------------------------------------------------------
// GUARDAR COMANDA
//
// Archivo: comandas_dd-mm-aaaa.dat
// Modo: ab -> agrega al final y crea si no existe.
// ---------------------------------------------------------
void guardarComanda(
    const char nombreArchivo[],
    Comanda nuevaComanda
)
{
    FILE* archivo = fopen(nombreArchivo, "ab");

    if (archivo == NULL)
    {
        cout << "\nERROR al abrir el archivo de comandas.\n";
        return;
    }

    fwrite(
        &nuevaComanda,
        sizeof(Comanda),
        1,
        archivo
    );

    fclose(archivo);
}

// ---------------------------------------------------------
// MAIN
// ---------------------------------------------------------
int main()
{
    char fecha[20];

    cout << "=====================================\n";
    cout << "     BUFFET ALBERT EINSTEIN\n";
    cout << "        CARGA DE VENTAS\n";
    cout << "=====================================\n\n";

    cout << "Ingrese la fecha (dd-mm-aaaa): ";
    cin.getline(fecha, 20);

    // -----------------------------------------------------
    // ARMAR NOMBRE DEL ARCHIVO DEL DIA
    // -----------------------------------------------------
    char nombreArchivo[50];

    strcpy(nombreArchivo, "comandas_");
    strcat(nombreArchivo, fecha);
    strcat(nombreArchivo, ".dat");

    char continuar = 'S';

    // -----------------------------------------------------
    // CARGA DE VENTAS
    // -----------------------------------------------------
    while (continuar == 'S' || continuar == 's')
    {
        int idMozo;
        char clave[20];

        cout << "\n----- NUEVA VENTA -----\n";

        // -------------------------------------------------
        // VALIDAR MOZO
        // -------------------------------------------------
        bool mozoValido = false;

        while (!mozoValido)
        {
            cout << "Ingrese ID del mozo: ";
            cin >> idMozo;

            cout << "Ingrese clave: ";
            cin >> clave;

            mozoValido = validarMozo(
                idMozo,
                clave
            );

            if (!mozoValido)
            {
                char intentar;

                cout << "¿Intentar nuevamente? (S/N): ";
                cin >> intentar;

                if (intentar != 'S' &&
                    intentar != 's')
                {
                    break;
                }
            }
        }

        // -------------------------------------------------
        // SI EL MOZO NO ES VALIDO
        // -------------------------------------------------
        if (!mozoValido)
        {
            cout << "\nVenta cancelada.\n";

            cout << "\n¿Desea cargar otra venta? (S/N): ";
            cin >> continuar;

            continue;
        }

        // -------------------------------------------------
        // INGRESAR PRODUCTO
        // -------------------------------------------------
        int codigoProducto;
        int cantidad;
        float precio;

        cout << "\nIngrese codigo del producto: ";
        cin >> codigoProducto;

        cout << "Ingrese cantidad: ";
        cin >> cantidad;

        // -------------------------------------------------
        // VALIDAR CANTIDAD
        // -------------------------------------------------
        if (cantidad <= 0)
        {
            cout << "\nLa cantidad debe ser mayor a cero.\n";

            cout << "\n¿Desea cargar otra venta? (S/N): ";
            cin >> continuar;

            continue;
        }

        // -------------------------------------------------
        // ACTUALIZAR STOCK Y REGISTRAR VENTA
        // -------------------------------------------------
        if (venderProducto(
                codigoProducto,
                cantidad,
                precio))
        {
            float comision =
                precio * cantidad * TASA_COMISION;

            Comanda nuevaComanda;

            nuevaComanda.idMozo = idMozo;
            nuevaComanda.codigoProducto = codigoProducto;
            nuevaComanda.cantidad = cantidad;
            nuevaComanda.comision = comision;

            guardarComanda(
                nombreArchivo,
                nuevaComanda
            );

            cout << "\nVENTA REGISTRADA CORRECTAMENTE\n";

            cout << "Precio unitario: $"
                 << precio << endl;

            cout << "Cantidad: "
                 << cantidad << endl;

            cout << "Comision: $"
                 << comision << endl;
        }

        // -------------------------------------------------
        // CONTINUAR
        // -------------------------------------------------
        cout << "\n¿Desea cargar otra venta? (S/N): ";
        cin >> continuar;
    }

    cout << "\nFin del programa.\n";

    return 0;
}
