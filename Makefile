# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lfl -lm	# lm to link the math library

# Project name
TARGET = BreezeLangCompiler

# Source files
BISON_SRC = parser.y
FLEX_SRC = lexer.l
C_SOURCES = symtab.c
GENERATED_SOURCES = lex.yy.c parser.tab.c
ALL_SOURCES = $(C_SOURCES) $(GENERATED_SOURCES)

# Object files
OBJECTS = $(ALL_SOURCES:.c=.o)

# Header files
HEADERS = symtab.h parser.tab.h

# Default target
all: $(TARGET)

# Generate parser source files
parser.tab.c parser.tab.h: $(BISON_SRC)
	bison -d $(BISON_SRC)

# Generate lexer source file
lex.yy.c: $(FLEX_SRC) parser.tab.h
	flex $(FLEX_SRC)

# Compile the final executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

# Generic rule for object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

# Clean generated files
clean:
	rm -f $(TARGET) $(OBJECTS) $(GENERATED_SOURCES) parser.tab.h

# Prevent make from deleting intermediate files
.PRECIOUS: parser.tab.c parser.tab.h lex.yy.c
