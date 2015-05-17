BIN_NAME = ia32hook.a

override FLAGS += -g -m32 -std=c99 -Wall -Wextra 

SRCDIR = .
OBJDIR = build

SRCS := hook.c tests/isOdd.c
#TEST += tests/isOdd.c
#TODO: `make win32 test` should work
#TODO: -lpthread on Linux
SRCS += $(TEST)
SRCS += #mhold-win32.c win32.c 
SRCS += mhold-unix.c
INC += -Iinclude/ -Ilib/
 
OBJS = $(SRCS:%.c=$(OBJDIR)/%.o) 
DISASM_SRCS = disasm.c assembl.c asmserv.c
DISASM_SRCDIR = ollydisasm
DISASM_OBJS = $(DISASM_SRCS:%.c=$(OBJDIR)/%.o) 
####################
.DEFAULT: (BIN_NAME)
$(BIN_NAME): $(OBJS) $(DISASM_OBJS)
	$(AR) rcs $(BIN_NAME) $(OBJS) $(DISASM_OBJS)

win32: CC = /usr/local/mingw/bin/i686-w64-mingw32-gcc
win32: AR = /usr/local/mingw/bin/i686-w64-mingw32-ar
win32: $(BIN_NAME)

test: SRCS += $(TEST)


test: $(OBJS) $(DISASM_OBJS)
	$(CC) $(FLAGS) $(OBJS) $(DISASM_OBJS) -o $@ $(LIBS)

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
	$(RM) $(BIN_NAME) test && cd $(OBJDIR) && $(RM) *.o *.d && cd $(DISASM_SRCDIR) && $(RM) *.o *.d && cd ../tests && $(RM) *.o *.d

