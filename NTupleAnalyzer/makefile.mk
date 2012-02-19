NTupleOBJS =  \
	Configuration.o \
	TreeAnalyzerBase.o \
	TreeReader.o \
	AnaClass.o \
	Davismt2.o \
	Hemisphere.o \
	LeptJetStat.o \
	Trigger.o

ConfOBJS = \
	ParameterSet.o \
	Entry.o \
	FileInPath.o \
	ParameterSetEntry.o \
	VParameterSetEntry.o \
	InputTag.o \
	ESInputTag.o \
	Exception.o \
	EDMException.o \
	types.o \
	MessageSender.o \
	MessageLogger.o \
	MessageDrop.o \
	ELseverityLevel.o \
	MessageLoggerQ.o \
	AbstractMLscribe.o \
	ELstring.o \
	ErrorObj.o \
	Hash.o \
	Digest.o \
	Parse.o \
	ELextendedID.o \
	ProcessDesc.o \
	PythonProcessDesc.o \
	PythonParameterSet.o \
	PythonManager.o \
	PythonWrapper.o \
	LuminosityBlockRange.o \
	initializeModule.o \
	EventRange.o

ConfOBJS_C = \
	TreeClassBase.o

ConfOBJS_c = \
	md5.o

$(PWD)/obj/$(MACHINE)/NTupleAnalyzer_deps.mk: $(ConfOBJS:.o=.cc) $(NTupleOBJS:.o=.cc) $(ConfOBJS_c:.o=.c) $(ConfOBJS_C:.o=.C)
	@mkdir -p $(PWD)/obj/$(MACHINE)
	@rm -f $@
	@touch $@
	@for p in  $(^); \
	do \
		echo `basename $$p` | awk -F . '{print $$(NF-1) "_Deps = \\"}' >> $@  ;\
		echo $$p \\ >> $@ ;\
		echo `$(CXX) -MM $(CXXFLAGS) $(INCLUDEDIRS) $$p | grep $(PWD)/ | grep -v $$p | grep -v external | grep -v CMSSW/src/ | tr '\n' ' ' | tr -d '\\'`  >> $@ ; \
	done

NTupleAnalyzerUSERLIBS = \
	-L$(PYTHONLIB) -lpython2.7 \
	-L$(BOOSTLIB) -lboost_python \
	-lboost_thread \
	-lboost_signals \
	-lboost_filesystem \
	-lboost_system \

$(TARGETDIR)/libNTupleAnalyzer.so: $(NTupleOBJS:%=$(PWD)/obj/$(MACHINE)/%) $(ConfOBJS:%=$(PWD)/obj/$(MACHINE)/%) $(ConfOBJS_c:%=$(PWD)/obj/$(MACHINE)/%) $(ConfOBJS_C:%=$(PWD)/obj/$(MACHINE)/%)
	@echo $@ :
	$(LINK.cc) $(LDFLAGS) $(SOFLAGS) $^ -o $@ $(LDLIBS) $(NTupleAnalyzerUSERLIBS)

.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-i386-Linux
