#  VERSION : 21.10.2000 00:50

OBJDIR = ${OBJECTCODE}

VPATH = src
INCLUDE = 
FINCLUDE = -I${FLUSH}/source/default/commons

ALL = cropequ dg2dg dg2ef ef2dg vr2dg nk2dg dg2vr risepsi prinequ jt2dg pb2dg pt2dg

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
preq =	prinequ.o rdeqdg.o rdeqlh.o
pbdg =	pb2dg.o rdeqpb.o wreqdg.o open_files.o
ptdg =	pt2dg.o
culham = culham.o rdefit.o wreqdg.o open_files.o double.o
sstdg = sst2dg.o wreqdg.o open_files.o
shotfile2dg = shotfile2dg.o wreqdg.o
jetppf2dg = jetppf2dg.o wreqdg.o

include config/compiler.${OBJECTCODE}
ifeq ($(shell [ -e config.local/compiler.${OBJECTCODE} ] && echo yes || echo no ),yes)
include config.local/compiler.${OBJECTCODE}
endif

include LISTOBJ

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
PREQ = $(preq:%.o=$(OBJDIR)/%.o)
JTDG = $(jtdg:%.o=$(OBJDIR)/%.o)
PBDG = $(pbdg:%.o=$(OBJDIR)/%.o)
PTDG = $(ptdg:%.o=$(OBJDIR)/%.o)
CULHAM = $(culham:%.o=$(OBJDIR)/%.o)
SSTDG = $(sstdg:%.o=$(OBJDIR)/%.o)
SHOTFILE2DG = $(shotfile2dg:%.o=$(OBJDIR)/%.o)
JETPPF2DG = $(jetppf2dg:%.o=$(OBJDIR)/%.o)

$(OBJDIR)/%.o : %.f
	${CPP} -P -C ${INCLUDE} $< > ${OBJDIR}/$*.f
#	$(COMPILE) -o ${OBJDIR}/$*.o $<
	$(COMPILE) -o ${OBJDIR}/$*.o ${OBJDIR}/$*.f ${FINCLUDE}
#	/bin/rm ${OBJDIR}/$*.f
	if [ -f $*.o ]; then /bin/mv $*.o ${OBJDIR}; fi

all: $(ALL:%=$(OBJDIR)/%)

${OBJDIR}/cropequ: $(CREQ)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/dg2dg: $(DGDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/culham: $(CULHAM)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/sst2dg: $(SSTDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/dg2ef: $(DGEF)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/dg2vr: $(DGVR)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/ef2dg: $(EFDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/vr2dg: $(VRDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/nk2dg: $(NKDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/nkn2dg: $(NKNDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/jt2dg: $(JTDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/risepsi: $(RSPS)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/prinequ: $(PREQ)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/pb2dg: $(PBDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/pt2dg: $(PTDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/dg_minuspsi_dg: $(DGMINUSPSIDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJDIR}/shotfile2dg: $(SHOTFILE2DG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA) $(LDWOS)

${OBJDIR}/jetppf2dg: $(JETPPF2DG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA) $(LDFLUSH)

clean:
	cd ${OBJDIR}; rm -rf *.o $(ALL)

TAGS:	tags

tags:
	rm -f TAGS ; etags src/*.f

depend: ${OBJS:.o=.f}
	makedepend -f  ${OBJDIR}/dependencies ${INCLUDE} $^
	mv ${OBJDIR}/dependencies ${OBJDIR}/dependencies.bak
	sed -e '3,$$s|^|${OBJDIR}/|' ${OBJDIR}/dependencies.bak > ${OBJDIR}/dependencies

LISTOBJ: listobj

listobj:
	@cd src ; echo "OBJS =" *.f | sed -e 's/ [^ /]*\// /g' -e 's/\.f/.o/g' > ../LISTOBJ

${OBJDIR}/dependencies:
	-mkdir -p ${OBJDIR}
	touch ${OBJDIR}/dependencies
	${MAKE} listobj
	${MAKE} depend

include ${OBJDIR}/dependencies

DOC:
	doxygen


