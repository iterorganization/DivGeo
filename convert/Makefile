#  VERSION : 10.12.2004 19:05

VPATH = src
INCLUDE =

ALL = cnveir cnvtria

include config/compiler.${OBJECTCODE}

eirdg = cnveir.o
tridg = cnvtria.o

EIRDG = $(eirdg:%.o=$(OBJECTCODE)/%.o)
TRIDG = $(tridg:%.o=$(OBJECTCODE)/%.o)

# .SUFFIXES:	 .o .f

# .f.o:
#	  $(FC) $<

$(OBJECTCODE)/%.o : %.f
	[ -d  ${OBJECTCODE} ] || mkdir  ${OBJECTCODE}
	$(COMPILE) -o ${OBJECTCODE}/$*.o $<
	if [ -f $*.o ]; then /bin/mv $*.o ${OBJECTCODE}; fi

all: $(ALL:%=$(OBJECTCODE)/%)

${OBJECTCODE}/cnveir: $(EIRDG) $(MSCL)/libmscl.a
	${FC} ${FFLAGS} -o ${OBJECTCODE}/cnveir $(EIRDG) ${LDLIBS} $(LDFLAGS) $(LDEXTRA)
#	if [ -x ${OBJECTCODE}/cnveir ]; then ln -sf ${PWD}/${OBJECTCODE}/cnveir ${SOLPSTOP}/src/bin/${OBJECTCODE}; fi

${OBJECTCODE}/cnvtria: $(TRIDG)
	${FC} ${FFLAGS} -o ${OBJECTCODE}/cnvtria $(TRIDG) ${LDLIBS} $(LDFLAGS) $(LDEXTRA)
#	if [ -x ${OBJECTCODE}/cnvtria ]; then ln -sf ${PWD}/${OBJECTCODE}/cnvtria ${SOLPSTOP}/src/bin/${OBJECTCODE}; fi


