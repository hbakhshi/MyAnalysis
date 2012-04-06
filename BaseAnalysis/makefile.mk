BASEOBJS =  \
	EfficiencyExt.o \
	EfficiencyH1.o \
	ObjectIDHandler.o \
	TreeCreator.o


$(PWD)/obj/$(MACHINE)/BaseAnalysis_deps.mk: $(BASEOBJS:.o=.cc)
	@mkdir -p $(PWD)/obj/$(MACHINE)
	@rm -f $@
	@touch $@
	@for p in  $(^); \
	do \
		echo `basename $$p` | awk -F . '{print $$(NF-1) "_Deps = \\"}' >> $@  ;\
		echo $$p \\ >> $@ ;\
		echo `$(CXX) -MM $(CXXFLAGS) $(INCLUDEDIRS) $$p | grep $(PWD)/ | grep -v $$p | grep -v external | grep -v CMSSW/src/ | tr '\n' ' ' | tr -d '\\'`  >> $@ ; \
	done
	
$(TARGETDIR)/libBaseAnalysis.so: $(BASEOBJS:%=$(PWD)/obj/$(MACHINE)/%)
	@$(LINK.cc) $(LDFLAGS) $(SOFLAGS) $^ -o $@ $(LDLIBS)

$(TARGETDIR)/BaseAnalysisTester: $(PWD)/obj/$(MACHINE)/Tester.o $(TARGETDIR)/libBaseAnalysis.so
	$(LINK.cc) $(CCFLAGS) $(CPPFLAGS) -g -gdb $< -o $@ $(LDLIBS) $(TARGETDIR)/libBaseAnalysis.so
	#$(PWD)/external/$(MACHINE)/BOOSTLIB/libboost_thread.so
	@echo Starting execution of $(@F)
	@$@

# Enable dependency checking
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-i386-Linux

