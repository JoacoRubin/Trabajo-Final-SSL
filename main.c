#include "compilador.h"

/**
 * Imprime el contenido completo de la tabla de simbolos con estadisticas
 */
void printSymbolTable() {
    printf("\n=== TABLA DE SIMBOLOS ===\n");
    printf("%-15s %-10s %-12s %-10s\n", "Nombre", "Tipo", "Inicializada", "Valor");
    printf("------------------------------------------------\n");
    
    Symbol* current = symbolTable;
    int total = 0, initialized = 0;
    
    while (current != NULL) {
        char typeStr[15], valueStr[20];
        dataTypeToString(current->type, typeStr);
        formatSymbolValue(current, valueStr);
        
        printf("%-15s %-10s %-12s %-10s\n", 
               current->name, typeStr, 
               current->initialized ? "Si" : "No", valueStr);
        
        total++;
        if (current->initialized) initialized++;
        current = current->next;
    }
    
    printf("================================================\n");
    printf("Total: %d | Inicializadas: %d | No inicializadas: %d\n", 
           total, initialized, total - initialized);
}

/**
 * Libera la memoria y reinicia el compilador
 */
void cleanup() {
    int count = 0;
    Symbol* current = symbolTable;
    
    // Liberar todos los simbolos de la lista enlazada
    while (current != NULL) {
        Symbol* next = current->next;
        if (current != NULL) free(current);
        current = next;
        count++;
    }
    
    // Reiniciar todas las variables globales
    symbolTable = NULL;
    currentPos = 0;
    currentLine = 1;
    currentColumn = 1;
    hasError = 0;
    
    printf("Memoria liberada correctamente (%d simbolos).\n", count);
}

/**
 * Lee un archivo de codigo fuente
 * @param filename: Nombre del archivo a leer
 * @return: Contenido del archivo o NULL si hay error
 */
char* readSourceFile(char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("ERROR: No se pudo abrir el archivo '%s'\n", filename);
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = (char*)malloc(fileSize + 1);
    if (!content) {
        printf("ERROR: No se pudo asignar memoria para el archivo\n");
        fclose(file);
        return NULL;
    }
    
    size_t bytesRead = fread(content, 1, fileSize, file);
    content[bytesRead] = '\0';
    fclose(file);
    
    // Verificar si la lectura fue exitosa
    if (bytesRead == 0 && fileSize > 0) {
        printf("ADVERTENCIA: El archivo parece estar vacio o hubo error al leer\n");
    }
    
    return content;
}

/**
 * Obtiene el codigo de ejemplo para demostracion
 * @return: Codigo fuente de ejemplo
 */
char* getExampleSourceCode() {
    return "// Programa de ejemplo con nuevos tipos y sentencias\n"
           "entero contador, limite;\n"
           "real promedio, suma;\n"
           "caracter letra, vocal;\n"
           "\n"
           "contador := 1;\n"
           "limite := 10;\n"
           "suma := 0.0;\n"
           "letra := 'A';\n"
           "\n"
           "// Ejemplo de sentencia SI\n"
           "si (contador <= limite) {\n"
           "    escribir(contador);\n"
           "    suma := suma + contador;\n"
           "} sino {\n"
           "    //escribir(\"Contador excede el límite\");\n"
           "}\n"
           "\n"
           "// Ejemplo de bucle MIENTRAS\n"
           "mientras (contador < limite) {\n"
           "    contador := contador + 1;\n"
           "    suma := suma + contador;\n"
           "}\n"
           "\n"
           "// Ejemplo de bucle REPETIR HASTA\n"
           "repetir {\n"
           "    escribir(letra);\n"
           "    contador := contador - 1;\n"
           "} hasta (contador = 0);\n"
           "\n"
           "promedio := suma / limite;\n"
           "escribir(promedio);\n";
}



/**
 * Inicializa compilador y muestra resultados
 * @param sourceCode: Codigo fuente a procesar
 * @return: 1 si la compilacion fue exitosa, 0 en caso contrario
 */
int compileAndShowResults(char* sourceCode) {
    if (!sourceCode) {
        printf("ERROR: Codigo fuente es NULL\n");
        return 0;
    }
    
    initSemantic();
    initParser();
    initLexer(sourceCode);
    parseProgram();
    
    int success = !hasError;
    printf(success ? "\nCOMPILACION EXITOSA\n" : "\nCOMPILACION FALLIDA\n");
    
    if (success) {
        printSymbolTable();
        printf("El programa es sintactica y semanticamente correcto.\n");
    } else {
        printf("Se encontraron errores durante el analisis.\n");
    }
    
    return success;
}

/**
 * Libera recursos del compilador
 * @param sourceCode: Codigo fuente a liberar
 * @param isFromFile: Si el codigo fue leido de archivo
 */
void cleanupCompiler(char* sourceCode, int isFromFile) {
    // Liberar codigo fuente solo si fue asignado dinamicamente
    if (isFromFile && sourceCode != NULL) {
        free(sourceCode);
        sourceCode = NULL; // Evitar double-free
        printf("Codigo fuente liberado de memoria.\n");
    }
    
    // Liberar tabla de simbolos y reiniciar variables
    cleanup();
}

/**
 * Funcion principal del compilador
 * @param argc: Numero de argumentos de linea de comandos
 * @param argv: Argumentos de linea de comandos
 * @return: Codigo de salida (0 = exito, 1 = error)
 */
int main(int argc, char* argv[]) {
    if (argc > 2) {
        printf("ERROR: Demasiados argumentos\nUso: %s [archivo_fuente.txt]\n", argv[0]);
        return 1;
    }
    
    printf("=== COMPILADOR SSL - TRABAJO FINAL ===\n");
    printf("Tipos soportados: entero, caracter, real\n");
    printf("Sentencias: si-sino, mientras, repetir-hasta\n");
    printf("=====================================\n\n");
    
    // Obtener codigo fuente
    int isFromFile = (argc == 2);
    char* sourceCode = isFromFile ? readSourceFile(argv[1]) : getExampleSourceCode();
    
    if (!sourceCode) {
        printf("ERROR: No se pudo obtener el codigo fuente\n");
        return 1;
    }
    
    if (!isFromFile) printf("Usando codigo de ejemplo para demostracion:\n\n");
    printf("CODIGO FUENTE:\n%s\n=====================================\n\n", sourceCode);
    
    // Compilar y mostrar resultados
    int success = compileAndShowResults(sourceCode);
    cleanupCompiler(sourceCode, isFromFile);
    
    return success ? 0 : 1;
}