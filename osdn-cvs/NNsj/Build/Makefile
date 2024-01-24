#############################################################################
#   Makefile for Palm Application
#  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#                            NNsiをベースとしたPalmwareプログラムスケルトン
#                                                       (for prc-tools 2.2)
#############################################################################

# ===========================================================================
#　 作成するプログラムの情報(ターゲットプログラムによって変更する部分)
# ===========================================================================
#---------------------------------------------------------------------------
# コンパイルスイッチなど
#---------------------------------------------------------------------------
CREATOR        = NNsj
PROGNAME       = NNsiMacroEdit
PROG_PRCFILE   = NNsj.prc
RCPFILE        = nnsh.rcp
MULTISEGDEF    = Sec.def
DEBUGFLAGS     =
HANDERA_SDK    = -I/PalmDev/include/HandEra -DUSE_HANDERA
#TARGET_CFLAGS  = $(HANDERA_SDK) $(USE_OPTION) $(DEBUGFLAGS) -save-temps
TARGET_CFLAGS  = $(HANDERA_SDK) $(USE_OPTION) $(DEBUGFLAGS)
USE_OPTION     = -DUSE_GLUE -DUSE_HIGHDENSITY
#
M68KLINKOPT    = -lPalmOSGlue
#---------------------------------------------------------------------------
#  ヘッダファイル
#---------------------------------------------------------------------------
hdrList = \
	  $(RSCDIR)/nnsh.h       \
	  $(ARMSRCDIR)/armutls.h \
	  $(BUILDDIR)/version.h  \
	  $(INCDIR)/local.h	 \
	  $(INCDIR)/global.h	 \
	  $(INCDIR)/proto.h	 \
	  $(INCDIR)/struct.h	 \
	  $(INCDIR)/sections.h	 \

#	  $(INCDIR)/symbols.h	 \
###	  $(INCDIR)/messages.h	 \
#---------------------------------------------------------------------------
#  ソースファイル(m68k)
#---------------------------------------------------------------------------
srcList = \
          nnshmain.c     \
          macroview.c    \
          linedata.c     \
          script.c       \
          opcode.c       \
          $(CmnSrcList)  \


CmnSrcList = \
	  skeleton.c \
	  main.c     \
	  machine.c  \
	  windows.c  \
      dbmgr.c    \
      filemgr.c  \
      nnshcmn.c  \

#---------------------------------------------------------------------------
#  ソースファイル(ARMlet)
#---------------------------------------------------------------------------
ARMSRC = \
#         armc0001.c \

#---------------------------------------------------------------------------
#  コンパイルディレクトリ指定
#---------------------------------------------------------------------------
INCDIR       = ../Inc
OBJDIR       = ../Obj
SRCDIR       = ../Src
RSCDIR       = ../Rsc
BUILDDIR     = .
ARMSRCDIR    = ../ArmSrc

# ===========================================================================
#    その他のmake指示 : prc-tools 2.2なら変更が不要な(はずの)部分
#
# ===========================================================================
#
# Tool names to use
#
CC           = m68k-palmos-gcc
ARMCC        = arm-palmos-gcc
ARMLD        = arm-palmos-ld
ARMOBJCOPY   = arm-palmos-objcopy
MULTIGEN     = m68k-palmos-multigen
BUILDPRC     = build-prc
PILRC        = pilrc
RM           = rm
TOUCH        = touch

ARMOBJ       = $(ARMSRC:%.c=$(OBJDIR)/%.o)
ARMBIN       = $(ARMOBJ:.o=.o.bin)
objList      = $(srcList:%.c=$(OBJDIR)/%.o)
rscList     := $(wildcard $(RSCDIR)/*)
SECTIONNAME  = $(basename $(MULTISEGDEF))-sections

# ===========================================================================
# Compile options
CFLAGS	    =   -Wall            \
                $(TARGET_CFLAGS) \
		-I.     	 \
		-I$(INCDIR)	 \
		-I$(SRCDIR)	 \
		-I$(ARMSRCDIR)   \
		-I$(RSCDIR)      \
#                -palmos5

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
all: $(OBJDIR)/$(CREATOR).m68k $(OBJDIR)/bin.res $(ARMBIN)
	$(MAKE) $(OBJDIR)/$(CREATOR).m68k
	$(MAKE) $(OBJDIR)/bin.res
	$(BUILDPRC) $(MULTISEGDEF) --output $(OBJDIR)/$(PROG_PRCFILE) \
		--creator $(CREATOR)  \
		--type appl           \
		--name $(PROGNAME)    \
                $(OBJDIR)/$(CREATOR).m68k \
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
#   Link and combine with resources. 
# ------------------------------------------------------------

$(SECTIONNAME).o: $(SECTIONNAME).s
	$(CC) $(CFLAGS) -c $(SECTIONNAME).s

$(SECTIONNAME).s $(SECTIONNAME).ld: $(MULTISEGDEF)
	$(MULTIGEN) $(MULTISEGDEF)

$(OBJDIR)/$(CREATOR).m68k : $(objList) $(SECTIONNAME).ld $(SECTIONNAME).o
	$(CC) $(CFLAGS) -o $(OBJDIR)/$(CREATOR).m68k $(objList) $(SECTIONNAME).o $(SECTIONNAME).ld $(M68KLINKOPT)

$(OBJDIR)/bin.res: 
	$(PILRC) -I $(RSCDIR) -Fj -q $(RSCDIR)/$(RCPFILE) $(OBJDIR)
	$(TOUCH) $(OBJDIR)/bin.res

debug:
	$(MAKE) clean
	$(MAKE) "DEBUGFLAGS=-DDEBUG -DTARGET_DEBUG"
