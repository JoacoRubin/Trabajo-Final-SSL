#ifndef COMPILADOR_H
#define COMPILADOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Definiciones de constantes */
#define MAX_TOKEN_LENGTH 50
#define MAX_IDENTIFIER_LENGTH 30
#define MAX_STRING_LENGTH 100
#define MAX_PROGRAM_LENGTH 1000

/* Tipos de tokens */
typedef enum {
    // Identificadores y literales
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_CHAR_LITERAL,
    TOKEN_REAL_LITERAL,
    TOKEN_STRING_LITERAL,
    
    // Palabras reservadas - Tipos de datos
    TOKEN_ENTERO,      // entero
    TOKEN_CARACTER,    // caracter
    TOKEN_REAL,        // real
    
    // Palabras reservadas - Estructuras de control
    TOKEN_SI,          // si
    TOKEN_SINO,        // sino
    TOKEN_MIENTRAS,    // mientras
    TOKEN_REPETIR,     // repetir
    TOKEN_HASTA,       // hasta
    
    // Palabras reservadas - Entrada/Salida
    TOKEN_LEER,        // leer
    TOKEN_ESCRIBIR,    // escribir
    
    // Operadores aritméticos
    TOKEN_PLUS,        // +
    TOKEN_MINUS,       // -
    TOKEN_MULTIPLY,    // *
    TOKEN_DIVIDE,      // /
    TOKEN_MOD,         // %
    TOKEN_ASSIGN,      // :=
    
    // Operadores relacionales
    TOKEN_EQUAL,       // =
    TOKEN_NOT_EQUAL,   // <>
    TOKEN_LESS,        // <
    TOKEN_LESS_EQUAL,  // <=
    TOKEN_GREATER,     // >
    TOKEN_GREATER_EQUAL, // >=
    
    // Operadores lógicos
    TOKEN_AND,         // y
    TOKEN_OR,          // o
    TOKEN_NOT,         // no
    
    // Delimitadores
    TOKEN_LPAREN,      // (
    TOKEN_RPAREN,      // )
    TOKEN_LBRACE,      // {
    TOKEN_RBRACE,      // }
    TOKEN_SEMICOLON,   // ;
    TOKEN_COMMA,       // ,
    
    // Especiales
    TOKEN_EOF,         // Fin de archivo
    TOKEN_ERROR        // Error léxico
} TokenType;

/* Tipos de datos del compilador */
typedef enum {
    TYPE_ENTERO,
    TYPE_CARACTER,
    TYPE_REAL,
    TYPE_ERROR
} DataType;

/* Estructura para un token */
typedef struct {
    TokenType type;
    char lexeme[MAX_TOKEN_LENGTH];
    int line;
    int column;
    union {
        int intValue;
        char charValue;
        float realValue;
    } value;
} Token;

/* Estructura para la tabla de símbolos */
typedef struct Symbol {
    char name[MAX_IDENTIFIER_LENGTH];
    DataType type;
    union {
        int intValue;
        char charValue;
        float realValue;
    } value;
    int initialized;
    struct Symbol* next;
} Symbol;

/* Variables globales */
extern char* sourceCode;
extern int currentPos;
extern int currentLine;
extern int currentColumn;
extern Token currentToken;
extern Symbol* symbolTable;
extern int hasError;

/* Funciones del analizador léxico (lexer.c) */
void initLexer(char* code);
Token getNextToken(void);
int isKeyword(char* word);
int isLetter(char c);
int isDigit(char c);
void skipWhitespace(void);
void skipComment(void);

/* Funciones del analizador sintáctico (parser.c) */
void initParser(void);
void parseProgram(void);
void parseDeclaration(void);
void parseStatement(void);
void parseAssignment(void);
void parseIfStatement(void);
void parseWhileStatement(void);
void parseRepeatStatement(void);
void parseReadStatement(void);
void parseWriteStatement(void);
void parseExpression(void);
void parseTerm(void);
void parseFactor(void);
void parseCondition(void);
void match(TokenType expected);
void syntaxError(char* message);

/* Funciones del analizador semántico (semantic.c) */
void initSemantic(void);
Symbol* lookupSymbol(char* name);
Symbol* insertSymbol(char* name, DataType type);
DataType checkExpressionType(void);
void checkAssignmentCompatibility(Symbol* var, DataType exprType);
void semanticError(char* message);
DataType getTokenDataType(TokenType type);

/* Funciones auxiliares principales */
void printToken(Token token);
void printSymbolTable(void);
void cleanup(void);
char* readSourceFile(char* filename);

/* Funciones de utilidad general (utils.c) */
void dataTypeToString(DataType type, char* typeStr);
void tokenTypeToString(TokenType tokenType, char* tokenStr);
void formatSymbolValue(Symbol* symbol, char* valueStr);
void formatLocation(int line, int column, char* locationStr);

/* Funciones de validación (utils.c) */
int isValidIdentifier(char* identifier);
int isValidRealNumber(char* realStr);

/* Funciones de diagnóstico (utils.c) */
int countSymbols(void);
void displaySymbolTableStatistics(void);

/* Funciones específicas del main (main.c) */
char* getExampleSourceCode(void);
int initializeCompiler(char* sourceCode);
void displayCompilationResults(int success);
void cleanupCompiler(char* sourceCode, int isFromFile);

/* Funciones auxiliares de semantic mejoradas */
void initializeSymbolValue(Symbol* symbol, DataType type);
Symbol* createSymbol(char* name, DataType type);
int insertSymbolInTable(Symbol* symbol);
void freeSymbol(Symbol* symbol);

#endif