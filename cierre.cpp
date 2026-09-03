#include <iostream>
#include <cstdio>
#include <cstring>
#include "common.h"

using namespace std;

const int MAX_COMANDAS = 1000; // ajustalo si hace falta

int main() {
    Comanda todasLasComandas[MAX_COMANDAS];
    int totalComandas = 0;
    char primeraFecha[11];

    int cantidadDias;
    cout << "¿Cuantos dias tiene la semana a cerrar? ";
    cin >> cantidadDias;

    for (int i = 0; i < cantidadDias; i++) {
        char fecha[11];
        cout << "Fecha del dia " << (i + 1) << " (DD-MM-AAAA): ";
        cin >> fecha;
        if (i == 0) {
        strcpy(primeraFecha, fecha);   
}

        char archivoDelDia[50];
        
        sprintf(archivoDelDia,"comandas_%s.dat",fecha); 
        FILE* f = fopen(archivoDelDia,"rb");
        
        if(f == NULL){
		cout<<"Se omite debido a que no se encontro nada del dia: "<<fecha <<endl;
		}else{
			Comanda c;
		while(fread(&c,sizeof(Comanda),1,f) == 1){
			cout<<c.idMozo<<" - "<<c.codigoProducto<<" - "<<c.cantidad<<c.comision<<endl;//Esto es para cuando pruebe ver que funciona, no forma parte
			todasLasComandas[totalComandas] = c;
			totalComandas++;
		}
		fclose(f);
    }
}

    for (int i = 0; i < totalComandas - 1; i++) {
    for (int j = 0; j < totalComandas - 1 - i; j++) {
        if (todasLasComandas[j].idMozo > todasLasComandas[j+1].idMozo) {
        	Comanda aux;
            aux=todasLasComandas[j];
            todasLasComandas[j]=todasLasComandas[j+1];
            todasLasComandas[j+1]=aux;
        }
    }
}

    // ACÁ: armar el nombre del archivo semanal y escribirlo
    int dia, mes, anio;
    sscanf(primeraFecha, "%d-%d-%d", &dia, &mes, &anio);
    int semana = (dia - 1) / 7 + 1;
    char nombreSemanal[50];
    sprintf(nombreSemanal, "comandas_semana_s%d-%02d.dat", semana, mes);
    FILE* fSemanal = fopen(nombreSemanal, "wb");
    for (int i = 0; i < totalComandas; i++) {
    fwrite(&todasLasComandas[i], sizeof(Comanda), 1, fSemanal);
    }
    fclose(fSemanal);
    cout << nombreSemanal << " generado con " << totalComandas << " comandas." << endl;

    cout << "Listo." << endl;
    return 0;
}