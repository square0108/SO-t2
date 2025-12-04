# SO-t2
Tarea 2 curso Sistemas Operativos

## Instrucciones de Compilación
```bash
make clean
make
```

## Sintaxis de comandos

#### Parte 1: Sincronización con barrera

El siguiente comando ejecuta el test de barrera con 5 hebras y 3 etapas:
```bash
./barr 5 3
```

#### Parte 2: Simulador de memoria virtual

Para leer trazas de memoria en formato `.txt`, el simulador se ejecuta como:
```bash
./sim <núm. de marcos de página> <tamaño de página en bytes> [--verbose] <path relativo a traza de memoria>
```

- Si el tamaño de página ingresado no es una potencia de 2, se redondeará a la menor más cercana.
- `--verbose` imprimirá a la consola información adicional (hit/miss, números de página virtual y de marco utilizado)
- Las trazas disponibles se encuentran en `p2/traces/`.
