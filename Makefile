#  VERSION : 02.11.2000 22:29

SHELL=/bin/sh
DG=$(OBJECTCODE)/dg
VPATH=src

include LISTOBJ

#LIBS=  -Wl,-Bstatic -lXm -Wl,-Bdynamic -lXt -lX11 -lm -L/usr/X11R6/lib
#CFLAGS= -g
#CC=cc

include config/compiler.$(OBJECTCODE)
ifeq ($(shell [ -e config.local/compiler.${OBJECTCODE} ] && echo yes || echo no ),yes)
include config.local/compiler.${OBJECTCODE}
endif

DEST = $(OBJS:%.o=$(OBJECTCODE)/%.o)

$(OBJECTCODE)/%.o : %.c
	 $(CC) $(CFLAGS) -c $< -o $@

$(DG): $(DEST)
	 $(CC) $(CFLAGS) -o $@ $(DEST) $(LIBS)

all: listobj depend $(DG)

update: clean listobj depend all

clean:
	/bin/rm -rf ${OBJECTCODE}/*.o $(DG) *.bak

neat:
	/bin/rm -rf ${OBJECTCODE}/*.o *.bak

tags:
	etags src/*.c src/*.h

depend: ${OBJS:.o=.c}
	makedepend -f  ${OBJECTCODE}/dependencies $^; \
	mv ${OBJECTCODE}/dependencies ${OBJECTCODE}/dependencies.bak; \
	sed -e '3,$$s/^/${OBJECTCODE}\//' ${OBJECTCODE}/dependencies.bak > ${OBJECTCODE}/dependencies

listobj:
	@P=`pwd`; cd src ; rm -f $${P}/LISTOBJ; touch $${P}/LISTOBJ; \
	echo "OBJS =" *.c | sed -e 's/ [^ /]*\// /g' -e 's/\.c/.o/g' -e 's/res2fbr\.o//g' > $${P}/LISTOBJ

${OBJECTCODE}/dependencies:
	-mkdir ${OBJECTCODE}
	-cd ${OBJECTCODE} ; ln -s ../src/dg.dgh ../dg.dgc .
	touch ${OBJECTCODE}/dependencies
	${MAKE} depend

LISTOBJ:
	touch LISTOBJ

include ${OBJECTCODE}/dependencies

