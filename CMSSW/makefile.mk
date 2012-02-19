GetCMSSWDir:
	mkdir -p $(PWD)/CMSSW/src/
	mkdir -p $(PWD)/CMSSW/src/FWCore/
	cp -r	$(CMSSWREALEASEBASE)/src/FWCore/Python \
		$(CMSSWREALEASEBASE)/src/FWCore/ParameterSet \
		$(CMSSWREALEASEBASE)/src/FWCore/PythonParameterSet \
		$(CMSSWREALEASEBASE)/src/FWCore/Utilities \
		$(CMSSWREALEASEBASE)/src/FWCore/MessageLogger \
		$(PWD)/CMSSW/src/FWCore/
	mkdir $(PWD)/CMSSW/src/DataFormats/
	cp -r	$(CMSSWREALEASEBASE)/src/DataFormats/Provenance/ ../CMSSW/src/DataFormats/
	mkdir -p $(PWD)/CMSSW/src/DataFormats/
	cp -r 	$(CMSSWREALEASEBASE)/src/DataFormats/Math/ \
		$(CMSSWREALEASEBASE)/src/DataFormats/BeamSpot/ \
		$(PWD)/CMSSW/src/DataFormats/
