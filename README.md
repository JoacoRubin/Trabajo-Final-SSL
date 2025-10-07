# COMPILADOR SSL - TRABAJO FINAL

## Descripción

Este proyecto implementa un compilador completo para un lenguaje de programación simplificado que incluye:

### Tipos de Datos
- **entero**: Números enteros
- **caracter**: Caracteres individuales (similar a char)
- **real**: Números decimales (similar a float)

### Estructuras de Control
- **si-sino**: Sentencias condicionales
- **mientras**: Bucles con condición al inicio
- **repetir-hasta**: Bucles con condición al final

## Estructura del Proyecto

```
├── compilador.h          # Definiciones y declaraciones principales
├── main.c               # Función principal y coordinación
├── lexer.c              # Analizador léxico (tokenización)
├── parser.c             # Analizador sintáctico (gramática)
├── semantic.c           # Analizador semántico (tipos y símbolos)
├── Makefile            # Automatización de compilación
├── INFORME_COMPILADOR.md # Informe técnico detallado
├── README.md           # Este archivo
├── utils.c              # Funciones auxiliares y utilidades
└── ejemplos/
    ├── ejemplo1_tipos.txt
    ├── ejemplo2_si_sino.txt
    ├── ejemplo3_mientras.txt
    ├── ejemplo4_repetir_hasta.txt
    ├── ejemplo5_programa_completo.txt
    ├── ejemplo_estructurado.txt
    ├── ejemplo_memoria_estructurada.txt
    └── casos_error.txt
```

## Compilación

### Usando Makefile (recomendado)
```bash
make all
```

### Compilación manual
```bash
gcc -Wall -Wextra -std=c99 -g -o compilador main.c lexer.c parser.c semantic.c
```

## Uso

### Ejecutar con código de ejemplo integrado
```bash
./compilador
```

### Ejecutar con un archivo específico
```bash
./compilador ejemplo1_tipos.txt
```

### Ejecutar casos de prueba
```bash
make test-tipos      # Prueba tipos de datos
make test-si         # Prueba sentencias SI-SINO
make test-mientras   # Prueba bucles MIENTRAS
make test-repetir    # Prueba bucles REPETIR-HASTA
make test-completo   # Programa completo
make test-errores    # Casos de error
```

## Sintaxis del Lenguaje

### Declaraciones
```
entero numero1, numero2;
real precio, descuento;
caracter letra, simbolo;
```

### Asignaciones
```
numero1 := 10;
precio := 99.99;
letra := 'A';
```

### Sentencia SI-SINO
```
si (condicion) {
    // sentencias
} sino {
    // sentencias alternativas
}
```

### Bucle MIENTRAS
```
mientras (condicion) {
    // sentencias
}
```

### Bucle REPETIR-HASTA
```
repetir {
    // sentencias
} hasta (condicion);
```

### Entrada y Salida
```
leer(variable);
escribir(expresion);
```

## Funcionalidades Implementadas

### Analizador Léxico
- Reconocimiento de palabras reservadas
- Identificadores y literales (enteros, reales, caracteres)
- Operadores aritméticos, relacionales y lógicos
- Manejo de comentarios (//)
- Información de ubicación (línea, columna)

### Analizador Sintáctico
- Análisis descendente recursivo
- Verificación de estructura gramatical
- Manejo de expresiones con precedencia
- Detección y reporte de errores sintácticos

### Analizador Semántico
- Tabla de símbolos con tipos
- Verificación de compatibilidad de tipos
- Detección de variables no declaradas
- Conversiones automáticas entre tipos compatibles
- Advertencias por pérdida de precisión

## Características Técnicas

### Programación Estructurada Implementada
- **Modularización Completa**: Código dividido en funciones atómicas y reutilizables
- **Separación de Responsabilidades**: Cada función tiene una responsabilidad única y bien definida
- **Funciones Auxiliares**: Archivo `utils.c` con funciones de utilidad general
- **Abstracción de Datos**: Estructuras bien definidas para tokens y símbolos
- **Control de Flujo Estructurado**: Uso exclusivo de secuencia, selección e iteración
- **Gestión de Memoria Estructurada**: Funciones especializadas para creación, manejo y liberación de memoria
- **Códigos de Retorno**: Todas las funciones críticas retornan códigos de éxito/error
- **Validación de Entrada**: Funciones específicas para validar argumentos y parámetros
- **Cleanup Verificado**: Sistema de limpieza que verifica la liberación completa de recursos

### Arquitectura Modular
- **lexer.c**: Análisis léxico con funciones especializadas por tipo de token
- **parser.c**: Análisis sintáctico con funciones independientes por construcción
- **semantic.c**: Análisis semántico con funciones modulares de verificación
- **utils.c**: Funciones auxiliares, validación, formato y diagnóstico
- **main.c**: Coordinación con funciones específicas por responsabilidad

### Gestión de Memoria y Punteros
- **Manejo Seguro de Punteros**: Verificación de NULL en todas las operaciones con punteros
- **Liberación Garantizada**: Sistema de cleanup que verifica la liberación completa de memoria
- **Gestión de Tabla de Símbolos**: Creación, inserción y liberación estructurada de símbolos
- **Prevención de Fugas**: Verificación automática de recursos no liberados
- **Reinicio de Estado**: Limpieza completa de variables globales

### Otras Características
- **Manejo de Errores**: Mensajes detallados con ubicación precisa y códigos de retorno
- **Tabla de Símbolos**: Gestión dinámica con estadísticas integradas y cleanup verificado
- **Verificación de Tipos**: Sistema completo con reglas de conversión y validación
- **Documentación**: Todas las funciones completamente documentadas con propósito y parámetros
- **Depuración**: Herramientas integradas para diagnóstico, depuración y verificación de memoria

## Ejemplos de Salida

El compilador muestra:
1. Código fuente analizado
2. Progreso del análisis
3. Tabla de símbolos final
4. Mensajes de error o éxito
5. Información de tipos y conversiones

## Autor

Trabajo Final - Sintaxis y Semántica de Lenguajes
Implementación de compilador con tipos carácter y real, y estructuras de control mientras, si y repetir-hasta.