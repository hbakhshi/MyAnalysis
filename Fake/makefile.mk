FRCalcObjsDep = \
	BaseAlgorithm.o \
	CalculateFakeRate.o \
	EleIsoTemplate.o \
	MuonIsoTemplate.o \
	EleJetFR.o \
	FPRatios.o \
	SameFlavourDiLeptonFakeEstimator.o \
	SingleLeptonSelector.o

FRCalcOBJ = $(FRCalcObjsDep) \
	Pmm_FR.o


FRUSERLIBS = \
	$(PYTHONLIB)/libpython2.6.so \
	$(BOOSTLIB)/libboost_python.so \
	$(BOOSTLIB)/libboost_thread.so \
	$(BOOSTLIB)/libboost_signals.so \
	$(BOOSTLIB)/libboost_filesystem.so \
	$(BOOSTLIB)/libboost_system.so \
	-L$(TARGETDIR) \
	-lNTupleAnalyzer \
	-lDataFormats \
	-lBaseAnalysis

$(PWD)/obj/$(MACHINE)/Fake_deps.mk: $(FRCalcObjsDep:.o=.cc)
	@mkdir -p $(PWD)/obj/$(MACHINE)
	@rm -f $@
	@touch $@
	@for p in  $(^); \
	do \
		echo `basename $$p` | awk -F . '{print $$(NF-1) "_Deps = \\"}' >> $@  ;\
		echo $$p \\ >> $@ ;\
		echo `$(CXX) -MM $(CXXFLAGS) $(INCLUDEDIRS) $$p | grep $(PWD)/ | grep -v $$p | grep -v external | grep -v CMSSW/src/ | tr '\n' ' ' | tr -d '\\'`  >> $@ ; \
	done

$(PWD)/obj/$(MACHINE)/Pmm_FR.o:  $(call FindSourceFiles,Pmm) $(value Pmm_Deps)
	@echo $@ :
	@$(CXX) $< -DFRCalculator -c -o $@ $(CXXFLAGS) $(INCLUDEDIRS)

$(TARGETDIR)/FRCalculator:  $(FRCalcOBJ:%=$(PWD)/obj/$(MACHINE)/%)
	@echo $@ :
	@$(LINK.cc) -DFRCalculator $(CCFLAGS) $(CPPFLAGS) $^ -o $@ $(LDLIBS) $(FRUSERLIBS)

# Enable dependency checking
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-i386-Linux
