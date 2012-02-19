DATAFORMATOBJS =  \
	DiElectronEvent.o \
	Electron.o \
	DiLeptonTTBarEvent.o


$(PWD)/obj/$(MACHINE)/DataFormats_deps.mk: $(DATAFORMATOBJS:.o=.cc)
	@mkdir -p $(PWD)/obj/$(MACHINE)	
	@rm -f $@
	@touch $@
	@for p in  $(^); \
	do \
		echo `basename $$p` | awk -F . '{print $$(NF-1) "_Deps = \\"}' >> $@  ;\
		echo $$p \\ >> $@ ;\
		echo `$(CXX) -MM $(CXXFLAGS) $(INCLUDEDIRS) $$p | grep $(PWD)/ | grep -v $$p | grep -v external | grep -v CMSSW/src/ | tr '\n' ' ' | tr -d '\\'`  >> $@ ; \
	done

$(TARGETDIR)/libDataFormats.so:  $(DATAFORMATOBJS:%=$(PWD)/obj/$(MACHINE)/%)
	@$(LINK.cc) $(LDFLAGS) $(SOFLAGS) $^ -o $@ $(LDLIBS)

# Enable dependency checking
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-i386-Linux
