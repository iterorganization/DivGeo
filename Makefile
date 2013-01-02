#  VERSION : 02.11.2000 22:29

SHELL = /bin/sh
DG = $(OBJECTCODE)/dg
VPATH = src

include LISTOBJ

#LIBS = -Wl,-Bstatic -lXm -Wl,-Bdynamic -lXt -lX11 -lm -L/usr/X11R6/lib
#CFLAGS = -g
#CC = cc

include config/compiler.$(OBJECTCODE)
ifeq ($(shell [ -e config.local/compiler.${OBJECTCODE} ] && echo yes || echo no ),yes)
include config.local/compiler.${OBJECTCODE}
endif

DEST = $(OBJS:%.o=$(OBJECTCODE)/%.o)

$(OBJECTCODE)/%.o : %.c
	 $(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(DG): $(DEST)
	 $(CC) $(CFLAGS) $(INCLUDE) -o $@ $(DEST) $(LIBS)

all: listobj depend $(DG)

update: clean listobj depend all

clean:
	/bin/rm -rf ${OBJECTCODE}/*.o $(DG) ${OBJECTCODE}/*.bak

neat:
	/bin/rm -rf ${OBJECTCODE}/*.o ${OBJECTCODE}/*.bak

tags:
	rm -f TAGS ; etags src/*.c src/*.h

depend: ${OBJS:.o=.c}
	-mkdir -p ${OBJECTCODE}
	-cd ${OBJECTCODE} ; ln -sf ../src/dg.dgh ../dg.dgc .
	$(CC) ${INCLUDE} -M $^ > ${OBJECTCODE}/dependencies

listobj:
	@P=`pwd`; cd src ; rm -f $${P}/LISTOBJ; touch $${P}/LISTOBJ; \
	echo "OBJS =" *.c | sed -e 's/ [^ /]*\// /g' -e 's/\.c/.o/g' -e 's/res2fbr\.o//g' > $${P}/LISTOBJ

${OBJECTCODE}/dependencies:
	-mkdir -p ${OBJECTCODE}
	-cd ${OBJECTCODE} ; ln -sf ../src/dg.dgh ../dg.dgc .
	touch ${OBJECTCODE}/dependencies
	${MAKE} listobj
	${MAKE} depend

LISTOBJ: listobj

include ${OBJECTCODE}/dependencies

