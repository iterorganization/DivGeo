#  VERSION : 21.10.2000 00:50

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

include LISTOBJ

CREQ = $(creq:%.o=$(OBJECTCODE)/%.o)
DGDG = $(dgdg:%.o=$(OBJECTCODE)/%.o)
DGMINUSPSIDG = $(dgminuspsidg:%.o=$(OBJECTCODE)/%.o)
DGEF = $(dgef:%.o=$(OBJECTCODE)/%.o)
DGVR = $(dgvr:%.o=$(OBJECTCODE)/%.o)
EFDG = $(efdg:%.o=$(OBJECTCODE)/%.o)
VRDG = $(vrdg:%.o=$(OBJECTCODE)/%.o)
NKDG = $(nkdg:%.o=$(OBJECTCODE)/%.o)
NKNDG = $(nkndg:%.o=$(OBJECTCODE)/%.o)
RSPS = $(rsps:%.o=$(OBJECTCODE)/%.o)
PREQ = $(preq:%.o=$(OBJECTCODE)/%.o)
JTDG = $(jtdg:%.o=$(OBJECTCODE)/%.o)
PBDG = $(pbdg:%.o=$(OBJECTCODE)/%.o)
PTDG = $(ptdg:%.o=$(OBJECTCODE)/%.o)
CULHAM = $(culham:%.o=$(OBJECTCODE)/%.o)
SSTDG = $(sstdg:%.o=$(OBJECTCODE)/%.o)
SHOTFILE2DG = $(shotfile2dg:%.o=$(OBJECTCODE)/%.o)
JETPPF2DG = $(jetppf2dg:%.o=$(OBJECTCODE)/%.o)

$(OBJECTCODE)/%.o : %.f
	${CPP} -P -C ${INCLUDE} $< >${OBJECTCODE}/$*.f
#	$(COMPILE) -o ${OBJECTCODE}/$*.o $<
	$(COMPILE) -o ${OBJECTCODE}/$*.o ${OBJECTCODE}/$*.f ${FINCLUDE}
#	/bin/rm ${OBJECTCODE}/$*.f
	if [ -f $*.o ]; then /bin/mv $*.o ${OBJECTCODE}; fi

all: $(ALL:%=$(OBJECTCODE)/%)

${OBJECTCODE}/cropequ: $(CREQ)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)
	if [ -d ${SOLPSTOP}/src/bin/${OBJECTCODE} ]; then ln -sf ${PWD}/${OBJECTCODE}/cropequ ${SOLPSTOP}/src/bin/${OBJECTCODE}; fi

${OBJECTCODE}/dg2dg: $(DGDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)
	if [ -d ${SOLPSTOP}/src/bin/${OBJECTCODE} ]; then ln -sf ${PWD}/${OBJECTCODE}/dg2dg ${SOLPSTOP}/src/bin/${OBJECTCODE}; fi

${OBJECTCODE}/culham: $(CULHAM)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJECTCODE}/sst2dg: $(SSTDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJECTCODE}/dg2ef: $(DGEF)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJECTCODE}/dg2vr: $(DGVR)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJECTCODE}/ef2dg: $(EFDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJECTCODE}/vr2dg: $(VRDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJECTCODE}/nk2dg: $(NKDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJECTCODE}/nkn2dg: $(NKNDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJECTCODE}/jt2dg: $(JTDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJECTCODE}/risepsi: $(RSPS)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJECTCODE}/prinequ: $(PREQ)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJECTCODE}/pb2dg: $(PBDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJECTCODE}/pt2dg: $(PTDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJECTCODE}/dg_minuspsi_dg: $(DGMINUSPSIDG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA)

${OBJECTCODE}/shotfile2dg: $(SHOTFILE2DG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA) $(LDWOS)

${OBJECTCODE}/jetppf2dg: $(JETPPF2DG)
	${FC} ${FFLAGS} -o $@ $^ ${LDLIBS} $(LDFLAGS) $(LDEXTRA) $(LDFLUSH)

clean:
	cd ${OBJECTCODE}; rm -rf *.o $(ALL)

tags:
	rm TAGS ; etags src/*.f

depend: ${OBJS:.o=.f}
	echo ${OBJS}
	echo ${OBJS:.o=.f}
	makedepend -f  ${OBJECTCODE}/dependencies ${INCLUDE} $^
	mv ${OBJECTCODE}/dependencies ${OBJECTCODE}/dependencies.bak
	sed -e '3,$$s/^/${OBJECTCODE}\//' ${OBJECTCODE}/dependencies.bak > ${OBJECTCODE}/dependencies

LISTOBJ: listobj

listobj:
	@cd src ; echo "OBJS =" *.f | sed -e 's/ [^ /]*\// /g' -e 's/\.f/.o/g' > ../LISTOBJ

${OBJECTCODE}/dependencies:
	-mkdir ${OBJECTCODE}
	touch ${OBJECTCODE}/dependencies
	${MAKE} depend

include ${OBJECTCODE}/dependencies

DOC:
	doxygen


