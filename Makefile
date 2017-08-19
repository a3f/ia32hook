LIB = ia32hook.a

CFLAGS += -g -m32 -std=c99 -D_GNU_SOURCE -Wall -Wextra

SRCDIR = .

SRCS := hook.c
TEST = tests/isOdd.c
#TODO: `make win32 test` should work
#TODO: -lpthread on Linux
INC += -Iinclude/ -Ilib/

CC ?= gcc

####################
ifeq ($(OS),Windows_NT)
RM = del /Q
RUNTEST = run-test
SRCS += mhold-win32.c win32.c
else
RUNTEST = ./run-test
CFLAGS += -pthread
LDFLAGS += -pthread
SRCS += mhold-unix.c
endif

OBJDIR = build
OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)
TESTOBJS = $(TEST:%.c=$(OBJDIR)/%.o)
DISASM_SRCS = disasm.c assembl.c asmserv.c
DISASM_SRCDIR = ollydisasm
DISASM_OBJS = $(DISASM_SRCS:%.c=$(OBJDIR)/%.o)


.DEFAULT: all
all: $(LIB)

$(LIB): $(OBJS) $(DISASM_OBJS)
	$(AR) rcs $(LIB) $(OBJS) $(DISASM_OBJS)

win32: CC = /usr/local/mingw/bin/i686-w64-mingw32-gcc
win32: AR = /usr/local/mingw/bin/i686-w64-mingw32-ar
win32: $(LIB)

.PHONY: test
test: run-test
	$(RUNTEST)

run-test: $(TESTOBJS) $(LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) $(TESTOBJS) -o $@ $(LIB) $(LIBS)

$(OBJS): $(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -MMD $(CFLAGS) -c $< -o $@ $(INC)
-include $(OBJDIR)/*.d

$(TESTOBJS): $(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -MMD $(CFLAGS) -c $< -o $@ $(INC)
-include $(OBJDIR)/*.d

$(DISASM_OBJS): $(OBJDIR)/%.o: $(DISASM_SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@




.PHONY: clean
clean:
	$(RM) $(LIB) run-test && cd $(OBJDIR) && $(RM) *.o *.d && cd $(DISASM_SRCDIR) && $(RM) *.o *.d && cd ../tests && $(RM) *.o *.d

