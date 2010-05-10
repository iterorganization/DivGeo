#  VERSION : 10.12.2004 19:05

VPATH = src
INCLUDE =

ALL = cnveir cnvtria

include config/compiler.${OBJECTCODE}
ifeq ($(shell [ -e config.local/compiler.${OBJECTCODE} ] && echo yes || echo no ),yes)
include config.local/compiler.${OBJECTCODE}
endif

eirdg = cnveir.o
tridg = cnvtria.o

EIRDG = $(eirdg:%.o=$(OBJECTCODE)/%.o)
TRIDG = $(tridg:%.o=$(OBJECTCODE)/%.o)

# .SUFFIXES:	 .o .f

# .f.o:
#	  $(FC) $<

$(OBJECTCODE)/%.o : %.f
	[ -d ${OBJECTCODE} ] || mkdir ${OBJECTCODE}
	$(COMPILE) -o ${OBJECTCODE}/$*.o $<
	if [ -f $*.o ]; then /bin/mv $*.o ${OBJECTCODE}; fi

all: $(ALL:%=$(OBJECTCODE)/%)

${OBJECTCODE}/cnveir: $(EIRDG) $(SOLPS_LIB)/libmscl.a
	${FC} ${FFLAGS} -o ${OBJECTCODE}/cnveir $(EIRDG) ${LDLIBS} $(LDFLAGS) $(LDEXTRA)
	if [ -d ${SOLPSTOP}/src/bin/${OBJECTCODE} ]; then ln -sf ${PWD}/${OBJECTCODE}/cnveir ${SOLPSTOP}/src/bin/${OBJECTCODE}; fi
	if [ -d ${SOLPSTOP}/bin/${OBJECTCODE} ]; then ln -sf ${PWD}/${OBJECTCODE}/cnveir ${SOLPSTOP}/bin/${OBJECTCODE}; fi

${OBJECTCODE}/cnvtria: $(TRIDG)
	${FC} ${FFLAGS} -o ${OBJECTCODE}/cnvtria $(TRIDG) ${LDLIBS} $(LDFLAGS) $(LDEXTRA)
	if [ -d ${SOLPSTOP}/src/bin/${OBJECTCODE} ]; then ln -sf ${PWD}/${OBJECTCODE}/cnvtria ${SOLPSTOP}/src/bin/${OBJECTCODE}; fi
	if [ -d ${SOLPSTOP}/bin/${OBJECTCODE} ]; then ln -sf ${PWD}/${OBJECTCODE}/cnvtria ${SOLPSTOP}/bin/${OBJECTCODE}; fi

clean:
	cd ${OBJECTCODE}; rm -rf *.o $(ALL)
 
