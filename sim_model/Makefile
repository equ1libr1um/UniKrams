### Makefile for door simulator model ###

SRCDIR = ../src
TMPDIR = tmp
BINDIR = ../bin

ifeq ($(shell uname),Linux)
TARGET += $(BINDIR)/door_model.so
PLATFORM = LINUX
else
TARGET = $(BINDIR)/door_model.dll
PLATFORM = WIN
endif

TOOLCHAIN_PREFIX=
TOOLCHAIN_SUFFIX=

CC = $(TOOLCHAIN_PREFIX)gcc$(TOOLCHAIN_SUFFIX)
LD = $(TOOLCHAIN_PREFIX)gcc$(TOOLCHAIN_SUFFIX)

INCDIRS += 

################################################################################
# list your source files here ##################################################
################################################################################

SRCFILES = fsm.c

################################################################################

OBJECTS = $(patsubst %.c,$(TMPDIR)/%.c.o,$(SRCFILES))

LDFLAGS = -shared
COMPILER_FLAGS = -O2 -Wall -g $(addprefix -I, $(INCDIRS)) -DSIM_MODEL -D$(PLATFORM)
ifeq ($(shell uname),Linux)
COMPILER_FLAGS += -fPIC -fno-function-sections
endif
CFLAGS = -std=gnu99 $(COMPILER_FLAGS)

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
	-@[ -d $(BINDIR) ] || mkdir -p $(BINDIR)
	$(LD) $(LDFLAGS) $^ -o $@

$(TMPDIR)/%.c.o:$(SRCDIR)/%.c
	-@[ -d $(dir $@) ] || mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -MT $@ -MF $@.d -o $@ -c $<

.PHONY: sim
sim: $(TARGET_SIM)

.PHONY: clean
clean:
	-rm -f $(TARGET)
	-rm -f $(TMPDIR)/*.o
	-rm -f $(TMPDIR)/*.o.d

-include $(wildcard $(TMPDIR)/*.o.d)
-include $(wildcard $(TMPDIR)/*.o.d)
