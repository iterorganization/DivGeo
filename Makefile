#  VERSION : 02.11.2000 22:29

OBJDIR = $(OBJECTCODE)

SHELL  = /bin/sh
DG     = $(OBJDIR)/dg
VPATH  = src

SRCDIR = ${PWD}

include LISTOBJ

#LIBS = -Wl,-Bstatic -lXm -Wl,-Bdynamic -lXt -lX11 -lm -L/usr/X11R6/lib
#CFLAGS = -g
#CC = cc

include config/compiler.$(OBJECTCODE)
ifeq ($(shell [ -e config.local/compiler.${OBJECTCODE} ] && echo yes || echo no ),yes)
include config.local/compiler.${OBJECTCODE}
endif

DEST = $(OBJS:%.o=$(OBJDIR)/%.o)

$(OBJDIR)/%.o : %.c
	 $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(DG): $(DEST)
	 $(CC) $(CFLAGS) $(INCLUDES) -o $@ $(DEST) $(LIBS)

all: VERSION listobj depend $(DG)

update: clean VERSION listobj depend all

.PHONY: VERSION clean listobj update depend all neat tags

clean:
	/bin/rm -rf ${OBJDIR}/*.o $(DG) ${OBJDIR}/*.bak src/git_version.h

neat:
	/bin/rm -rf ${OBJDIR}/*.o ${OBJDIR}/*.bak

tags:
	rm -f TAGS ; etags src/*.c src/*.h

depend: ${OBJS:.o=.c}
	-mkdir -p ${OBJDIR}
	-cd ${OBJDIR} ; ln -sf ${SRCDIR}/src/dg.dgh ${SRCDIR}/dg.dgc .
	$(CC) ${INCLUDES} -M $^ > ${OBJDIR}/dependencies

listobj:
	@P=`pwd`; cd src ; rm -f $${P}/LISTOBJ; touch $${P}/LISTOBJ; \
	echo "OBJS =" *.c | sed -e 's/ [^ /]*\// /g' -e 's/\.c/.o/g' -e 's/res2fbr\.o//g' > $${P}/LISTOBJ

VERSION: src/git_version.h

src/git_version.h:
ifeq ($(shell [ -d ${SOLPSTOP} ] && echo yes || echo no ),yes)
	echo "#define GIT_VERSION \0042`(cd ${SOLPSTOP}; git describe --dirty --always)`\0042" > src/git_version.h
else
	echo "#define GIT_VERSION \0042`git describe --dirty --always`\0042" > src/git_version.h
endif

${OBJDIR}/dependencies:
	-mkdir -p ${OBJDIR}
	-cd ${OBJDIR} ; ln -sf ${SRCDIR}/src/dg.dgh ${SRCDIR}/dg.dgc .
	touch ${OBJDIR}/dependencies
	${MAKE} VERSION
	${MAKE} tags
	${MAKE} listobj
	${MAKE} depend

LISTOBJ: listobj

include ${OBJDIR}/dependencies

