MACHINE1 = $(shell uname -n)
MACHINE = $(if $(findstring lxplus, $(MACHINE1)),lxplus,$(MACHINE1))
PWD = $(shell pwd)

ROOTCFLAGS     = $(shell $(PWD)/external/$(MACHINE)/root-config --cflags)
ROOTLIBS       = $(shell $(PWD)/external/$(MACHINE)/root-config --libs)
ROOTGLIBS      = $(shell $(PWD)/external/$(MACHINE)/root-config --glibs)

PYTHONINCLUDE = $(PWD)/external/$(MACHINE)/PYTHONINCLUDE
PYTHONLIB = $(PWD)/external/$(MACHINE)/PYTHONLIB
BOOSTINCLUDE = $(PWD)/external/$(MACHINE)/BOOSTINCLUDE
BOOSTLIB = $(PWD)/external/$(MACHINE)/BOOSTLIB
CLHEPINCLUDE = $(PWD)/external/$(MACHINE)/CLHEPINCLUDE
CMSSWREALEASEBASE = $(PWD)/external/$(MACHINE)/CMSSWREALEASEBASE

python_version_full := $(wordlist 2,4,$(subst ., ,$(shell python --version 2>&1)))
python_version_major := $(word 1,${python_version_full})
python_version_minor := $(word 2,${python_version_full})
python_version_patch := $(word 3,${python_version_full})
python_lib = -lpython$(python_version_major).$(python_version_minor)

CXX = $(if $(wildcard $(PWD)/external/$(MACHINE)/GCC),$(PWD)/external/$(MACHINE)/GCC/bin/g++ ,g++)
BASICOPTS = -O0 -g -DDEBUG

CCFLAGS = $(BASICOPTS) -fPIC
CXXFLAGS = $(BASICOPTS) $(ROOTCFLAGS) -fPIC -DBOOST_FILESYSTEM_VERSION=2 

LDFLAGS        = $(BASICOPTS) #-g
SOFLAGS        = -O -shared

TARGETDIR = $(PWD)/bin/$(MACHINE)

INCLUDEDIRS = \
	-I$(PWD)/NTupleAnalyzer/include \
	-I$(PWD)/ \
	-I$(CMSSWREALEASEBASE)/src \
	-I$(PYTHONINCLUDE) \
	-I$(BOOSTINCLUDE) \
	-I$(CLHEPINCLUDE)

VPATH = $(PWD)/NTupleAnalyzer/src/base/:\
	$(PWD)/NTupleAnalyzer/src/helper/:\
	$(PWD)/DataFormats/src/:\
	$(PWD)/BaseAnalysis/src/:\
	$(PWD)/ElectronCharge/src/:\
	$(PWD)/WPolarization/src/:\
	$(PWD)/Fake/src/:\
	$(CMSSWREALEASEBASE)/src/FWCore/Python/src/:\
	$(CMSSWREALEASEBASE)/src/FWCore/ParameterSet/src/:\
	$(CMSSWREALEASEBASE)/src/DataFormats/Provenance/src/:\
	$(CMSSWREALEASEBASE)/src/FWCore/PythonParameterSet/src/:\
	$(CMSSWREALEASEBASE)/src/FWCore/Utilities/src/:\
	$(CMSSWREALEASEBASE)/src/FWCore/MessageLogger/src/

LDLIBS = $(ROOTGLIBS) -lTree -lTreePlayer -lGenVector \
	$(if $(wildcard $(PWD)/external/$(MACHINE)/PCRE/libpcre.so.0),$(PWD)/external/$(MACHINE)/PCRE/libpcre.so.0,) \
	$(if $(wildcard $(PWD)/external/$(MACHINE)/DCAP/libdcap.so),$(PWD)/external/$(MACHINE)/DCAP/libdcap.so $(PWD)/external/$(MACHINE)/DCAP/libpdcap.so,)

.PHONY : all clean distclean prepare

all:    $(TARGETDIR) \
	$(TARGETDIR)/libNTupleAnalyzer.so \
	$(TARGETDIR)/libDataFormats.so \
	$(TARGETDIR)/libBaseAnalysis.so \
	$(TARGETDIR)/WPolarization \
	$(TARGETDIR)/Pmm \
	$(TARGETDIR)/SusySelection \
	$(TARGETDIR)/FRCalculator \
	$(TARGETDIR)/FitValidation \
	$(PWD)/external/$(MACHINE).tar
	@echo $(python_lib)
	@echo $(LDLIBS)
	@echo $(CXX)

$(PWD)/external/$(MACHINE).tar: external/$(MACHINE)
	tar -cf $@ $<

prepare: $(PWD)/external/$(MACHINE).tar
	rm -rf external/$(MACHINE)
	tar -xf external/$(MACHINE).tar -C external/


FindSourceFiles = $(shell find . -name "$1.cc") $(shell find . -name "$1.C") $(shell find . -name "$1.cpp") $(shell find . -name "$1.c")
#FindHeaderFile = $(shell find . -name "$1.h") $(shell find . -name "$1.hh")
#FindDependenciesFiles = $(shell $(CXX) -MM $2 $(CXXFLAGS) $(INCLUDEDIRS) $1 | grep $(PWD)/ | grep -v external | grep -v CMSSW/src/ | tr '\n' ' ' | tr -d '\\' )

Directories = NTupleAnalyzer DataFormats BaseAnalysis ElectronCharge Fake WPolarization
include $(Directories:%=%/makefile.mk)
include $(Directories:%=$(PWD)/obj/$(MACHINE)/%_deps.mk)
include CMSSW/makefile.mk

.SECONDEXPANSION:

$(PWD)/obj/$(MACHINE)/%.o: $$(call FindSourceFiles,$$(*F)) $$(value $$(*F)_Deps)
	@echo $@ :
	#@echo $^
	$(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDEDIRS)

clean:
	rm -f $(TARGETDIR)/*
	rm -f $(PWD)/obj/$(MACHINE)/*.o

distclean:
	rm -rf bin/*
	rm -rf obj/*

$(TARGETDIR):
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(PWD)/obj/$(MACHINE)/

$(PWD)/../Analysis.bz2.tar: CMSSW \
		ElectronCharge \
		BaseAnalysis \
		NTupleAnalyzer \
		DataFormats \
		Run \
		Makefile
	tar -cjf $@ \
		$^ \
		--exclude='*.root' --exclude='*LSF*' --exclude='*Runs*' --exclude='*.pyc' --exclude='*GetData*'

# Enable dependency checking
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-i386-Linux
