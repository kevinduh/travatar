include $(top_srcdir)/common.am
TRAVATARH = test-travatar.h test-base.h

AM_CXXFLAGS += -I$(srcdir)/../include -I$(srcdir)/../kenlm -I$(srcdir)/.. -DPKGDATADIR='"$(pkgdatadir)"'

noinst_PROGRAMS = test-travatar
TESTS = test-travatar

nobase_include_HEADERS = \
	test-alignment.h \
	test-base.h \
	test-binarizer.h \
	test-dict.h \
	test-eval-measure.h \
	test-graph-transformer.h \
	test-hyper-graph.h \
	test-io-util.h \
	test-lookup-table.h \
	test-rule-extractor.h \
	test-travatar-runner.h \
	test-tree-io.h \
	test-trimmer.h \
	test-tune.h \
	test-weights.h \
	test-hiero.h \
	test-lookup-table-fsm.h

test_travatar_SOURCES = \
	test-travatar.cc \
	test-alignment.cc \
    test-binarizer.cc \
    test-dict.cc \
    test-eval-measure.cc \
    test-graph-transformer.cc \
    test-hiero.cc \
    test-hyper-graph.cc \
    test-io-util.cc \
    test-lookup-table-fsm.cc \
    test-lookup-table.cc \
    test-lm-composer.cc \
    test-rule-extractor.cc \
    test-travatar-runner.cc \
    test-tree-io.cc \
    test-trimmer.cc \
    test-tune.cc \
    test-weights.cc \
	${TRAVATARH}

test_travatar_LDADD = \
	../lib/libtravatar.la \
	../kenlm/lm/libklm.la \
	../kenlm/util/libklm_util.la \
	../kenlm/search/libklm_search.la \
	../tercpp/libter.la \
	../marisa/libmarisa.la \
	../liblbfgs/liblbfgs.la \
	-lboost_thread${BOOSTMT} \
	-lboost_regex${BOOSTMT} \
	-lboost_iostreams${BOOSTMT} \
	-lboost_system${BOOSTMT} \
	-lz ${LIBRT}
