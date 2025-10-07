#include "compilador.h"

/* ========== FUNCIONES DE UTILIDAD GENERAL ========== */

/**
 * Convierte un tipo de dato a su representacion en cadena
 * @param type: Tipo de dato a convertir
 * @param typeStr: Buffer donde almacenar el resultado
 */
void dataTypeToString(DataType type, char* typeStr) {
    static const char* typeNames[] = {"entero", "caracter", "real", "error"};
    strcpy(typeStr, (type >= TYPE_ENTERO && type <= TYPE_ERROR) ? typeNames[type] : "desconocido");
}

/**
 * Convierte un token a su representacion en cadena
 * @param tokenType: Tipo de token a convertir
 * @param tokenStr: Buffer donde almacenar el resultado
 */
void tokenTypeToString(TokenType tokenType, char* tokenStr) {
    static const char* tokenNames[] = {
        "IDENTIFICADOR", "NUMERO", "CARACTER", "REAL", "CADENA",
        "TIPO_ENTERO", "TIPO_CARACTER", "TIPO_REAL",
        "SI", "SINO", "MIENTRAS", "REPETIR", "HASTA", "LEER", "ESCRIBIR",
        "ASIGNACION", "SUMA", "RESTA", "MULTIPLICACION", "DIVISION", "MODULO",
        "MENOR", "MAYOR", "MENOR_IGUAL", "MAYOR_IGUAL", "IGUAL", "DIFERENTE",
        "Y", "O", "NO", "PARENTESIS_IZQ", "PARENTESIS_DER", "LLAVE_IZQ", 
        "LLAVE_DER", "PUNTO_COMA", "COMA", "FIN_ARCHIVO", "ERROR"
    };
    
    if (tokenType >= 0 && tokenType < sizeof(tokenNames)/sizeof(tokenNames[0])) {
        strcpy(tokenStr, tokenNames[tokenType]);
    } else {
        strcpy(tokenStr, "DESCONOCIDO");
    }
}

/* ========== FUNCIONES DE VALIDACION ========== */

/**
 * Verifica si un identificador tiene un formato valido
 * @param identifier: Identificador a verificar
 * @return: 1 si es valido, 0 en caso contrario
 */
int isValidIdentifier(char* identifier) {
    if (identifier == NULL || strlen(identifier) == 0) {
        return 0;
    }
    
    // Debe comenzar con letra o guion bajo
    if (!isLetter(identifier[0])) {
        return 0;
    }
    
    // Resto de caracteres deben ser letras, digitos o guion bajo
    for (int i = 1; identifier[i] != '\0'; i++) {
        if (!isLetter(identifier[i]) && !isDigit(identifier[i])) {
            return 0;
        }
    }
    
    // Verificar que no sea palabra reservada
    return (isKeyword(identifier) == TOKEN_IDENTIFIER);
}

/**
 * Verifica si un numero real tiene formato valido
 * @param realStr: Cadena que representa el numero real
 * @return: 1 si es valido, 0 en caso contrario
 */
int isValidRealNumber(char* realStr) {
    if (realStr == NULL || strlen(realStr) == 0) {
        return 0;
    }
    
    int hasDot = 0;
    int hasDigits = 0;
    
    for (int i = 0; realStr[i] != '\0'; i++) {
        if (isDigit(realStr[i])) {
            hasDigits = 1;
        } else if (realStr[i] == '.') {
            if (hasDot) {
                return 0; // Mas de un punto decimal
            }
            hasDot = 1;
        } else {
            return 0; // Caracter invalido
        }
    }
    
    return hasDigits && hasDot;
}

/* ========== FUNCIONES DE FORMATO Y PRESENTACION ========== */

/**
 * Formatea un valor de simbolo como cadena
 * @param symbol: Simbolo cuyo valor se quiere formatear
 * @param valueStr: Buffer donde almacenar el resultado
 */
void formatSymbolValue(Symbol* symbol, char* valueStr) {
    if (!symbol || !symbol->initialized) {
        strcpy(valueStr, "N/A");
    } else if (symbol->type == TYPE_ENTERO) {
        sprintf(valueStr, "%d", symbol->value.intValue);
    } else if (symbol->type == TYPE_CARACTER) {
        sprintf(valueStr, "'%c'", symbol->value.charValue);
    } else if (symbol->type == TYPE_REAL) {
        sprintf(valueStr, "%.2f", symbol->value.realValue);
    } else {
        strcpy(valueStr, "N/A");
    }
}

/**
 * Formatea informacion de ubicacion (linea y columna)
 * @param line: Numero de linea
 * @param column: Numero de columna
 * @param locationStr: Buffer donde almacenar el resultado
 */
void formatLocation(int line, int column, char* locationStr) {
    sprintf(locationStr, "linea %d, columna %d", line, column);
}

/* ========== FUNCIONES DE DIAGNOSTICO ========== */

/**
 * Cuenta el numero de simbolos en la tabla de simbolos
 * @return: Numero de simbolos
 */
int countSymbols() {
    int count = 0;
    Symbol* current = symbolTable;
    
    while (current != NULL) {
        count++;
        current = current->next;
    }
    
    return count;
}

/**
 * Muestra estadisticas basicas de la tabla de simbolos  
 */
void displaySymbolTableStatistics() {
    int total = 0, initialized = 0, integers = 0, reals = 0, chars = 0;
    Symbol* current = symbolTable;
    
    while (current != NULL) {
        total++;
        if (current->initialized) initialized++;
        if (current->type == TYPE_ENTERO) integers++;
        else if (current->type == TYPE_REAL) reals++;
        else if (current->type == TYPE_CARACTER) chars++;
        current = current->next;
    }
    
    printf("\n=== ESTADISTICAS ===\n");
    printf("Variables: %d (Enteras: %d, Reales: %d, Caracter: %d)\n", 
           total, integers, reals, chars);
    printf("Inicializadas: %d | No inicializadas: %d\n", 
           initialized, total - initialized);
}

