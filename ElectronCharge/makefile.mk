PMMOBJS = \
	Pmm_Z.o \
	ZEventSelector.o \
	PmmCalculator.o

SUSYSSOBJS = \
	Pmm_Susy.o \
	PMMReader.o \
	Predict.o \
	SSSusySelector.o

DEPObjs = \
	PMMReader.o \
	SSSusySelector.o \
	ZEventSelector.o \
	PmmCalculator.o 	

DEPObjs_cpp = \
	Pmm.o

ElectronChargeUSERLIBS = \
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

$(PWD)/obj/$(MACHINE)/ElectronCharge_deps.mk: $(DEPObjs:.o=.cc) $(DEPObjs_cpp:.o=.cpp)
	@mkdir -p $(PWD)/obj/$(MACHINE)
	@rm -f $@
	@touch $@
	@for p in  $(^); \
	do \
		echo `basename $$p` | awk -F . '{print $$(NF-1) "_Deps = \\"}' >> $@  ;\
		echo $$p \\ >> $@ ;\
		echo `$(CXX) -MM $(CXXFLAGS) $(INCLUDEDIRS) $$p | grep $(PWD)/ | grep -v $$p | grep -v external | grep -v CMSSW/src/ | tr '\n' ' ' | tr -d '\\'`  >> $@ ; \
	done

$(PWD)/obj/$(MACHINE)/Pmm_Z.o:  $(call FindSourceFiles,Pmm) $(value Pmm_Deps)
	@echo $@ :
	@echo `ls -l $(BOOSTLIB)/libboost_python.so`
	@$(CXX) $< -DZSelector -c -o $@ $(CXXFLAGS) $(INCLUDEDIRS)
	
$(PWD)/obj/$(MACHINE)/Pmm_Susy.o:  $(call FindSourceFiles,Pmm) $(value Pmm_Deps)
	@echo $@
	@$(CXX) $< -DSUSYSelector -c -o $@ $(CXXFLAGS) $(INCLUDEDIRS)


$(TARGETDIR)/Pmm: $(PMMOBJS:%=$(PWD)/obj/$(MACHINE)/%)
	@echo $@ :
	@echo `ls -l $(BOOSTLIB)/libboost_python.so`
	$(LINK.cc) -DZSelector $(CCFLAGS) $(CPPFLAGS) $^ -o $@ $(LDLIBS) $(ElectronChargeUSERLIBS)

$(TARGETDIR)/SusySelection:  $(SUSYSSOBJS:%=$(PWD)/obj/$(MACHINE)/%)
	@echo $@ :
	@$(LINK.cc) -DSUSYSelector $(CCFLAGS) $(CPPFLAGS) $^ -o $@ $(LDLIBS) $(ElectronChargeUSERLIBS)

# Enable dependency checking
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-i386-Linux
