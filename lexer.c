#include "compilador.h"

/* Variables globales del analizador lexico */
char* sourceCode = NULL;
int currentPos = 0;
int currentLine = 1;
int currentColumn = 1;
Token currentToken;

/**
 * Inicializa el analizador lexico con el codigo fuente proporcionado
 * @param code: Cadena de caracteres que contiene el codigo fuente a analizar
 */
void initLexer(char* code) {
    sourceCode = code;
    currentPos = 0;
    currentLine = 1;
    currentColumn = 1;
    currentToken = getNextToken();
}

/**
 * Verifica si una palabra es una palabra reservada del lenguaje
 * @param word: Palabra a verificar
 * @return: Tipo de token correspondiente o TOKEN_IDENTIFIER si no es palabra reservada
 */
int isKeyword(char* word) {
    // Tipos de datos
    if (strcmp(word, "entero") == 0) return TOKEN_ENTERO;
    if (strcmp(word, "caracter") == 0) return TOKEN_CARACTER;
    if (strcmp(word, "real") == 0) return TOKEN_REAL;
    
    // Estructuras de control
    if (strcmp(word, "si") == 0) return TOKEN_SI;
    if (strcmp(word, "sino") == 0) return TOKEN_SINO;
    if (strcmp(word, "mientras") == 0) return TOKEN_MIENTRAS;
    if (strcmp(word, "repetir") == 0) return TOKEN_REPETIR;
    if (strcmp(word, "hasta") == 0) return TOKEN_HASTA;
    
    // Entrada/Salida
    if (strcmp(word, "leer") == 0) return TOKEN_LEER;
    if (strcmp(word, "escribir") == 0) return TOKEN_ESCRIBIR;
    
    // Operadores logicos
    if (strcmp(word, "y") == 0) return TOKEN_AND;
    if (strcmp(word, "o") == 0) return TOKEN_OR;
    if (strcmp(word, "no") == 0) return TOKEN_NOT;
    
    return TOKEN_IDENTIFIER;
}

/**
 * Verifica si un caracter es una letra
 * @param c: Caracter a verificar
 * @return: 1 si es letra, 0 en caso contrario
 */
int isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

/**
 * Verifica si un caracter es un digito
 * @param c: Caracter a verificar
 * @return: 1 si es digito, 0 en caso contrario
 */
int isDigit(char c) {
    return c >= '0' && c <= '9';
}

/**
 * Omite espacios en blanco, tabulaciones y saltos de linea
 */
void skipWhitespace() {
    while (sourceCode[currentPos] == ' ' || sourceCode[currentPos] == '\t' || 
           sourceCode[currentPos] == '\n' || sourceCode[currentPos] == '\r') {
        if (sourceCode[currentPos] == '\n') {
            currentLine++;
            currentColumn = 1;
        } else {
            currentColumn++;
        }
        currentPos++;
    }
}

/**
 * Omite comentarios de linea que comienzan con //
 */
void skipComment() {
    if (sourceCode[currentPos] == '/' && sourceCode[currentPos + 1] == '/') {
        while (sourceCode[currentPos] != '\n' && sourceCode[currentPos] != '\0') {
            currentPos++;
        }
    }
}

/**
 * Procesa identificadores y palabras reservadas
 * @return: Token de identificador o palabra reservada
 */
Token processIdentifier() {
    Token token;
    token.line = currentLine;
    token.column = currentColumn;
    
    int start = currentPos;
    while (isLetter(sourceCode[currentPos]) || isDigit(sourceCode[currentPos])) {
        currentPos++;
        currentColumn++;
    }
    
    int length = currentPos - start;
    strncpy(token.lexeme, &sourceCode[start], length);
    token.lexeme[length] = '\0';
    
    token.type = isKeyword(token.lexeme);
    return token;
}

/**
 * Procesa numeros enteros y reales
 * @return: Token numerico (entero o real)
 */
Token processNumber() {
    Token token;
    token.line = currentLine;
    token.column = currentColumn;
    
    int start = currentPos;
    int hasDecimal = 0;
    
    // Procesar parte entera
    while (isDigit(sourceCode[currentPos])) {
        currentPos++;
        currentColumn++;
    }
    
    // Verificar si tiene parte decimal
    if (sourceCode[currentPos] == '.') {
        hasDecimal = 1;
        currentPos++;
        currentColumn++;
        
        // Procesar parte decimal
        while (isDigit(sourceCode[currentPos])) {
            currentPos++;
            currentColumn++;
        }
    }
    
    int length = currentPos - start;
    strncpy(token.lexeme, &sourceCode[start], length);
    token.lexeme[length] = '\0';
    
    if (hasDecimal) {
        token.type = TOKEN_REAL_LITERAL;
        token.value.realValue = atof(token.lexeme);
    } else {
        token.type = TOKEN_NUMBER;
        token.value.intValue = atoi(token.lexeme);
    }
    
    return token;
}

/**
 * Procesa caracteres literales entre comillas simples
 * @return: Token de caracter literal o error
 */
Token processCharLiteral() {
    Token token;
    token.line = currentLine;
    token.column = currentColumn;
    
    currentPos++; // Saltar comilla inicial
    currentColumn++;
    
    if (sourceCode[currentPos] == '\0' || sourceCode[currentPos] == '\n') {
        token.type = TOKEN_ERROR;
        strcpy(token.lexeme, "ERROR: Caracter literal no cerrado");
        return token;
    }
    
    token.value.charValue = sourceCode[currentPos];
    sprintf(token.lexeme, "'%c'", token.value.charValue);
    currentPos++;
    currentColumn++;
    
    if (sourceCode[currentPos] != '\'') {
        token.type = TOKEN_ERROR;
        strcpy(token.lexeme, "ERROR: Caracter literal no cerrado");
        return token;
    }
    
    currentPos++; // Saltar comilla final
    currentColumn++;
    token.type = TOKEN_CHAR_LITERAL;
    return token;
}

/**
 * Procesa cadenas literales entre comillas dobles
 * @return: Token de cadena literal o error
 */
Token processStringLiteral() {
    Token token;
    token.line = currentLine;
    token.column = currentColumn;
    
    int start = currentPos;
    currentPos++; // Saltar comilla inicial
    currentColumn++;
    
    while (sourceCode[currentPos] != '"' && sourceCode[currentPos] != '\0' && sourceCode[currentPos] != '\n') {
        currentPos++;
        currentColumn++;
    }
    
    if (sourceCode[currentPos] != '"') {
        token.type = TOKEN_ERROR;
        strcpy(token.lexeme, "ERROR: Cadena literal no cerrada");
        return token;
    }
    
    currentPos++; // Saltar comilla final
    currentColumn++;
    
    int length = currentPos - start;
    strncpy(token.lexeme, &sourceCode[start], length);
    token.lexeme[length] = '\0';
    
    token.type = TOKEN_STRING_LITERAL;
    return token;
}

/**
 * Procesa operadores de dos caracteres
 * @return: Token de operador de dos caracteres o TOKEN_ERROR si no es valido
 */
Token processTwoCharOperator() {
    Token token;
    token.line = currentLine;
    token.column = currentColumn;
    
    char first = sourceCode[currentPos];
    char second = sourceCode[currentPos + 1];
    
    if (first == ':' && second == '=') {
        token.type = TOKEN_ASSIGN;
        strcpy(token.lexeme, ":=");
    } else if (first == '<' && second == '>') {
        token.type = TOKEN_NOT_EQUAL;
        strcpy(token.lexeme, "<>");
    } else if (first == '<' && second == '=') {
        token.type = TOKEN_LESS_EQUAL;
        strcpy(token.lexeme, "<=");
    } else if (first == '>' && second == '=') {
        token.type = TOKEN_GREATER_EQUAL;
        strcpy(token.lexeme, ">=");
    } else {
        token.type = TOKEN_ERROR;
        strcpy(token.lexeme, "ERROR: Operador no valido");
    }
    
    if (token.type != TOKEN_ERROR) {
        currentPos += 2;
        currentColumn += 2;
    }
    
    return token;
}

/**
 * Crea un token con informacion basica
 * @param type: Tipo del token
 * @param lexeme: Lexema del token
 * @return: Token creado
 */
Token createBasicToken(TokenType type, char* lexeme) {
    Token token;
    token.type = type;
    token.line = currentLine;
    token.column = currentColumn;
    strcpy(token.lexeme, lexeme);
    return token;
}

/**
 * Procesa operadores aritméticos
 * @param currentChar: Carácter actual
 * @param token: Token a modificar
 * @return: 1 si fue procesado, 0 si no
 */
int processArithmeticOperator(char currentChar, Token* token) {
    if (currentChar == '+') {
        *token = createBasicToken(TOKEN_PLUS, "+");
        return 1;
    }
    if (currentChar == '-') {
        *token = createBasicToken(TOKEN_MINUS, "-");
        return 1;
    }
    if (currentChar == '*') {
        *token = createBasicToken(TOKEN_MULTIPLY, "*");
        return 1;
    }
    if (currentChar == '/') {
        *token = createBasicToken(TOKEN_DIVIDE, "/");
        return 1;
    }
    if (currentChar == '%') {
        *token = createBasicToken(TOKEN_MOD, "%");
        return 1;
    }
    return 0;
}

/**
 * Procesa operadores relacionales
 * @param currentChar: Carácter actual
 * @param token: Token a modificar
 * @return: 1 si fue procesado, 0 si no
 */
int processRelationalOperator(char currentChar, Token* token) {
    if (currentChar == '=') {
        *token = createBasicToken(TOKEN_EQUAL, "=");
        return 1;
    }
    if (currentChar == '<') {
        *token = createBasicToken(TOKEN_LESS, "<");
        return 1;
    }
    if (currentChar == '>') {
        *token = createBasicToken(TOKEN_GREATER, ">");
        return 1;
    }
    return 0;
}

/**
 * Procesa delimitadores
 * @param currentChar: Carácter actual
 * @param token: Token a modificar
 * @return: 1 si fue procesado, 0 si no
 */
int processDelimiter(char currentChar, Token* token) {
    if (currentChar == '(') {
        *token = createBasicToken(TOKEN_LPAREN, "(");
        return 1;
    }
    if (currentChar == ')') {
        *token = createBasicToken(TOKEN_RPAREN, ")");
        return 1;
    }
    if (currentChar == '{') {
        *token = createBasicToken(TOKEN_LBRACE, "{");
        return 1;
    }
    if (currentChar == '}') {
        *token = createBasicToken(TOKEN_RBRACE, "}");
        return 1;
    }
    if (currentChar == ';') {
        *token = createBasicToken(TOKEN_SEMICOLON, ";");
        return 1;
    }
    if (currentChar == ',') {
        *token = createBasicToken(TOKEN_COMMA, ",");
        return 1;
    }
    return 0;
}

/**
 * Procesa operadores de un carácter
 * @param currentChar: Carácter actual
 * @return: Token de operador de un carácter
 */
Token processSingleCharOperator(char currentChar) {
    Token token;
    token.line = currentLine;
    token.column = currentColumn;
    
    // Intentar procesar operadores aritméticos
    if (processArithmeticOperator(currentChar, &token)) {
        currentPos++;
        currentColumn++;
        return token;
    }
    
    // Intentar procesar operadores relacionales
    if (processRelationalOperator(currentChar, &token)) {
        currentPos++;
        currentColumn++;
        return token;
    }
    
    // Intentar procesar delimitadores
    if (processDelimiter(currentChar, &token)) {
        currentPos++;
        currentColumn++;
        return token;
    }
    
    // Carácter desconocido
    token.type = TOKEN_ERROR;
    sprintf(token.lexeme, "ERROR: Carácter desconocido '%c'", currentChar);
    currentPos++;
    currentColumn++;
    return token;
}

/**
 * Verifica si el carácter actual inicia un operador de dos caracteres
 * @param currentChar: Carácter actual
 * @return: 1 si puede ser operador de dos caracteres, 0 en caso contrario
 */
int isTwoCharOperatorStart(char currentChar) {
    return (currentChar == ':' && sourceCode[currentPos + 1] == '=') ||
           (currentChar == '<' && (sourceCode[currentPos + 1] == '>' || sourceCode[currentPos + 1] == '=')) ||
           (currentChar == '>' && sourceCode[currentPos + 1] == '=');
}

/**
 * Obtiene el siguiente token del código fuente
 * @return: Token obtenido del análisis léxico
 */
Token getNextToken() {
    Token token;
    
    // Omitir espacios en blanco y comentarios
    skipWhitespace();
    skipComment();
    skipWhitespace();
    
    // Verificar fin de archivo
    if (sourceCode[currentPos] == '\0') {
        token.type = TOKEN_EOF;
        token.line = currentLine;
        token.column = currentColumn;
        strcpy(token.lexeme, "EOF");
        return token;
    }
    
    char currentChar = sourceCode[currentPos];
    
    // Identificadores y palabras reservadas
    if (isLetter(currentChar)) {
        return processIdentifier();
    }
    
    // Números (enteros y reales)
    if (isDigit(currentChar)) {
        return processNumber();
    }
    
    // Caracteres literales
    if (currentChar == '\'') {
        return processCharLiteral();
    }
    
    // Cadenas literales
    if (currentChar == '"') {
        return processStringLiteral();
    }
    
    // Operadores de dos caracteres
    if (isTwoCharOperatorStart(currentChar)) {
        return processTwoCharOperator();
    }
    
    // Operadores de un carácter
    return processSingleCharOperator(currentChar);
}