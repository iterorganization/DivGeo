#  VERSION : 02.11.2000 22:29

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

OBJDIR = ${PWD}/builds/$(HOST).$(OBJECTCODE)$(EXT_DEBUG)

SHELL  = /bin/sh
DG     = $(OBJDIR)/dg
VPATH  = src

SRCDIR = ${PWD}

include ${OBJDIR}/LISTOBJ

#LIBS = -Wl,-Bstatic -lXm -Wl,-Bdynamic -lXt -lX11 -lm -L/usr/X11R6/lib
#CFLAGS = -g
#CC = cc


ifeq ($(shell [ -e config/config.${HOST}.${OBJECTCODE} ] && echo yes || echo no ),yes)
include config/config.${HOST}.${OBJECTCODE}
else
$(error config/config.${HOST}.${OBJECTCODE} not found.)
endif

ifeq ($(shell [ -e config/config.${HOST}.${OBJECTCODE}.local ] && echo yes || echo no ),yes)
include config/config.${HOST}.${OBJECTCODE}.local
endif

DEST = $(OBJS:%.o=$(OBJDIR)/%.o)

$(OBJDIR)/%.o : %.c
	 $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(DG): $(DEST)
	 $(CC) $(CFLAGS) $(INCLUDES) -o $@ $(DEST) $(LIBS)

all: VERSION listobj depend $(DG)

update: clean VERSION listobj depend all

.PHONY: VERSION clean listobj update depend all neat tags force

clean:
	/bin/rm -rf ${OBJDIR}/*.o $(DG) ${OBJDIR}/*.bak src/git_version.h

neat:
	/bin/rm -rf ${OBJDIR}/*.o ${OBJDIR}/*.bak

tags:
	rm -f TAGS ; etags src/*.c src/*.h

depend: ${OBJS:.o=.c}
	-mkdir -p ${OBJDIR}
	-cd ${OBJDIR} ; ln -sf ${SRCDIR}/src/dg.dgh ${SRCDIR}/dg.dgc .
	$(CC) ${INCLUDES} -M $^ | sed '/^[^ ]*.o: / s|^|${OBJDIR}/|' > ${OBJDIR}/dependencies

listobj:
	@P=${OBJDIR}; cd src ; rm -f $${P}/LISTOBJ; touch $${P}/LISTOBJ; \
	echo "OBJS =" *.c | sed -e 's/ [^ /]*\// /g' -e 's/\.c/.o/g' -e 's/res2fbr\.o//g' > $${P}/LISTOBJ

VERSION: src/git_version.h

src/git_version.h: force
	@echo "#define GIT_VERSION \"`git describe --dirty --always`\"" > src/git_version_new.h
	@if cmp -s src/git_version_new.h src/git_version.h; then rm src/git_version_new.h; else mv src/git_version_new.h src/git_version.h; fi
	

${OBJDIR}/dependencies:
	-mkdir -p ${OBJDIR}
	-cd ${OBJDIR} ; ln -sf ${SRCDIR}/src/dg.dgh ${SRCDIR}/dg.dgc .
	touch ${OBJDIR}/dependencies
	${MAKE} VERSION
	${MAKE} tags
	${MAKE} listobj
	${MAKE} depend

${OBJDIR}/LISTOBJ: listobj

include ${OBJDIR}/dependencies


