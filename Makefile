BIN_NAME = ia32hook.dll

CC = gcc
#CC = /usr/local/mingw/bin/i686-w64-mingw32-gcc

override FLAGS += -g -m32 -std=c99 -Wall -Wextra 

SRCDIR = .
OBJDIR = build

SRCS = hook.c win32.c
SRCS = hook.c ollydisasm/disasm.c ollydisasm/assembl.c ollydisasm/asmserv.c 
# SRCS += test/printf.c

INC += -Iinclude/ -Ilib/
 
OBJS = $(SRCS:%.c=$(OBJDIR)/%.o) 

DISASM_SRCS = disasm.c assembl.c asmserv.c
DISASM_SRCDIR = ollydisasm
DISASM_OBJS = $(DISASM_SRCS:%.c=$(OBJDIR)/%.o) 
####################
.DEFAULT: (BIN_NAME)
$(BIN_NAME): $(OBJS) 
	$(CC) -shared $(FLAGS) $(OBJS) -o $@ $(LIBS)

$(OBJS): $(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -MMD $(FLAGS) -c $< -o $@ $(INC) 
-include $(OBJDIR)/*.d

$(DISASM_OBJS): $(OBJDIR)/%.o: $(DISASM_SRCDIR)/%.c
	$(CC) -m32 -c $< -o $@


ifeq ($(OS),Windows_NT)
RM = del /Q
endif


.PHONY: clean
clean:
	$(RM) $(BIN_NAME) && cd $(OBJDIR) && $(RM) *.o *.d && cd $(DISASM_SRCDIR) && $(RM) *.o *.d && cd ../test && $(RM) *.o *.d

