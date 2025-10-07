#include "compilador.h"

/* Variables globales del analizador sintactico */
int hasError = 0;

/**
 * Inicializa el analizador sintactico
 */
void initParser() {
    hasError = 0;
}

/**
 * Verifica si el token actual coincide con el esperado y avanza al siguiente token
 * @param expected: Tipo de token esperado
 */
void match(TokenType expected) {
    if (currentToken.type == expected) {
        currentToken = getNextToken();
    } else {
        char message[100];
        sprintf(message, "Se esperaba token tipo %d, se encontro %d", expected, currentToken.type);
        syntaxError(message);
    }
}

/**
 * Maneja errores sintacticos
 * @param message: Mensaje descriptivo del error
 */
void syntaxError(char* message) {
    hasError = 1;
    printf("ERROR SINTACTICO en linea %d, columna %d: %s\n", currentToken.line, currentToken.column, message);
    printf("Token actual: %s\n", currentToken.lexeme);
}

/**
 * Analiza el programa completo
 * Gramatica: Programa -> { Declaracion | Sentencia }
 */
void parseProgram() {
    printf("Iniciando analisis sintactico...\n");
    
    while (currentToken.type != TOKEN_EOF && !hasError) {
        if (currentToken.type == TOKEN_ENTERO || currentToken.type == TOKEN_CARACTER || currentToken.type == TOKEN_REAL) {
            parseDeclaration();
        } else {
            parseStatement();
        }
    }
    
    if (!hasError) {
        printf("Analisis sintactico completado exitosamente.\n");
    }
}

/**
 * Obtiene el tipo de dato de un token de tipo
 * @return: Tipo de dato correspondiente
 */
DataType getDataTypeFromToken() {
    if (currentToken.type == TOKEN_ENTERO) {
        return TYPE_ENTERO;
    }
    
    if (currentToken.type == TOKEN_CARACTER) {
        return TYPE_CARACTER;
    }
    
    if (currentToken.type == TOKEN_REAL) {
        return TYPE_REAL;
    }
    
    return TYPE_ERROR;
}

/**
 * Verifica y procesa un tipo de dato
 * @return: Tipo de dato encontrado
 */
DataType parseDataType() {
    DataType varType = getDataTypeFromToken();
    
    if (varType == TYPE_ERROR) {
        syntaxError("Se esperaba tipo de dato (entero, caracter, real)");
        return TYPE_ERROR;
    }
    
    match(currentToken.type);
    return varType;
}

/**
 * Procesa un identificador en una declaracion
 * @param varType: Tipo de dato de la variable
 */
void processVariableDeclaration(DataType varType) {
    if (currentToken.type != TOKEN_IDENTIFIER) {
        syntaxError("Se esperaba identificador");
        return;
    }
    
    Symbol* symbol = insertSymbol(currentToken.lexeme, varType);
    if (symbol == NULL) {
        char message[100];
        sprintf(message, "Variable '%s' ya declarada", currentToken.lexeme);
        syntaxError(message);
    }
    match(TOKEN_IDENTIFIER);
}

/**
 * Procesa una lista de identificadores separados por comas
 * @param varType: Tipo de dato de las variables
 */
void parseIdentifierList(DataType varType) {
    processVariableDeclaration(varType);
    
    // Procesar variables adicionales separadas por coma
    while (currentToken.type == TOKEN_COMMA) {
        match(TOKEN_COMMA);
        processVariableDeclaration(varType);
    }
}

/**
 * Analiza declaraciones de variables
 * Gramatica: Declaracion -> TipoDato Identificador { , Identificador } ;
 */
void parseDeclaration() {
    DataType varType = parseDataType();
    
    if (varType != TYPE_ERROR) {
        parseIdentifierList(varType);
    }
    
    match(TOKEN_SEMICOLON);
}

/**
 * Analiza sentencias del programa
 * Gramatica: Sentencia -> Asignacion | SentenciaSi | SentenciaMientras | SentenciaRepetir | SentenciaLeer | SentenciaEscribir
 */
void parseStatement() {
    if (currentToken.type == TOKEN_IDENTIFIER) {
        parseAssignment();
    } else {
        if (currentToken.type == TOKEN_SI) {
            parseIfStatement();
        } else {
            if (currentToken.type == TOKEN_MIENTRAS) {
                parseWhileStatement();
            } else {
                if (currentToken.type == TOKEN_REPETIR) {
                    parseRepeatStatement();
                } else {
                    if (currentToken.type == TOKEN_LEER) {
                        parseReadStatement();
                    } else {
                        if (currentToken.type == TOKEN_ESCRIBIR) {
                            parseWriteStatement();
                        } else {
                            syntaxError("Sentencia no valida");
                            currentToken = getNextToken(); // Intentar recuperacion
                        }
                    }
                }
            }
        }
    }
}

/**
 * Verifica la variable en una asignacion
 * @return: Puntero al simbolo de la variable o NULL si hay error
 */
Symbol* processAssignmentVariable() {
    if (currentToken.type != TOKEN_IDENTIFIER) {
        syntaxError("Se esperaba identificador en asignacion");
        return NULL;
    }
    
    Symbol* var = lookupSymbol(currentToken.lexeme);
    if (var == NULL) {
        char message[100];
        sprintf(message, "Variable '%s' no declarada", currentToken.lexeme);
        semanticError(message);
    }
    
    match(TOKEN_IDENTIFIER);
    return var;
}

/**
 * Verifica la compatibilidad de tipos en la asignacion
 * @param var: Variable que recibe la asignacion
 */
void checkAssignmentSemantics(Symbol* var) {
    if (var != NULL) {
        DataType exprType = checkExpressionType();
        checkAssignmentCompatibility(var, exprType);
    }
}

/**
 * Analiza sentencias de asignacion
 * Gramática: Asignacion -> Identificador := Expresion ;
 */
void parseAssignment() {
    Symbol* var = processAssignmentVariable();
    match(TOKEN_ASSIGN);
    parseExpression();
    checkAssignmentSemantics(var);
    match(TOKEN_SEMICOLON);
}

/**
 * Analiza un bloque de sentencias entre llaves
 */
void parseBlock() {
    match(TOKEN_LBRACE);
    
    while (currentToken.type != TOKEN_RBRACE && currentToken.type != TOKEN_EOF && !hasError) {
        parseStatement();
    }
    
    match(TOKEN_RBRACE);
}

/**
 * Analiza la condición de una sentencia condicional
 */
void parseIfCondition() {
    match(TOKEN_LPAREN);
    parseCondition();
    match(TOKEN_RPAREN);
}

/**
 * Analiza el bloque SINO opcional
 */
void parseElseBlock() {
    if (currentToken.type == TOKEN_SINO) {
        match(TOKEN_SINO);
        parseBlock();
    }
}

/**
 * Analiza sentencias condicionales SI
 * Gramática: SentenciaSi -> si ( Condicion ) { Sentencia* } [ sino { Sentencia* } ]
 */
void parseIfStatement() {
    match(TOKEN_SI);
    parseIfCondition();
    parseBlock();
    parseElseBlock();
}

/**
 * Analiza la condición de un bucle MIENTRAS
 */
void parseWhileCondition() {
    match(TOKEN_LPAREN);
    parseCondition();
    match(TOKEN_RPAREN);
}

/**
 * Analiza sentencias de bucle MIENTRAS
 * Gramática: SentenciaMientras -> mientras ( Condicion ) { Sentencia* }
 */
void parseWhileStatement() {
    match(TOKEN_MIENTRAS);
    parseWhileCondition();
    parseBlock();
}

/**
 * Analiza la condición final de un bucle REPETIR HASTA
 */
void parseUntilCondition() {
    match(TOKEN_HASTA);
    match(TOKEN_LPAREN);
    parseCondition();
    match(TOKEN_RPAREN);
    match(TOKEN_SEMICOLON);
}

/**
 * Analiza sentencias de bucle REPETIR HASTA
 * Gramática: SentenciaRepetir -> repetir { Sentencia* } hasta ( Condicion ) ;
 */
void parseRepeatStatement() {
    match(TOKEN_REPETIR);
    parseBlock();
    parseUntilCondition();
}

/**
 * Verifica y procesa el identificador en una sentencia LEER
 */
void processReadIdentifier() {
    if (currentToken.type != TOKEN_IDENTIFIER) {
        syntaxError("Se esperaba identificador en sentencia leer");
        return;
    }
    
    Symbol* var = lookupSymbol(currentToken.lexeme);
    if (var == NULL) {
        char message[100];
        sprintf(message, "Variable '%s' no declarada", currentToken.lexeme);
        semanticError(message);
    } else {
        var->initialized = 1; // Marcar como inicializada después de leer
    }
    
    match(TOKEN_IDENTIFIER);
}

/**
 * Analiza la estructura de paréntesis y contenido para LEER
 */
void parseReadParameters() {
    match(TOKEN_LPAREN);
    processReadIdentifier();
    match(TOKEN_RPAREN);
}

/**
 * Analiza la estructura de paréntesis y contenido para ESCRIBIR
 */
void parseWriteParameters() {
    match(TOKEN_LPAREN);
    parseExpression();
    match(TOKEN_RPAREN);
}

/**
 * Analiza sentencias de lectura
 * Gramática: SentenciaLeer -> leer ( Identificador ) ;
 */
void parseReadStatement() {
    match(TOKEN_LEER);
    parseReadParameters();
    match(TOKEN_SEMICOLON);
}

/**
 * Analiza sentencias de escritura
 * Gramática: SentenciaEscribir -> escribir ( Expresion ) ;
 */
void parseWriteStatement() {
    match(TOKEN_ESCRIBIR);
    parseWriteParameters();
    match(TOKEN_SEMICOLON);
}

/**
 * Analiza expresiones aritméticas
 * Gramática: Expresion -> Termino { ( + | - ) Termino }
 */
void parseExpression() {
    parseTerm();
    
    while (currentToken.type == TOKEN_PLUS || currentToken.type == TOKEN_MINUS) {
        match(currentToken.type);
        parseTerm();
    }
}

/**
 * Analiza términos de expresiones
 * Gramática: Termino -> Factor { ( * | / | % ) Factor }
 */
void parseTerm() {
    parseFactor();
    
    while (currentToken.type == TOKEN_MULTIPLY || currentToken.type == TOKEN_DIVIDE || currentToken.type == TOKEN_MOD) {
        match(currentToken.type);
        parseFactor();
    }
}

/**
 * Analiza factores de expresiones
 * Gramática: Factor -> Identificador | Numero | NumeroReal | CaracterLiteral | ( Expresion )
 */
void parseFactor() {
    if (currentToken.type == TOKEN_IDENTIFIER) {
        Symbol* var = lookupSymbol(currentToken.lexeme);
        if (var == NULL) {
            char message[100];
            sprintf(message, "Variable '%s' no declarada", currentToken.lexeme);
            semanticError(message);
        }
        match(TOKEN_IDENTIFIER);
    } else {
        if (currentToken.type == TOKEN_NUMBER) {
            match(TOKEN_NUMBER);
        } else {
            if (currentToken.type == TOKEN_REAL_LITERAL) {
                match(TOKEN_REAL_LITERAL);
            } else {
                if (currentToken.type == TOKEN_CHAR_LITERAL) {
                    match(TOKEN_CHAR_LITERAL);
                } else {
                    if (currentToken.type == TOKEN_LPAREN) {
                        match(TOKEN_LPAREN);
                        parseExpression();
                        match(TOKEN_RPAREN);
                    } else {
                        syntaxError("Se esperaba identificador, número o expresión entre paréntesis");
                    }
                }
            }
        }
    }
}

/**
 * Analiza condiciones lógicas
 * Gramática: Condicion -> Expresion OperadorRelacional Expresion { OperadorLogico Condicion }
 */
void parseCondition() {
    parseExpression();
    
    // Verificar operador relacional
    if (currentToken.type == TOKEN_EQUAL || currentToken.type == TOKEN_NOT_EQUAL ||
        currentToken.type == TOKEN_LESS || currentToken.type == TOKEN_LESS_EQUAL ||
        currentToken.type == TOKEN_GREATER || currentToken.type == TOKEN_GREATER_EQUAL) {
        match(currentToken.type);
        parseExpression();
    } else {
        syntaxError("Se esperaba operador relacional en condición");
    }
    
    // Verificar operadores lógicos
    while (currentToken.type == TOKEN_AND || currentToken.type == TOKEN_OR) {
        match(currentToken.type);
        parseCondition();
    }
    
    // Manejar operador NOT
    if (currentToken.type == TOKEN_NOT) {
        match(TOKEN_NOT);
        parseCondition();
    }
}