# Makefile for microcontroller firmware 

# programs
DELETE	=	@cmd.exe /c del
CC	=	NC30.EXE
AS	=	AS30.EXE
LN	=	LN30.EXE
LMC	=	LMC30.EXE

##### FILES AND PATHS ##########################################################

SRCDIR = ..\src
TMPDIR = tmp
BINDIR = ..\bin

COBJECTS = $(patsubst $(SRCDIR)/%.c,$(TMPDIR)/%.r30,$(wildcard $(SRCDIR)/*.c))
ASMOBJECTS = $(patsubst $(SRCDIR)/%.a30,$(TMPDIR)/%.r30,$(wildcard $(SRCDIR)/*.a30))
OBJECTS = $(COBJECTS) $(ASMOBJECTS)

TARGET	=	$(BINDIR)\door.x30
IMAGE = 	$(TARGET:%.x30=%.mot)

##### FLAGS ####################################################################

CFLAGS	=	-g
ASMFLAGS	=	

##### RULES ####################################################################

.PHONY: all
all: $(IMAGE)

#machine file conversion
$(IMAGE): $(TARGET)
	$(LMC) -O $(basename $@) $< 

#linking
$(TARGET): $(OBJECTS) 
	$(LN) -O $@ -L nc30lib.lib -G $(ASMOBJECTS) $(COBJECTS)

#compiling
$(TMPDIR)/%.r30: $(SRCDIR)\%.c
	$(CC) $(CFLAGS) -c -dir $(TMPDIR) $<

#assembling		
$(TMPDIR)/%.r30: $(SRCDIR)\%.a30 
	$(AS) $(ASMFLAGS) -O$(TMPDIR) $<


.PHONY:	clean
clean:
	$(DELETE) $(subst /,\,$(OBJECTS))
	$(DELETE) $(TARGET)
	$(DELETE) $(IMAGE)
