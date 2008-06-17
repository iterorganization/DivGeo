#  VERSION : 14.07.98 20:13

VPATH = src
INCLUDE =

ALL = cnveir

include config/compiler.${OBJECTCODE}

#FC = f77 -c
#LN = f77

eirdg = cnveir.o

EIRDG = $(eirdg:%.o=$(OBJECTCODE)/%.o)

# .SUFFIXES:	 .o .f

# .f.o:
#	  $(FC) $<

$(OBJECTCODE)/%.o : %.f
	$(COMPILE) -o ${OBJECTCODE}/$*.o $<
	if [ -f $*.o ]; then /bin/mv $*.o ${OBJECTCODE}; fi

all: $(ALL:%=$(OBJECTCODE)/%)

${OBJECTCODE}/cnveir: $(EIRDG) $(MSCL)/libmscl.a
	${FC} ${FFLAGS} -o ${OBJECTCODE}/cnveir $(EIRDG) ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

