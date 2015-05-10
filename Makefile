# TODO: remove olly warnings
BIN_NAME = sqrt
# ^ exe should be removed

#CC = gcc
CC = /usr/local/mingw/bin/i686-w64-mingw32-gcc

override FLAGS += -g -m32 -std=c11 -Wall -Wextra 

SRCDIR = .
OBJDIR = build

#LIBS = -lpsapi

SRCS = hook.c ollydisasm/disasm.c ollydisasm/assembl.c ollydisasm/asmserv.c 
SRCS += test/printf.c 
INC += -Iinclude/ -Ilib/
 
OBJS = $(SRCS:%.c=$(OBJDIR)/%.o) 


####################
.DEFAULT: (BIN_NAME)
$(BIN_NAME): $(OBJS) 
	$(CC) $(FLAGS) $(OBJS) -o $@ $(LIBS)

$(OBJS): $(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -MMD $(FLAGS) -c $< -o $@ $(INC) 
	$(SEP)
-include $(OBJDIR)/*.d

ifeq ($(OS),Windows_NT)
RM = del /Q
endif


.PHONY: clean
clean:
	$(RM) $(BIN_NAME) && cd $(OBJDIR) && $(RM) *.o *.d && cd ollydisasm && $(RM) *.o *.d && cd ../test && $(RM) *.o *.d 

