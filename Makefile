#  VERSION : 02.11.2000 22:29

PROG = dg.exe

# Test whether necessary environment variables are defined; if not, exit
ifndef HOST_NAME
$(error HOST_NAME not defined)
endif
ifndef COMPILER
$(error COMPILER not defined)
endif

MAKES = Makefile 
# Include global SOLPS compiler settings
ifndef SOLPS_CPP
ifeq ($(shell [ -e ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER} ] && echo yes || echo no ),yes)
  include ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER}
  MAKES += ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER}
  ifeq ($(shell [ -e ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER}.local ] && echo yes || echo no ),yes)
    include ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER}.local
    MAKES += ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER}.local
  endif
else
  $(warning ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER} not found.)
endif
else
  MAKES += ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER}
  ifeq ($(shell [ -e ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER}.local ] && echo yes || echo no ),yes)
    include ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER}.local
    MAKES += ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER}.local
  endif
endif

ifdef SOLPS_DEBUG
EXT_DEBUG = .debug
endif

OBJDIR = ${PWD}/builds/$(HOST_NAME).$(COMPILER)$(EXT_DEBUG)

SHELL  = /bin/sh
DG     = $(OBJDIR)/${PROG}
VPATH  = src

SRCDIR = ${PWD}

include ${OBJDIR}/LISTOBJ

#LIBS = -Wl,-Bstatic -lXm -Wl,-Bdynamic -lXt -lX11 -lm -L/usr/X11R6/lib
#CFLAGS = -g
#CC = cc


ifeq ($(shell [ -e config/config.${HOST_NAME}.${COMPILER} ] && echo yes || echo no ),yes)
include config/config.${HOST_NAME}.${COMPILER}
MAKES+= config/config.${HOST_NAME}.${COMPILER}
else
$(error config/config.${HOST_NAME}.${COMPILER} not found.)
endif

ifeq ($(shell [ -e config/config.${HOST_NAME}.${COMPILER}.local ] && echo yes || echo no ),yes)
include config/config.${HOST_NAME}.${COMPILER}.local
MAKES+= config/config.${HOST_NAME}.${COMPILER}.local
endif

DEST = $(OBJS:%.o=$(OBJDIR)/%.o)

$(OBJDIR)/%.o : %.c
	 $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(DG): $(DEST) $(MAKES)
	 $(CC) $(CFLAGS) $(INCLUDES) -o $@ $(DEST) $(LIBS)

all: VERSION listobj depend $(DG)

update: clean VERSION listobj depend all

.PHONY: VERSION clean listobj update depend all neat tags force

clean:
	/bin/rm -rf ${OBJDIR}/*.o $(DG) ${OBJDIR}/*.bak src/git_version_DG.h ${OBJDIR}/dependencies* ${OBJDIR}/LISTOBJ ${OBJDIR}/dg.dgc ${OBJDIR}/dg.dgh

neat:
	/bin/rm -rf ${OBJDIR}/*.o ${OBJDIR}/*.bak

tags:
	rm -f TAGS ; etags src/*.c src/*.h dg.dgc

depend: ${OBJS:.o=.c}
	-mkdir -p ${OBJDIR}
	-cd ${OBJDIR} ; ln -sf ${SRCDIR}/src/dg.dgh ${SRCDIR}/dg.dgc .
	$(CC) ${INCLUDES} -M $^ | sed '/^[^ ]*.o: / s|^|${OBJDIR}/|' | \
	sed -e 's,^${OBJDIR}/,\$${OBJDIR}/,' | \
	sed 's,: ${SOLPSTOP},: $${SOLPSTOP},' > ${OBJDIR}/dependencies

listobj:
	@P=${OBJDIR}; cd src ; rm -f $${P}/LISTOBJ; touch $${P}/LISTOBJ; \
	echo "OBJS =" *.c | sed -e 's/ [^ /]*\// /g' -e 's/\.c/.o/g' -e 's/res2fbr\.o//g' > $${P}/LISTOBJ

VERSION: src/git_version_DG.h

src/git_version_DG.h: force
	@echo "#define GIT_VERSION_DG \"`git describe --dirty --always`\"" > src/git_version_new.h
	@if cmp -s src/git_version_new.h src/git_version_DG.h; then rm src/git_version_new.h; else mv src/git_version_new.h src/git_version_DG.h; fi

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


