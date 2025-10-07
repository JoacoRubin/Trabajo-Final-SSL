#include "compilador.h"

/* Variables globales del analizador semantico */
Symbol* symbolTable = NULL;

/**
 * Inicializa el analizador semantico
 */
void initSemantic() {
    symbolTable = NULL;
}

/**
 * Busca un simbolo en la tabla de simbolos
 * @param name: Nombre del simbolo a buscar
 * @return: Puntero al simbolo encontrado o NULL si no existe
 */
Symbol* lookupSymbol(char* name) {
    Symbol* current = symbolTable;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/**
 * Inicializa los valores por defecto de un simbolo segun su tipo
 * @param symbol: Puntero al simbolo a inicializar
 * @param type: Tipo de dato del simbolo
 */
void initializeSymbolValue(Symbol* symbol, DataType type) {
    if (symbol == NULL) return;
    
    symbol->value.intValue = 0; // Inicializar por defecto
    if (type == TYPE_CARACTER) symbol->value.charValue = '\0';
    else if (type == TYPE_REAL) symbol->value.realValue = 0.0f;
}

/**
 * Crea y configura un nuevo simbolo
 * @param name: Nombre del simbolo
 * @param type: Tipo de dato del simbolo
 * @return: Puntero al simbolo creado o NULL si hay error
 */
Symbol* createSymbol(char* name, DataType type) {
    if (name == NULL || strlen(name) == 0) {
        return NULL;
    }
    
    Symbol* newSymbol = (Symbol*)malloc(sizeof(Symbol));
    if (newSymbol == NULL) {
        printf("ERROR CRITICO: No se pudo asignar memoria para el simbolo '%s'\n", name);
        return NULL;
    }
    
    // Verificar que el nombre no sea demasiado largo
    if (strlen(name) >= MAX_IDENTIFIER_LENGTH) {
        printf("ERROR: Nombre de variable demasiado largo: '%s'\n", name);
        free(newSymbol);
        return NULL;
    }
    
    strcpy(newSymbol->name, name);
    newSymbol->type = type;
    newSymbol->initialized = 0;
    newSymbol->next = NULL;
    
    initializeSymbolValue(newSymbol, type);
    
    return newSymbol;
}

/**
 * Inserta un simbolo en la tabla de simbolos
 * @param symbol: Simbolo a insertar
 * @return: 1 si se inserto correctamente, 0 en caso contrario
 */
int insertSymbolInTable(Symbol* symbol) {
    if (symbol == NULL) {
        return 0;
    }
    
    symbol->next = symbolTable;
    symbolTable = symbol;
    return 1;
}

/**
 * Inserta un nuevo simbolo en la tabla de simbolos
 * @param name: Nombre del simbolo
 * @param type: Tipo de dato del simbolo
 * @return: Puntero al simbolo insertado o NULL si ya existe o hay error
 */
Symbol* insertSymbol(char* name, DataType type) {
    // Verificar si el simbolo ya existe
    if (lookupSymbol(name) != NULL) {
        return NULL; // Ya existe
    }
    
    // Crear nuevo simbolo
    Symbol* newSymbol = createSymbol(name, type);
    if (newSymbol == NULL) {
        return NULL;
    }
    
    // Insertar en la tabla
    if (!insertSymbolInTable(newSymbol)) {
        free(newSymbol);
        return NULL;
    }
    
    return newSymbol;
}

/**
 * Obtiene el tipo de dato correspondiente a un tipo de token
 * @param type: Tipo de token
 * @return: Tipo de dato correspondiente
 */
DataType getTokenDataType(TokenType type) {
    return (type == TOKEN_ENTERO || type == TOKEN_NUMBER) ? TYPE_ENTERO :
           (type == TOKEN_CARACTER || type == TOKEN_CHAR_LITERAL) ? TYPE_CARACTER :
           (type == TOKEN_REAL || type == TOKEN_REAL_LITERAL) ? TYPE_REAL : TYPE_ERROR;
}

/**
 * Verifica el tipo de una expresión simple (para análisis básico)
 * Esta función proporciona una verificación básica de tipos
 * @return: Tipo de dato de la expresión
 */
DataType checkExpressionType() {
    // Esta es una implementación simplificada
    // En un compilador real, se necesitaría un análisis más complejo
    
    // Para este ejemplo, asumimos que la expresión es del tipo del último token procesado
    if (currentToken.type == TOKEN_NUMBER) {
        return TYPE_ENTERO;
    }
    
    if (currentToken.type == TOKEN_REAL_LITERAL) {
        return TYPE_REAL;
    }
    
    if (currentToken.type == TOKEN_CHAR_LITERAL) {
        return TYPE_CARACTER;
    }
    
    if (currentToken.type == TOKEN_IDENTIFIER) {
        Symbol* var = lookupSymbol(currentToken.lexeme);
        if (var != NULL) {
            return var->type;
        }
        return TYPE_ERROR;
    }
    
    // Tipo por defecto para expresiones aritméticas
    return TYPE_ENTERO;
}

/**
 * Verifica compatibilidad para asignacion a variable entera
 * @param var: Variable entera
 * @param exprType: Tipo de la expresión
 */
void checkIntegerAssignment(Symbol* var, DataType exprType) {
    if (exprType == TYPE_ENTERO) {
        return; // Compatibilidad perfecta
    }
    
    if (exprType == TYPE_REAL) {
        printf("ADVERTENCIA: Asignación de real a entero puede causar pérdida de precisión\n");
    } else if (exprType == TYPE_CARACTER) {
        printf("ADVERTENCIA: Asignacion de caracter a entero (conversion automatica)\n");
    } else {
        char message[100];
        sprintf(message, "Incompatibilidad de tipos: no se puede asignar tipo %d a variable entera '%s'", 
               exprType, var->name);
        semanticError(message);
    }
}

/**
 * Verifica compatibilidad para asignacion a variable real
 * @param var: Variable real
 * @param exprType: Tipo de la expresión
 */
void checkRealAssignment(Symbol* var, DataType exprType) {
    if (exprType == TYPE_REAL) {
        return; // Compatibilidad perfecta
    }
    
    if (exprType == TYPE_ENTERO) {
        printf("INFO: Conversion automatica de entero a real\n");
    } else if (exprType == TYPE_CARACTER) {
        printf("ADVERTENCIA: Asignacion de caracter a real (conversion automatica)\n");
    } else {
        char message[100];
        sprintf(message, "Incompatibilidad de tipos: no se puede asignar tipo %d a variable real '%s'", 
               exprType, var->name);
        semanticError(message);
    }
}

/**
 * Verifica compatibilidad para asignacion a variable caracter
 * @param var: Variable caracter
 * @param exprType: Tipo de la expresión
 */
void checkCharacterAssignment(Symbol* var, DataType exprType) {
    if (exprType == TYPE_CARACTER) {
        return; // Compatibilidad perfecta
    }
    
    if (exprType == TYPE_ENTERO) {
        printf("ADVERTENCIA: Asignacion de entero a caracter (conversion automatica)\n");
    } else {
        char message[100];
        sprintf(message, "Incompatibilidad de tipos: no se puede asignar tipo %d a variable caracter '%s'", 
               exprType, var->name);
        semanticError(message);
    }
}

/**
 * Ejecuta las verificaciones de tipo específicas según el tipo de variable
 * @param var: Variable que recibe la asignación
 * @param exprType: Tipo de la expresión asignada
 */
void performTypeCompatibilityCheck(Symbol* var, DataType exprType) {
    if (var->type == TYPE_ENTERO) {
        checkIntegerAssignment(var, exprType);
    } else {
        if (var->type == TYPE_REAL) {
            checkRealAssignment(var, exprType);
        } else {
            if (var->type == TYPE_CARACTER) {
                checkCharacterAssignment(var, exprType);
            } else {
                semanticError("Tipo de variable desconocido");
            }
        }
    }
}

/**
 * Marca una variable como inicializada
 * @param var: Variable a marcar como inicializada
 */
void markVariableAsInitialized(Symbol* var) {
    if (var != NULL) {
        var->initialized = 1;
    }
}

/**
 * Verifica la compatibilidad de tipos en asignaciones
 * @param var: Variable que recibe la asignación
 * @param exprType: Tipo de la expresión asignada
 */
void checkAssignmentCompatibility(Symbol* var, DataType exprType) {
    if (var == NULL) {
        return;
    }
    
    performTypeCompatibilityCheck(var, exprType);
    markVariableAsInitialized(var);
}

/**
 * Maneja errores semánticos
 * @param message: Mensaje descriptivo del error
 */
void semanticError(char* message) {
    hasError = 1;
    printf("ERROR SEMANTICO en línea %d: %s\n", currentLine, message);
}

/**
 * Verifica que todas las variables utilizadas estén inicializadas
 * @param name: Nombre de la variable a verificar
 */
void checkVariableInitialization(char* name) {
    Symbol* var = lookupSymbol(name);
    if (var != NULL && !var->initialized) {
        char message[100];
        sprintf(message, "Variable '%s' utilizada sin inicializar", name);
        printf("ADVERTENCIA: %s\n", message);
    }
}

/**
 * Verifica operaciones aritméticas entre enteros
 * @param operator: Operador aritmético
 * @return: Tipo resultado de la operación
 */
DataType checkIntegerArithmetic(TokenType operator) {
    if (operator == TOKEN_DIVIDE) {
        printf("ADVERTENCIA: División entera puede causar pérdida de precisión\n");
    }
    return TYPE_ENTERO;
}

/**
 * Verifica si una operación involucra tipos reales
 * @param leftType: Tipo del operando izquierdo
 * @param rightType: Tipo del operando derecho
 * @return: 1 si involucra reales, 0 en caso contrario
 */
int operationInvolvesReal(DataType leftType, DataType rightType) {
    return (leftType == TYPE_REAL && rightType == TYPE_ENTERO) ||
           (leftType == TYPE_ENTERO && rightType == TYPE_REAL) ||
           (leftType == TYPE_REAL && rightType == TYPE_REAL);
}

/**
 * Verifica operaciones aritmeticas con caracteres
 * @param leftType: Tipo del operando izquierdo
 * @param rightType: Tipo del operando derecho
 * @param operator: Operador aritmético
 * @return: Tipo resultado de la operación o TYPE_ERROR si no es válida
 */
DataType checkCharacterArithmetic(DataType leftType, DataType rightType, TokenType operator) {
    // Operaciones entre caracteres
    if (leftType == TYPE_CARACTER && rightType == TYPE_CARACTER) {
        if (operator == TOKEN_PLUS || operator == TOKEN_MINUS) {
            return TYPE_ENTERO; // Aritmética de caracteres produce enteros
        }
    }
    
    // Operaciones entre carácter y entero
    if ((leftType == TYPE_CARACTER && rightType == TYPE_ENTERO) ||
        (leftType == TYPE_ENTERO && rightType == TYPE_CARACTER)) {
        return TYPE_ENTERO;
    }
    
    return TYPE_ERROR;
}

/**
 * Verifica la validez de una operación aritmética
 * @param leftType: Tipo del operando izquierdo
 * @param rightType: Tipo del operando derecho
 * @param operator: Operador aritmético
 * @return: Tipo resultado de la operación
 */
DataType validateArithmeticOperation(DataType leftType, DataType rightType, TokenType operator) {
    // Operaciones entre enteros
    if (leftType == TYPE_ENTERO && rightType == TYPE_ENTERO) {
        return checkIntegerArithmetic(operator);
    }
    
    // Operaciones con reales
    if (operationInvolvesReal(leftType, rightType)) {
        return TYPE_REAL;
    }
    
    // Operaciones con caracteres
    DataType charResult = checkCharacterArithmetic(leftType, rightType, operator);
    if (charResult != TYPE_ERROR) {
        return charResult;
    }
    
    // Operación no válida
    return TYPE_ERROR;
}

/**
 * Verifica la compatibilidad de tipos en operaciones aritméticas
 * @param leftType: Tipo del operando izquierdo
 * @param rightType: Tipo del operando derecho
 * @param operator: Operador aritmético
 * @return: Tipo resultado de la operación
 */
DataType checkArithmeticOperation(DataType leftType, DataType rightType, TokenType operator) {
    DataType resultType = validateArithmeticOperation(leftType, rightType, operator);
    
    if (resultType == TYPE_ERROR) {
        char message[100];
        sprintf(message, "Operación aritmética no válida entre tipos %d y %d", leftType, rightType);
        semanticError(message);
    }
    
    return resultType;
}

/**
 * Verifica la compatibilidad de tipos en operaciones relacionales
 * @param leftType: Tipo del operando izquierdo
 * @param rightType: Tipo del operando derecho
 * @return: 1 si la comparación es válida, 0 en caso contrario
 */
int checkRelationalOperation(DataType leftType, DataType rightType) {
    // Las comparaciones son válidas entre tipos compatibles
    if (leftType == rightType) {
        return 1;
    }
    
    // Comparaciones entre tipos numéricos son válidas
    if ((leftType == TYPE_ENTERO || leftType == TYPE_REAL) &&
        (rightType == TYPE_ENTERO || rightType == TYPE_REAL)) {
        return 1;
    }
    
    // Comparaciones entre carácter y entero son válidas
    if ((leftType == TYPE_CARACTER && rightType == TYPE_ENTERO) ||
        (leftType == TYPE_ENTERO && rightType == TYPE_CARACTER)) {
        return 1;
    }
    
    char message[100];
    sprintf(message, "Comparación no válida entre tipos %d y %d", leftType, rightType);
    semanticError(message);
    return 0;
}