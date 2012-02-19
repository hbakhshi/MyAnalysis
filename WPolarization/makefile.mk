WPOLOBJS = \
	WPolarization.o \
	TTbarSelector.o \
	DileptonAnalyticalSolver.o \
	BJetAssigner.o \
	NeutrinoSolver.o \
	CosThetaAnalysis.o \
	GenRecComparison.o

WPolarizationUSERLIBS = \
	-L$(PYTHONLIB) -lpython2.7 \
	-L$(BOOSTLIB) -lboost_python \
	-lboost_thread \
	-lboost_signals \
	-lboost_filesystem \
	-lboost_system \
	-L$(TARGETDIR) \
	-lNTupleAnalyzer \
	-lDataFormats \
	-lBaseAnalysis

$(PWD)/obj/$(MACHINE)/WPolarization_deps.mk: $(WPOLOBJS:.o=.cc)
	@mkdir -p $(PWD)/obj/$(MACHINE)
	@rm -f $@
	@touch $@
	@for p in  $(^); \
	do \
		echo `basename $$p` | awk -F . '{print $$(NF-1) "_Deps = \\"}' >> $@  ;\
		echo $$p \\ >> $@ ;\
		echo `$(CXX) -MM $(CXXFLAGS) $(INCLUDEDIRS) $$p | grep $(PWD)/ | grep -v $$p | grep -v external | grep -v CMSSW/src/ | tr '\n' ' ' | tr -d '\\'`  >> $@ ; \
	done
	
$(TARGETDIR)/WPolarization :  $(WPOLOBJS:%=$(PWD)/obj/$(MACHINE)/%)
	@echo $@ :
	#@echo $^
	$(LINK.cc) $(CCFLAGS) $(CPPFLAGS) $^ -o $@ $(LDLIBS) $(WPolarizationUSERLIBS)

WPolarization : $(TARGETDIR)/WPolarization
	@echo $@

# Enable dependency checking
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-i386-Linux
