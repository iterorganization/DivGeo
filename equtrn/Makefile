#  VERSION : 21.10.2000 00:50

# Test whether necessary environment variables are defined; if not, exit
ifndef HOST_NAME
$(error HOST_NAME not defined)
endif
ifndef COMPILER
$(error COMPILER not defined)
endif

MAKETAGS ?= ctags -e -f

MAKES = Makefile
# Include global SOLPS compiler settings
ifndef SOLPS_CPP
  ifeq ($(shell [ -e ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER} ] && echo yes || echo no ),yes)
    include ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER}
    MAKES += ${SOLPSTOP}/SETUP/setup.csh.${HOST_NAME}.${COMPILER} ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER}
  else
    $(warning ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER} not found. Assuming stand-alone compilation.)
  endif
  ifeq ($(shell [ -e ${SOLPSTOP}/SETUP/config.common.${COMPILER} ] && echo yes || echo no ),yes)
    include ${SOLPSTOP}/SETUP/config.common.${COMPILER}
    MAKES += ${SOLPSTOP}/SETUP/config.common.${COMPILER}
  endif
else
  MAKES += ${SOLPSTOP}/Makefile ${SOLPSTOP}/SETUP/setup.csh.${HOST_NAME}.${COMPILER} ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER}
  ifeq ($(shell [ -e ${SOLPSTOP}/SETUP/config.common.${COMPILER} ] && echo yes || echo no ),yes)
    MAKES += ${SOLPSTOP}/SETUP/config.common.${COMPILER}
  endif
endif
ifeq ($(shell [ -e ${SOLPSTOP}/SETUP/setup.csh.${HOST_NAME}.${COMPILER}.local ] && echo yes || echo no ),yes)
  MAKES += ${SOLPSTOP}/SETUP/setup.csh.${HOST_NAME}.${COMPILER}.local
endif
ifeq ($(shell [ -e ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER}.local ] && echo yes || echo no ),yes)
  include ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER}.local
  MAKES += ${SOLPSTOP}/SETUP/config.${HOST_NAME}.${COMPILER}.local
endif

ifdef SOLPS_DEBUG
EXT_DEBUG = .debug
endif

OBJDIR = ${PWD}/builds/${HOST_NAME}.${COMPILER}${EXT_DEBUG}

SRCDIR = ${PWD}/src
VPATH = src
INCLUDE = -I../src
INCMODS += -I${OBJDIR}
FINCLUDE =
ifeq ($(shell [ -d ${FLUSH}/source/default/commons ] && echo yes || echo no ),yes)
FINCLUDE += -I${FLUSH}/source/default/commons
endif

PROG_cropequ = cropequ
PROG_dg2dg   = dg2dg
PROG_dg2ef   = dg2ef
PROG_ef2dg   = ef2dg
PROG_fiesta2dg = fiesta2dg
PROG_cr2dg   = cr2dg
PROG_vr2dg   = vr2dg
PROG_dg2vr   = dg2vr
PROG_nk2dg   = nk2dg
PROG_nkn2dg  = nkn2dg
PROG_risepsi = risepsi
PROG_move_equ = move_equ
PROG_prinequ = prinequ
PROG_jt2dg   = jt2dg
PROG_pb2dg   = pb2dg
PROG_pt2dg   = pt2dg
PROG_sst2dg  = sst2dg
PROG_ids2dg  = ids2dg
PROG_dg2ids  = dg2ids
PROG_culham  = culham
PROG_dg_symm_dg     = dg_symm_dg
PROG_dg_minuspsi_dg = dg_minuspsi_dg
PROG_shotfile2dg    = shotfile2dg
PROG_jetppf2dg      = jetppf2dg
PROG_dgcut = dgcut

.PHONY: DOC listobj

ALL = ${PROG_cropequ} ${PROG_dg2dg} ${PROG_dg2ef} ${PROG_ef2dg} ${PROG_fiesta2dg} ${PROG_cr2dg} ${PROG_vr2dg} ${PROG_nk2dg} ${PROG_dg2vr} ${PROG_risepsi} ${PROG_prinequ} ${PROG_jt2dg} ${PROG_pb2dg} ${PROG_pt2dg} ${PROG_nkn2dg} ${PROG_move_equ} ${PROG_dg_symm_dg} ${PROG_dg_minuspsi_dg} ${PROG_dgcut}

ifeq (${HOST_NAME}, JET)
ifeq (${COMPILER},pgf90)
ALL += ${PROG_jetppf2dg}
endif
endif

ifeq (${HOST_NAME}, IPP)
ALL += ${PROG_shotfile2dg}
endif

ifeq (${HOST_NAME}, ITM)
#ALL += ${PROG_shotfile2dg}
endif

ifdef IMAS_VERSION
ALL += ${PROG_ids2dg} ${PROG_dg2ids}
endif

EQDIMOBJ = eqdim.o eqdim_constants.o

creq = cropequ.o rdeqdg.o wreqdg.o rdeqlh.o open_files.o ${EQDIMOBJ}
dgdg = dg2dg.o rdeqdg.o wreqdg.o rdeqlh.o double.o open_files.o ${EQDIMOBJ}
dgminuspsidg = dg_minuspsi_dg.o rdeqdg.o wreqdg.o rdeqlh.o open_files.o ${EQDIMOBJ}
dgef = dg2ef.o rdeqdg.o wrefit.o rdeqlh.o open_files.o ${EQDIMOBJ}
efdg = ef2dg.o wreqdg.o rdefit.o open_files.o ${EQDIMOBJ}
fiestadg = fiesta2dg.o wreqdg.o rdfiesta.o open_files.o ${EQDIMOBJ}
crdg = cr2dg.o wreqdg.o rdcreate.o open_files.o ${EQDIMOBJ}
vrdg = vr2dg.o wreqdg.o rdeqvr.o open_files.o ${EQDIMOBJ}
nkdg = nk2dg.o wreqdg.o rdeqnk.o open_files.o ${EQDIMOBJ}
nkndg = nkn2dg.o wreqdg.o rdeqnk2.o open_files.o ${EQDIMOBJ}
jtdg = jt2dg.o rdeqjt.o wreqdg.o rdeqjh.o open_files.o ${EQDIMOBJ}
dgvr = dg2vr.o rdeqdg.o wreqvr.o rdeqlh.o open_files.o ${EQDIMOBJ}
rsps = risepsi.o rdeqdg.o wreqdg.o rdeqlh.o open_files.o ${EQDIMOBJ}
mveq = move_equ.o rdeqdg.o wreqdg.o rdeqlh.o open_files.o ${EQDIMOBJ}
preq = prinequ.o rdeqdg.o rdeqlh.o ${EQDIMOBJ}
pbdg = pb2dg.o rdeqpb.o wreqdg.o open_files.o ${EQDIMOBJ}
ptdg = pt2dg.o ${EQDIMOBJ}
culham = culham.o rdefit.o wreqdg.o open_files.o double.o ${EQDIMOBJ}
sstdg = sst2dg.o wreqdg.o open_files.o ${EQDIMOBJ}
shotfile2dg = shotfile2dg.o wreqdg.o ${EQDIMOBJ}
jetppf2dg = jetppf2dg.o wreqdg.o ${EQDIMOBJ}
dgsymmdg = dg_symm_dg.o rdeqdg.o rdeqlh.o wreqdg.o open_files.o ${EQDIMOBJ}
dgcut = dgcut.o rdeqdg.o rdeqlh.o wreqdg.o open_files.o ${EQDIMOBJ}
idsdg = ids2dg.o rdids.o wreqdg.o wrtrdg.o chcase.o streql.o usrnam.o ${EQDIMOBJ}
dgids = dg2ids.o rdtrg.o wrids.o streql.o usrnam.o ${EQDIMOBJ}

ifeq ($(shell [ -e config/config.${HOST_NAME}.${COMPILER} ] && echo yes || echo no ),yes)
include config/config.${HOST_NAME}.${COMPILER}
MAKES += config/config.${HOST_NAME}.${COMPILER}
else
$(error config/config.${HOST_NAME}.${COMPILER} not found.)
endif

ifeq ($(shell [ -e config/config.common.${COMPILER} ] && echo yes || echo no ),yes)
include config/config.common.${COMPILER}
MAKES += config/config.common.${COMPILER}
endif

ifeq ($(shell [ -e config/config.${HOST_NAME}.${COMPILER}.local ] && echo yes || echo no ),yes)
include config/config.${HOST_NAME}.${COMPILER}.local
MAKES += config/config.${HOST_NAME}.${COMPILER}.local
endif

# Verify that some needed variables are defined
ifndef LD_MSCL
ifndef SOLPS_CPP
$(error LD_MSCL not defined!)
else
$(error LD_MSCL not defined. Run the install_dependencies script first.)
endif
endif

# Validate LISTOBJ before including it.  If it is absent or corrupted (e.g.
# left over from a previously interrupted parallel build) delete it so that
# the missing-file recovery path below triggers a clean atomic regeneration
# via the "${OBJDIR}/LISTOBJ: listobj" rule.
$(shell awk 'FNR==1{if(!/^OBJS *=/){e=1;exit}} FNR==2{if(!/^OBJS90 *=/){e=1;exit}} END{exit e}' \
        $(OBJDIR)/LISTOBJ 2>/dev/null || rm -f $(OBJDIR)/LISTOBJ)
include $(OBJDIR)/LISTOBJ

CREQ = $(creq:%.o=$(OBJDIR)/%.o)
DGDG = $(dgdg:%.o=$(OBJDIR)/%.o)
DGMINUSPSIDG = $(dgminuspsidg:%.o=$(OBJDIR)/%.o)
DGEF = $(dgef:%.o=$(OBJDIR)/%.o)
DGVR = $(dgvr:%.o=$(OBJDIR)/%.o)
EFDG = $(efdg:%.o=$(OBJDIR)/%.o)
FIESTADG = $(fiestadg:%.o=$(OBJDIR)/%.o)
CRDG = $(crdg:%.o=$(OBJDIR)/%.o)
VRDG = $(vrdg:%.o=$(OBJDIR)/%.o)
NKDG = $(nkdg:%.o=$(OBJDIR)/%.o)
NKNDG = $(nkndg:%.o=$(OBJDIR)/%.o)
RSPS = $(rsps:%.o=$(OBJDIR)/%.o)
MVEQ = $(mveq:%.o=$(OBJDIR)/%.o)
PREQ = $(preq:%.o=$(OBJDIR)/%.o)
JTDG = $(jtdg:%.o=$(OBJDIR)/%.o)
PBDG = $(pbdg:%.o=$(OBJDIR)/%.o)
PTDG = $(ptdg:%.o=$(OBJDIR)/%.o)
CULHAM = $(culham:%.o=$(OBJDIR)/%.o)
SSTDG = $(sstdg:%.o=$(OBJDIR)/%.o)
SHOTFILE2DG = $(shotfile2dg:%.o=$(OBJDIR)/%.o)
JETPPF2DG = $(jetppf2dg:%.o=$(OBJDIR)/%.o)
DGSYMMDG = $(dgsymmdg:%.o=$(OBJDIR)/%.o)
DGCUT = $(dgcut:%.o=$(OBJDIR)/%.o)
IDSDG = $(idsdg:%.o=$(OBJDIR)/%.o)
DGIDS = $(dgids:%.o=$(OBJDIR)/%.o)

MODLISTF90 = ${SRCDIR}/*.f90

$(OBJDIR)/%.mod : %.f90
	${CPP} ${DEFINES} -P ${SOLPSINCLUDE} ${INCLUDE} $< > ${OBJDIR}/$*.f90
	$(COMPILE) ${INCMODS} ${FFLAGSEXTRA} -o ${OBJDIR}/$*.o ${OBJDIR}/$*.f90 ${FINCLUDE}
	@if [ -f $*.o ]; then /bin/mv $*.o ${OBJDIR}; fi
	@if [ -f ${OBJDIR}/$*.o ]; then touch ${OBJDIR}/$*.o; fi
	@if [ -f $*.mod ]; then /bin/mv $*.mod ${OBJDIR}; fi
	@if [ -f ${OBJDIR}/$*.mod ]; then touch ${OBJDIR}/$*.mod; fi

$(OBJDIR)/%.o : %.f90
	${CPP} ${DEFINES} -P ${SOLPSINCLUDE} ${INCLUDE} $< > ${OBJDIR}/$*.f90
	sleep 1
	$(COMPILE) ${INCMODS} ${FFLAGSEXTRA} -o ${OBJDIR}/$*.o ${OBJDIR}/$*.f90 ${FINCLUDE}
	@if [ -f $*.o ]; then /bin/mv $*.o ${OBJDIR}; fi
	@if [ -f ${OBJDIR}/$*.o ]; then touch ${OBJDIR}/$*.o; fi
	@if [ -f $*.mod ]; then /bin/mv $*.mod ${OBJDIR}; fi
	@if [ -f ${OBJDIR}/$*.mod ]; then touch ${OBJDIR}/$*.mod; fi

$(OBJDIR)/%.o : %.f
	${CPP} ${DEFINES} -P ${SOLPSINCLUDE} ${INCLUDE} $< > ${OBJDIR}/$*.f
	$(COMPILE) ${INCMODS} ${FFLAGSEXTRA} -o ${OBJDIR}/$*.o ${OBJDIR}/$*.f ${FINCLUDE}
	@if [ -f $*.o ]; then /bin/mv $*.o ${OBJDIR}; fi

ifndef NO_MANUAL
all: VERSION $(ALL:%=$(OBJDIR)/%) .doxygen
else
all: VERSION $(ALL:%=$(OBJDIR)/%)
endif

.PHONY: VERSION clean listobj depend all tags force

${OBJDIR}/${PROG_cropequ}: $(CREQ) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(CREQ) $(LDLIBS) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_dg2dg}: $(DGDG) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(DGDG) $(LDLIBS) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_culham}: $(CULHAM) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(CULHAM) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_sst2dg}: $(SSTDG) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(SSTDG) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_dg2ef}: $(DGEF) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(DGEF) $(LDLIBS) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_dg2vr}: $(DGVR) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(DGVR) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_ef2dg}: $(EFDG) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(EFDG) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_fiesta2dg}: $(FIESTADG) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(FIESTADG) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_cr2dg}: $(CRDG) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(CRDG) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_vr2dg}: $(VRDG) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(VRDG) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_nk2dg}: $(NKDG) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(NKDG) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_nkn2dg}: $(NKNDG) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(NKNDG) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_jt2dg}: $(JTDG) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(JTDG) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_risepsi}: $(RSPS) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(RSPS) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_move_equ}: $(MVEQ) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(MVEQ) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_prinequ}: $(PREQ) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(PREQ) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_pb2dg}: $(PBDG) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(PBDG) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_pt2dg}: $(PTDG) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(PTDG) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_dg_minuspsi_dg}: $(DGMINUSPSIDG) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(DGMINUSPSIDG) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_shotfile2dg}: $(SHOTFILE2DG) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(SHOTFILE2DG) $(LDFLAGS) $(LDEXTRA) $(LDWOS)

${OBJDIR}/${PROG_jetppf2dg}: $(JETPPF2DG) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(JETPPF2DG) $(LDFLAGS) $(LDEXTRA) $(LDFLUSH)

${OBJDIR}/${PROG_dg_symm_dg}: $(DGSYMMDG) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(DGSYMMDG) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_dgcut}: $(DGCUT) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(DGCUT) $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_ids2dg}: $(IDSDG) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(IDSDG) $(LDFLAGS) $(LD_AL) $(LDEXTRA)

${OBJDIR}/${PROG_dg2ids}: $(DGIDS) $(MAKES)
	${FC} ${FFLAGS} ${FFLAGSEXTRA} -o $@ $(DGIDS) $(LDFLAGS) $(LD_AL) $(LDEXTRA)

clean:
	cd ${OBJDIR}; rm -rf *.o *.f *.f90 *.mod $(ALL) LISTOBJ dependencies*
	rm -rf .doxygen

TAGS:	tags

tags:
	rm -f TAGS ; ${MAKETAGS} TAGS src/*.f src/*.f90 src/*.inc || touch TAGS

depend: ${OBJS:.o=.f} ${OBJS90:.o=.f90}
	@makedepend ${DEFINES} -f- ${SOLPSINCLUDE} ${INCLUDE} $^ | \
	sed -e '3,$$s|^src/|${OBJDIR}/|' | \
	sed -e 's,^${OBJDIR}/,\$${OBJDIR}/,' | \
	sed -e 's,: ${SOLPSTOP},: $${SOLPSTOP},' > ${OBJDIR}/dependencies
	@echo '#1 ' >> ${OBJDIR}/dependencies
	@egrep -aiH '^ {0,}use ' $^ | grep -v 'IGNORE' | tr , ' '  | \
	awk '{sub("\\.f:",".o:",$$1);sub("\\.f90:",".o:",$$1);sub("^.*/","$${OBJDIR}/",$$1); print $$1,"$${OBJDIR}/"tolower($$3)".mod"}' | \
	sed -e 's|^src/|${OBJDIR}/|' | \
	sed -e 's,^${OBJDIR}/,\$${OBJDIR}/,' | \
	sed -e 's,: ${SOLPSTOP},: $${SOLPSTOP},' >> ${OBJDIR}/dependencies
ifeq ($(shell `pkg-config --exists fundamental-constants`; echo $$?),0)
ifneq (${MOD},o)
	@echo '#2 ' >> ${OBJDIR}/dependencies
	@egrep -aiH '^ {0,}use ' ${MODLISTF90} | grep -v 'IGNORE' | tr , ' ' | \
	awk '{sub("\\.f90:",".${MOD}:",$$1);sub("^.*/","$${OBJDIR}/",$$1); print $$1,"$${OBJDIR}/"tolower($$3)".${MOD}"}' | \
	sed -e 's|^src/|${OBJDIR}/|' | \
	sed -e 's,^${OBJDIR}/,\$${OBJDIR}/,' | \
	sed -e 's,: ${SOLPSTOP},: $${SOLPSTOP},' >> ${OBJDIR}/dependencies
endif
endif

${OBJDIR}/LISTOBJ: listobj

listobj:
	@mkdir -p $(OBJDIR)
	@set -e; _tmpf=$$(mktemp $(OBJDIR)/LISTOBJ.XXXXXX); \
	 ( cd src; echo "OBJS =" *.f | sed -e 's/ [^ /]*\// /g' -e 's/\.f/.o/g' ) > $$_tmpf; \
	 ( cd src; echo "OBJS90 =" *.f90 | sed -e 's/ [^ /]*\// /g' -e 's/\.f90/.o/g' ) >> $$_tmpf; \
	 echo "SRCS =" src/*.f90 src/*.f >> $$_tmpf; \
	 mv $$_tmpf $(OBJDIR)/LISTOBJ

VERSION: ../src/git_version_DG.h

../src/git_version_DG.h: force
	@echo "#define GIT_VERSION_DG \"`git describe --dirty --always`\"" > ${OBJDIR}/git_version_new.h
	@if cmp -s ${OBJDIR}/git_version_new.h ../src/git_version_DG.h; then rm ${OBJDIR}/git_version_new.h; else mv ${OBJDIR}/git_version_new.h ../src/git_version_DG.h; fi

${OBJDIR}/dependencies:
	-mkdir -p ${OBJDIR}
	printf '# Dummy dependencies file for DivGeo/equtrn\n' > ${OBJDIR}/dependencies
	${MAKE} VERSION
	${MAKE} listobj
	${MAKE} depend

.doxygen: ${SRCS} Doxyfile doxygen-patch.sty latex_header.tex tabu_doxygen.sty longtable_doxygen.sty ${MAKES}
	doxygen
	# Remove the early \usepackage{array} block that Doxygen inserts as a
	# workaround for the old tabu package.  Now that tabu is replaced by our
	# longtable-based patch, loading array before infwarerr breaks array's
	# own \@preamerr (it calls a pre-infwarerr \GenericError).  Without this
	# block, array is loaded normally inside doxygen.sty (via longtable),
	# which is after infwarerr -- so all error-handling macros are consistent.
	perl -i -0pe 's/[ \t]*\\ifx\\requestedLaTeXdate\\undefined\n[ \t]*\\usepackage\{array\}[^\n]*\n[ \t]*\\else\n[ \t]*\\usepackage\{array\}[^\n]*\n[ \t]*\\fi\n//' doxygen/latex/refman.tex
	cd doxygen/latex; ${MAKE}; cd .. ; ln -sf latex/refman.pdf .
	touch $@

DOC: .doxygen

# Delete empty/stale dependencies file so make rebuilds it via its rule.
$(shell [ -s ${OBJDIR}/dependencies ] || rm -f ${OBJDIR}/dependencies)
include ${OBJDIR}/dependencies

# Override the %.o pattern rule for the two modules files below with an explicit
# rule whose recipe is a no-op (@:) to avoid race conditions
${OBJDIR}/eqdim_constants.o: ${OBJDIR}/eqdim_constants.mod
	@:
${OBJDIR}/eqdim.o: ${OBJDIR}/eqdim.mod
	@:

ifeq ($(shell [ -e ${PWD}/config/dependencies.local ] && echo yes || echo no ),yes)
include ${PWD}/config/dependencies.local
endif
