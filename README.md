# TP Integrador — Buffet Albert Einstein

Trabajo Práctico grupal de **Algoritmos y Estructuras de Datos** (UTN FRBA) —
unidad de **archivos binarios en C/C++**.

> **Grupo: 8** 
> **Integrantes: _(nombre — usuario de GitHub, uno por línea)
  Micael Ventrici Gonçalves - Micael1012
  Luca Gaston Liendro Torres - Luca022
  Santiago Luis Gamieta - Gamieta
  Estefania Aylen Mercado Segovia - AYLU
  ** 

La consigna completa está en **`enunciado.pdf`**. Leela antes de arrancar: el
cliente (Alberto) cuenta su problema a su manera y ustedes tienen que descubrir
cómo resolverlo con lo que vimos de archivos.

## Estructura del repo

```
.
├── enunciado.pdf              # la consigna
├── datos/                     # archivos PROVISTOS por la cátedra (no se tocan)
│   ├── comandas_historicas.dat
│   ├── inventario.dat
│   └── dump_datos_de_prueba.txt   # los dos .dat en texto, para verificar la lectura
├── .gitignore
│
│   # --- estos los crean USTEDES (no vienen en el repo base) ---
├── normalizacion.cpp          # cada uno con su propio main; NO hay main.cpp
├── ventas.cpp
├── cierre.cpp
└── resumen.cpp
```

Los 4 programas los escriben ustedes desde cero: el repo base no trae código.
Créenlos en la raíz del repo (como en el diagrama del enunciado).

## Cómo trabajar

Los programas leen y escriben archivos `.dat` en la carpeta donde se ejecutan.
Los archivos de `datos/` son los **originales** y no se modifican: copiálos a la
carpeta donde vas a compilar y correr, y trabajá sobre esas copias.

```bash
cp datos/comandas_historicas.dat datos/inventario.dat .
```

Compilar y correr cada programa (cada uno tiene su propio `main`, no hay `main.cpp`):

```bash
g++ -O2 -o normalizacion normalizacion.cpp
g++ -O2 -o ventas        ventas.cpp
g++ -O2 -o cierre        cierre.cpp
g++ -O2 -o resumen       resumen.cpp
```

Orden de ejecución (cada programa deja los archivos que usa el siguiente):

```
./normalizacion     # historicas + inventario  ->  mozos.dat, comandas_dd-mm-aaaa.dat (varios), inventario actualizado
./ventas            # carga interactiva de la semana  ->  agrega a las planillas del día
./cierre            # junta los días de la semana      ->  comandas_semana_sX-mm.dat
./resumen           # imprime el resumen por mozo + total del buffet
```

## Verificar que leen bien el binario

`datos/dump_datos_de_prueba.txt` tiene el contenido de los dos archivos provistos
en texto legible. Si tu programa imprime otra cosa al leerlos, revisá los `struct`
(orden de campos / tamaños): tienen que dar `sizeof(ComandaHistorica)=76` y
`sizeof(Producto)=64`.

## Qué se entrega

- Este mismo repo (uno por grupo), con los **4 programas** `.cpp` en la raíz
  (cada uno con su propio `main`, no hay `main.cpp`).
- El **README** con el grupo, los integrantes (nombre + usuario de GitHub) y, si
  hace falta, cualquier aclaración de cómo correrlo.
- Los `.dat` de `datos/` son los **provistos por la cátedra** y ya vienen en el
  repo: no los borren ni agreguen otros. Cualquier `.dat` que ustedes copien a la
  raíz o generen al correr **no** se versiona (el `.gitignore` ya los deja afuera).
  La cátedra corre sus programas sobre su propio dataset.
- **Commits repartidos:** cada integrante tiene que tener commits propios a lo
  largo del trabajo. El historial es parte de lo que se mira para la defensa
  individual (no vale un único commit final ni que suba todo una sola persona).

  ## Reparto de trabajo

- **normalizacion.cpp** — Luca Gaston Liendro Torres
- **ventas.cpp** — Estefania Aylen Mercado Segovi
- **cierre.cpp** — Micael Ventrici Gonçalves
- **resumen.cpp** — Santiago Luis Gamieta

## Explicación de las partes clave

### cierre.cpp: 
Junta las planillas diarias (`comandas_dd-mm-aaaa.dat`) en una planilla semanal (`comandas_semana_sX-mm.dat`), ordenada por mozo.
- Las fechas de la semana se piden al usuario (no se escanea la carpeta), por ser mas simple.
- El ordenamiento se hace en memoria (burbujeo por `idMozo`), no sobre el archivo, por eficiencia para no acceder al disco en cada paso.
- Semana y mes se calculan solos a partir de la primera fecha cargada: `semana = (día - 1) / 7 + 1`. 
  
### normalizacion.cpp
Arma la lista de mozos numerados a partir de las 30 ventas históricas (desordenadas) y separa las ventas en planillas por día, dejando el inventario actualizado.

- La lista de mozos se arma con búsqueda lineal (todavía no hay nada ordenado por nombre), asignando un id correlativo a cada uno nuevo.
- Las ventas de cada día se ordenan por `idMozo` con inserción, alcanza de sobra para el volumen que maneja el TP.
- El stock se descuenta con búsqueda binaria sobre `inventario.dat`, que ya viene ordenado por código.
- La clave inicial de cada mozo (su número) se guarda cifrada con corrimiento (K=5), nunca en texto plano.
- Los `structs` (`Mozo`, `Producto`, `Comanda`, etc.) y la constante `K_CIFRADO` se centralizaron en `common.h`, para que los 4 programas compartan exactamente el mismo formato binario y no haya desajustes entre ellos.
### ventas.cpp
El programa junta las planillas diarias ("comandas_dd-mm-aaaa.dat") en una planilla semanal ("comandas_semana_sX-mm.dat") y las ordena por "idMozo" mediante burbujeo en memoria.
Las fechas de la semana se ingresan manualmente para evitar escanear la carpeta.
La semana y el mes se calculan automáticamente a partir de la primera fecha ingresada.

### resumen.cpp


