#  VERSION : 10.12.2004 19:05

OBJDIR = ${OBJECTCODE}
ifdef SOLPSTOP
OBJDIR = $(SOLPSTOP)/bin/$(OBJECTCODE)/DivGeo/convert
endif


VPATH = src
INCLUDE =

ALL = cnveir cnvtria

include config/compiler.${OBJECTCODE}
ifeq ($(shell [ -e config.local/compiler.${OBJECTCODE} ] && echo yes || echo no ),yes)
include config.local/compiler.${OBJECTCODE}
endif

eirdg = cnveir.o
tridg = cnvtria.o

EIRDG = $(eirdg:%.o=$(OBJDIR)/%.o)
TRIDG = $(tridg:%.o=$(OBJDIR)/%.o)

# .SUFFIXES:	 .o .f

# .f.o:
#	  $(FC) $<

$(OBJDIR)/%.o : %.f
	[ -d ${OBJDIR} ] || mkdir ${OBJDIR}
	$(COMPILE) -o ${OBJDIR}/$*.o $<
	if [ -f $*.o ]; then /bin/mv $*.o ${OBJDIR}; fi

all: $(ALL:%=$(OBJDIR)/%)

${OBJDIR}/cnveir: $(EIRDG) $(SOLPS_LIB)/libmscl.a
	${FC} ${FFLAGS} -o ${OBJDIR}/cnveir $(EIRDG) ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/cnvtria: $(TRIDG)
	${FC} ${FFLAGS} -o ${OBJDIR}/cnvtria $(TRIDG) ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

clean:
	cd ${OBJDIR}; rm -rf *.o $(ALL)

