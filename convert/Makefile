#  VERSION : 10.12.2004 19:05

# Test whether necessary environment variables are defined; if not, exit
ifndef HOST
$(error HOST not defined)
endif
ifndef OBJECTCODE
$(error OBJECTCODE not defined)
endif
ifdef SOLPS_DEBUG
EXT_DEBUG = .debug
endif

OBJDIR = ${PWD}/builds/${HOST}.${OBJECTCODE}${EXT_DEBUG}

VPATH = src
INCLUDE =

ALL = cnveir cnvtria

ifeq ($(shell [ -e config/config.${HOST}.${OBJECTCODE} ] && echo yes || echo no ),yes)
include config/config.${HOST}.${OBJECTCODE}
else
$(error config/config.${HOST}.${OBJECTCODE} not found.)
endif

ifeq ($(shell [ -e config/config.${HOST}.${OBJECTCODE}.local ] && echo yes || echo no ),yes)
include config/config.${HOST}.${OBJECTCODE}.local
endif

eirdg = cnveir.o
tridg = cnvtria.o

EIRDG = $(eirdg:%.o=$(OBJDIR)/%.o)
TRIDG = $(tridg:%.o=$(OBJDIR)/%.o)

# .SUFFIXES:	 .o .f

# .f.o:
#	  $(FC) $<

$(OBJDIR)/%.o : %.f
	[ -d ${OBJDIR} ] || mkdir -p ${OBJDIR}
	$(COMPILE) -o ${OBJDIR}/$*.o $<
	if [ -f $*.o ]; then /bin/mv $*.o ${OBJDIR}; fi

all: $(ALL:%=$(OBJDIR)/%)

${OBJDIR}/cnveir: $(EIRDG)
	${FC} ${FFLAGS} -o ${OBJDIR}/cnveir $(EIRDG) ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/cnvtria: $(TRIDG)
	${FC} ${FFLAGS} -o ${OBJDIR}/cnvtria $(TRIDG) ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

clean:
	cd ${OBJDIR}; rm -rf *.o $(ALL)

