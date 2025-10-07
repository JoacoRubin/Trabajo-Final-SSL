# Makefile para el compilador SSL

# Compilador y flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# Archivos fuente y objeto
SOURCES = main.c lexer.c parser.c semantic.c utils.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = compilador

# Regla principal
all: $(TARGET)

# Compilar el ejecutable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# Compilar archivos objeto
%.o: %.c compilador.h
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	del /Q *.o $(TARGET).exe 2>nul || true

# Ejecutar con código de ejemplo
test: $(TARGET)
	./$(TARGET)

# Ejecutar casos de prueba específicos
test-tipos: $(TARGET)
	./$(TARGET) ejemplo1_tipos.txt

test-si: $(TARGET)
	./$(TARGET) ejemplo2_si_sino.txt

test-mientras: $(TARGET)
	./$(TARGET) ejemplo3_mientras.txt

test-repetir: $(TARGET)
	./$(TARGET) ejemplo4_repetir_hasta.txt

test-completo: $(TARGET)
	./$(TARGET) ejemplo5_programa_completo.txt

test-errores: $(TARGET)
	./$(TARGET) casos_error.txt

test-estructurado: $(TARGET)
	./$(TARGET) ejemplo_estructurado.txt

test-memoria: $(TARGET)
	./$(TARGET) ejemplo_memoria_estructurada.txt

# Ayuda
help:
	@echo "Makefile para el compilador SSL"
	@echo "Comandos disponibles:"
	@echo "  make all         - Compila el compilador"
	@echo "  make test        - Ejecuta con código de ejemplo"
	@echo "  make test-tipos  - Prueba tipos de datos"
	@echo "  make test-si     - Prueba sentencias SI-SINO"
	@echo "  make test-mientras - Prueba bucles MIENTRAS"
	@echo "  make test-repetir  - Prueba bucles REPETIR-HASTA"
	@echo "  make test-completo - Ejecuta programa completo"
	@echo "  make test-errores  - Prueba manejo de errores"
	@echo "  make test-estructurado - Prueba ejemplo de programación estructurada"
	@echo "  make test-memoria  - Prueba gestión de memoria y programación estructurada"
	@echo "  make clean       - Limpia archivos generados"

.PHONY: all clean test test-tipos test-si test-mientras test-repetir test-completo test-errores help