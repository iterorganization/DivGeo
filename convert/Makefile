#  VERSION : 10.12.2004 19:05

# Test whether necessary environment variables are defined; if not, exit
ifndef HOST
$(error HOST not defined)
endif
ifndef COMPILER
$(error COMPILER not defined)
endif
ifdef SOLPS_DEBUG
EXT_DEBUG = .debug
endif

OBJDIR = ${PWD}/builds/${HOST}.${COMPILER}${EXT_DEBUG}

VPATH = src
INCLUDE =

PROGEIR  = cnveir.exe
PROGTRIA = cnvtria.exe

ALL = ${PROGEIR} ${PROGTRIA}

ifeq ($(shell [ -e config/config.${HOST}.${COMPILER} ] && echo yes || echo no ),yes)
include config/config.${HOST}.${COMPILER}
else
$(error config/config.${HOST}.${COMPILER} not found.)
endif

ifeq ($(shell [ -e config/config.${HOST}.${COMPILER}.local ] && echo yes || echo no ),yes)
include config/config.${HOST}.${COMPILER}.local
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

${OBJDIR}/${PROGEIR}: $(EIRDG)
	${FC} ${FFLAGS} -o ${OBJDIR}/${PROGEIR} $(EIRDG) ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROGTRIA}: $(TRIDG)
	${FC} ${FFLAGS} -o ${OBJDIR}/${PROGTRIA} $(TRIDG) ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

clean:
	cd ${OBJDIR}; rm -rf *.o $(ALL)

