#  VERSION : 21.10.2000 00:50

# Test whether necessary environment variables are defined; if not, exit
ifndef HOST_NAME
$(error HOST_NAME not defined)
endif
ifndef COMPILER
$(error COMPILER not defined)
endif
ifdef SOLPS_DEBUG
EXT_DEBUG = .debug
endif

OBJDIR = ${PWD}/builds/${HOST_NAME}.${COMPILER}${EXT_DEBUG}

VPATH = src
INCLUDE =
FINCLUDE =
ifeq ($(shell [ -d ${FLUSH}/source/default/commons ] && echo yes || echo no ),yes)
FINCLUDE += -I${FLUSH}/source/default/commons
endif


PROG_cropequ = cropequ
PROG_dg2dg   = dg2dg
PROG_dg2ef   = dg2ef
PROG_ef2dg   = ef2dg
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
PROG_culham  = culham
PROG_dg_symm_dg     = dg_symm_dg
PROG_dg_minuspsi_dg = dg_minuspsi_dg
PROG_shotfile2dg    = shotfile2dg
PROG_jetppf2dg      = jetppf2dg

ALL = ${PROG_cropequ} ${PROG_dg2dg} ${PROG_dg2ef} ${PROG_ef2dg} ${PROG_vr2dg} ${PROG_nk2dg} ${PROG_dg2vr} ${PROG_risepsi} ${PROG_prinequ} ${PROG_jt2dg} ${PROG_pb2dg} ${PROG_pt2dg} ${PROG_dg_symm_dg} ${PROG_move_equ}


creq =  cropequ.o rdeqdg.o wreqdg.o rdeqlh.o open_files.o
dgdg =	dg2dg.o rdeqdg.o wreqdg.o rdeqlh.o double.o open_files.o
dgminuspsidg =	dg_minuspsi_dg.o rdeqdg.o wreqdg.o rdeqlh.o
dgef =	dg2ef.o rdeqdg.o wrefit.o rdeqlh.o open_files.o
efdg =	ef2dg.o wreqdg.o rdefit.o open_files.o
vrdg =	vr2dg.o wreqdg.o rdeqvr.o open_files.o
nkdg =	nk2dg.o wreqdg.o rdeqnk.o open_files.o
nkndg =  nkn2dg.o wreqdg.o rdeqnk2.o
jtdg =	jt2dg.o rdeqjt.o wreqdg.o rdeqjh.o open_files.o
dgvr =	dg2vr.o rdeqdg.o wreqvr.o rdeqlh.o open_files.o
rsps =	risepsi.o rdeqdg.o wreqdg.o rdeqlh.o open_files.o
mveq =	move_equ.o rdeqdg.o wreqdg.o rdeqlh.o open_files.o
preq =	prinequ.o rdeqdg.o rdeqlh.o
pbdg =	pb2dg.o rdeqpb.o wreqdg.o open_files.o
ptdg =	pt2dg.o
culham = culham.o rdefit.o wreqdg.o open_files.o double.o
sstdg = sst2dg.o wreqdg.o open_files.o
shotfile2dg = shotfile2dg.o wreqdg.o
jetppf2dg = jetppf2dg.o wreqdg.o
dgsymmdg = dg_symm_dg.o rdeqdg.o rdeqlh.o wreqdg.o open_files.o

ifeq ($(shell [ -e config/config.${HOST_NAME}.${COMPILER} ] && echo yes || echo no ),yes)
include config/config.${HOST_NAME}.${COMPILER}
else
$(error config/config.${HOST_NAME}.${COMPILER} not found.)
endif

ifeq ($(shell [ -e config/config.${HOST_NAME}.${COMPILER}.local ] && echo yes || echo no ),yes)
include config/config.${HOST_NAME}.${COMPILER}.local
endif

include $(OBJDIR)/LISTOBJ

CREQ = $(creq:%.o=$(OBJDIR)/%.o)
DGDG = $(dgdg:%.o=$(OBJDIR)/%.o)
DGMINUSPSIDG = $(dgminuspsidg:%.o=$(OBJDIR)/%.o)
DGEF = $(dgef:%.o=$(OBJDIR)/%.o)
DGVR = $(dgvr:%.o=$(OBJDIR)/%.o)
EFDG = $(efdg:%.o=$(OBJDIR)/%.o)
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

$(OBJDIR)/%.o : %.f
	${CPP} -P ${INCLUDE} $< > ${OBJDIR}/$*.f
#	$(COMPILE) -o ${OBJDIR}/$*.o $<
	$(COMPILE) -o ${OBJDIR}/$*.o ${OBJDIR}/$*.f ${FINCLUDE}
#	/bin/rm ${OBJDIR}/$*.f
	if [ -f $*.o ]; then /bin/mv $*.o ${OBJDIR}; fi

all: $(ALL:%=$(OBJDIR)/%)

${OBJDIR}/${PROG_cropequ}: $(CREQ)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_dg2dg}: $(DGDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_culham}: $(CULHAM)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_sst2dg}: $(SSTDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_dg2ef}: $(DGEF)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_dg2vr}: $(DGVR)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_ef2dg}: $(EFDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_vr2dg}: $(VRDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_nk2dg}: $(NKDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_nkn2dg}: $(NKNDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_jt2dg}: $(JTDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_risepsi}: $(RSPS)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_move_equ}: $(MVEQ)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_prinequ}: $(PREQ)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_pb2dg}: $(PBDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_pt2dg}: $(PTDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_dg_minuspsi_dg}: $(DGMINUSPSIDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/${PROG_shotfile2dg}: $(SHOTFILE2DG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA) $(LDWOS)

${OBJDIR}/${PROG_jetppf2dg}: $(JETPPF2DG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA) $(LDFLUSH)

${OBJDIR}/${PROG_dg_symm_dg}: $(DGSYMMDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

clean:
	cd ${OBJDIR}; rm -rf *.o *.f $(ALL) LISTOBJ dependencies*

TAGS:	tags

tags:
	rm -f TAGS ; etags src/*.f src/*.inc

depend: ${OBJS:.o=.f}
	makedepend -f- ${INCLUDE} $^ | \
	sed -e '3,$$s|^|${OBJDIR}/|' | \
	sed -e 's,^${OBJDIR}/,\$${OBJDIR}/,' | \
	sed -e 's,: ${SOLPSTOP},: $${SOLPSTOP},' > ${OBJDIR}/dependencies

$(OBJDIR)/LISTOBJ: listobj

listobj:
	@cd src ; echo "OBJS =" *.f | sed -e 's/ [^ /]*\// /g' -e 's/\.f/.o/g' > $(OBJDIR)/LISTOBJ

${OBJDIR}/dependencies:
	-mkdir -p ${OBJDIR}
	touch ${OBJDIR}/dependencies
	${MAKE} listobj
	${MAKE} depend

DOC: 
	doxygen

include ${OBJDIR}/dependencies


