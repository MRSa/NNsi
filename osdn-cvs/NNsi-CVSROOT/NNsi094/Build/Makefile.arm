##############################################################################
#   Makefile for Palm Application
#  - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#
##############################################################################

# ===========================================================================
#     Target Program informations
# ===========================================================================
CREATOR      = NNsi 
PROGNAME     = NNsi
MULTISEGDEF  = NNsi.def
PROG_PRCFILE = NNsi.prc
RCPFILE      = nnsh.rcp.icon
NNSI_CFLAGS  = -I/PalmDev/include/HandEra -DUSE_HANDERA -DUSE_GLUE -DUSE_CLIE -DUSE_ARMLET

######################################################################
# Tool names to use
CC           = m68k-palmos-gcc
ARMCC        = arm-palmos-gcc
ARMLD        = arm-palmos-ld
ARMOBJCOPY   = arm-palmos-objcopy
MULTIGEN     = m68k-palmos-multigen
BUILDPRC     = build-prc
PILRC        = pilrc
RM           = rm
######################################################################
# Directory paths
OBJDIR       = ../Obj
SRCDIR       = ../Src
RSCDIR       = ../Rsc
ARMSRCDIR    = ../ArmSrc
######################################################################

# ------------------------------------------------------------
#   Compile objects 
# ------------------------------------------------------------
hdrList = \
	  $(RSCDIR)/nnsh.h       \
	  $(ARMSRCDIR)/armutls.h \
	  $(SRCDIR)/local.h	 \
	  $(SRCDIR)/global.h	 \
	  $(SRCDIR)/proto.h	 \
	  $(SRCDIR)/struct.h	 \
	  $(SRCDIR)/symbols.h	 \
	  $(SRCDIR)/messages.h	 \
	  $(SRCDIR)/machine.c    \
	  $(SRCDIR)/main.c       \

srcList = \
	  msgview.c	\
	  netmng.c	\
	  netent.c	\
	  offline.c	\
	  httpmgr.c	\
	  dbmgr.c	\
	  filemgr.c	\
	  windows.c	\
	  nnshmain.c	\
	  skeleton.c	\
	  nnshset.c	\
	  bbsmng.c	\
	  subjmgr.c	\
	  msgwrite.c	\
	  msgmgr.c	\

objList  = $(srcList:%.c=$(OBJDIR)/%.o)

rscList :=  $(wildcard $(RSCDIR)/*)


ARMSRC = \
	 armc0001.c \
	 armc0002.c \

ARMOBJ = $(ARMSRC:%.c=$(OBJDIR)/%.o)
ARMBIN = $(ARMOBJ:.o=.o.bin)

# ===========================================================================
# Compile options
DEBUGFLAGS  =
CFLAGS	    =   -Wall           \
                $(NNSI_CFLAGS)	\
                $(DEBUGFLAGS)   \
		-I$(SRCDIR)	\
		-I$(ARMSRCDIR)	\
		-I$(RSCDIR)	\
		-save-temps     \
                -palmos5
M68KCFLAGS = -O2 
ARMCFLAGS  = -O2 -Wmissing-prototypes -Wmissing-declarations -nostartfiles -nostdlib -fno-builtin -fPIC -ffixed-r8 -ffixed-r9

CODESEGCFLAGS	= $(CFLAGS) -Winline -nostartfiles


VPATH=$(SRCDIR):$(ARMSRCDIR)

# These are fake targets used to perform certain actions. Use the .PHONY
#  command to make sure they don't get confused with actual filenames
.PHONY : all install clean help makeObjDir $(OBJDIR)/bin.res

#############################################################
# Master Builds
#############################################################
all: $(OBJDIR)/$(PROGNAME).m68k $(OBJDIR)/bin.res $(ARMBIN)
	$(MAKE) $(OBJDIR)/$(PROGNAME).m68k
	$(MAKE) $(OBJDIR)/bin.res
	$(BUILDPRC) $(MULTISEGDEF) --output $(OBJDIR)/$(PROG_PRCFILE) \
		--creator $(CREATOR)  \
		--type appl           \
		--name $(PROGNAME)    \
                $(OBJDIR)/$(PROGNAME).m68k \
		$(OBJDIR)/*.bin


install:   
	$(MAKE) makeObjDir
	$(MAKE) clean
	$(MAKE)

clean:
	$(RM) -f $(OBJDIR)/*.o $(OBJDIR)/*.s $(OBJDIR)/*.i $(OBJDIR)/bin.res
	$(RM) -f $(OBJDIR)/*.bin $(OBJDIR)/*.arm $(OBJDIR)/*~ 
	$(RM) -f $(OBJDIR)/*.m68k ./*.o ./*.s ./*.i ./*.ld ./*~

makeObjDir :
	if [ ! -e $(OBJDIR) ]; then	 mkdir $(OBJDIR); fi


#############################################################
# Implicit build rules
#############################################################
$(objList) : $(srcList) $(hdrList)
	$(CC) $(M68KCFLAGS) $(CFLAGS) -o $@ -c $(SRCDIR)/$(basename $(notdir $@)).c

$(ARMOBJ) : $(ARMSRC) $(hdrList)
	$(ARMCC) $(CFLAGS) $(ARMCFLAGS) -o $@  $(ARMSRCDIR)/$(basename $(notdir $@)).c

#$(ARMLK) : $(ARMOBJ)
#	$(ARMLD) $(basename $@) $@

$(ARMBIN) : $(ARMOBJ)
	$(ARMOBJCOPY) -v -j .text -O binary $(basename $@) $@

# ------------------------------------------------------------
# Link and combine with resources. 
# ------------------------------------------------------------

NNsi-sections.o: NNsi-sections.s
	$(CC) $(CFLAGS) -c NNsi-sections.s

NNsi-sections.s NNsi-sections.ld: $(MULTISEGDEF)
	$(MULTIGEN) $(MULTISEGDEF)

$(OBJDIR)/$(PROGNAME).m68k : $(objList) NNsi-sections.ld NNsi-sections.o
	$(CC) $(CFLAGS) -o $(OBJDIR)/$(PROGNAME).m68k $(objList) NNsi-sections.o NNsi-sections.ld -lPalmOSGlue

$(OBJDIR)/bin.res: 
	$(PILRC) -I $(RSCDIR) -Fj -q $(RSCDIR)/$(RCPFILE) $(OBJDIR)
	touch $(OBJDIR)/bin.res

debug :
	$(MAKE) clean
	$(MAKE) "DEBUGFLAGS=-DOFFLINE_DEBUG -DDEBUG"
