# project name (generate executable with this name)
TARGET   = mapache

CC       = gcc
SH	 = sh
# compiling flags here
CFLAGS   = -std=c99 -g -Wall -I.

LINKER   = gcc -o
# linking flags here
#LFLAGS   = -Wall -I. -lm
LFLAGS   = 

# change these to set the proper directories where each files shoould be
SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f


$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

compile:
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS)
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONEY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"

run: #remove clean compile
	@$(SH) run.sh

kill:
	killall mapache
