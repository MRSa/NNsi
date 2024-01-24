#------------------------------------------------------------------------------
# $Id: Makefile,v 1.3 2004/05/09 01:16:08 mrsa Exp $
#
#------------------------------------------------------------------------------
TARGET    = damain
#INCFILES = local.h formID.h formInfo.h proto.h struct.h symbols.h version.h

RCP       = formInfo.rcp
TARGETPRC = checkMailDA.prc
TITLE     = "checkMail DA"
CID       = "cMDA"
TYPE      = "DAcc"

CC        = m68k-palmos-gcc
OBJRES    = m68k-palmos-obj-res
BUILDRC   = build-prc
PILRC     = pilrc
TOUCH     = touch
CFLAGS    = -O2 -Wall
SOURCE    = $(TARGET).c
OBJS      = $(TARGET).o

all: $(TARGETPRC)

$(TARGETPRC): code03e8.grc bin.stamp
	$(BUILDRC) --no-check-resources -t $(TYPE) $(TARGETPRC) $(TITLE) $(CID) code03e8.grc *.bin

code03e8.grc: $(TARGET)
	$(OBJRES) $(TARGET)
	mv code0001.$(TARGET).grc $@
	rm *.$(TARGET).grc

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -nostartfiles $(OBJS) -o $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $<

bin.stamp: $(RCP)
	$(PILRC) $(RCP)
	$(TOUCH) bin.stamp

clean:
	$(RM) -f *.o $(TARGET) *.out *.bin *.stamp *.grc *~ *.bak core

header:
	mv formInfo.h formInfo.h.bak;touch formInfo.h;pilrc -h formInfo.h formInfo.rcp;$(MAKE) clean

