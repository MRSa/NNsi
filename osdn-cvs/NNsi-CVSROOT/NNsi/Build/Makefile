##############################################################################
#
#   $Id: Makefile,v 1.68 2005/07/16 15:36:28 mrsa Exp $
#
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
NNSI_CFLAGS  = -I/PalmDev/include -DUSE_HANDERA                         \
               -DUSE_GLUE -DUSE_CLIE -DUSE_ARMLET -DUSE_COLOR           \
               -DUSE_HIGHDENSITY -DUSE_TSPATCH -DSTRIP_MEANINGLESS_LINE \
               -DUSE_XML_OUTPUT -DUSE_NEWTHREAD_FEATURE -DUSE_LOGCHARGE \
               -DUSE_BTTOGGLE_SDK -DUSE_BT_CONTROL                      \
               -DUSE_SSL                                                \
               -DUSE_MACRO -DUSE_MACROEDIT -DMACRO_TEST                 \
               -DUSE_PIN_DIA -DRELEASE_NNSISETTING -DRELEASE_NNSIGLOBAL \
               -DUSE_PIN_GARMIN                                         \
               -DUSE_CONTENT_LENGTH_INFO                                \
               -DPREVENT_DUPLICATE_POST                                 \
               -DUSE_SSL_V2                                             \
               -DUSE_ZLIB                                               \
#              -DUSE_DEBUG_ROM                                          \
#              -DUSE_SSL_V2                                             \
#              -DSET_SOCKET_OPTION                                      \
#              -DUSE_ADDR_NETLIB                                        \
#               -DUSE_REPORTER
#               -DUSE_PIN_DIA -DRELEASE_NNSISETTING
#               -DUSE_PIN_DIA -DRELEASE_NNSISETTING -DRELEASE_NNSIGLOBAL \

#               -DUSE_PIN_DIA -DRELEASE_NNSISETTING

#               -DUSE_PIN_DIA                       -DRELEASE_NNSIGLOBAL \
#               -DRELEASE_NNSISETTING               -DRELEASE_NNSIGLOBAL \



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
INCDIR       = ../Inc
ARMSRCDIR    = ../ArmSrc
######################################################################

# ------------------------------------------------------------
#   Compile objects 
# ------------------------------------------------------------
hdrList = \
	  $(RSCDIR)/nnsh.h       \
	  $(INCDIR)/armutls.h \
	  $(INCDIR)/local.h	 \
	  $(INCDIR)/global.h	 \
	  $(INCDIR)/proto.h	 \
	  $(INCDIR)/struct.h	 \
	  $(INCDIR)/symbols.h	 \
	  $(INCDIR)/messages.h	 \
	  $(INCDIR)/han2zen.h	 \
	  $(INCDIR)/xmldefs.h	 \
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
	  nnshmain.c \
	  skeleton.c \
      nnshcmn.c  \
	  nnshset.c	 \
	  bbsmng.c	 \
	  subjmgr.c	 \
	  msgwrite.c \
	  msgmgr.c	 \
	  msglist.c	 \
      inoutXML.c \
      nnshdmmy.c \
      macro.c    \
      macroedit.c \
      sslv2.c    \
      sslv2md5.c \
      sslv2rc4.c \
      sslv2rsa.c \
      getlog.c   \
      getloglist.c \
      modifylog.c \


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
		-I$(INCDIR)	\
		-I$(RSCDIR)	\
		-save-temps
#        -palmos5r3

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
	$(MAKE) "DEBUGFLAGS=-DUSE_REPORTER_VIEW -DUSE_REPORTER_MAIN"

offline_debug :
	$(MAKE) clean
	$(MAKE) "DEBUGFLAGS=-DOFFLINE_DEBUG -DDEBUG -DUSE_REPORTER -DUSE_REPORTER_MAIN"
