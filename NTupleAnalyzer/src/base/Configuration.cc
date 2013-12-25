#include "base/Configuration.h"

RunInfo* info::TheInfo = new RunInfo();
int info::CPU_Number(0);


class SiStripDQMFolder {
public:
  SiStripDQMFolder( edm::ParameterSet parent_dir , const SiStripDQMFolder* TheParent = NULL ){
    this->Name = parent_dir.getParameter< string >("Name");
    this->DetIdRanges = parent_dir.getParameter< std::vector<std::string> >("DetIds");
    this->Parent = TheParent ;

    SubDirInfo = parent_dir.getParameterSetVector("SubDirs");
    SubDirs.reserve(SubDirInfo.size());

    int subdir_id = 0;
    for( edm::VParameterSet::const_iterator subdir = SubDirInfo.begin() ; subdir != SubDirInfo.end() ; subdir++){
      SiStripDQMFolder* subfolder = new SiStripDQMFolder( *subdir , this ); //(&SubDirs[subdir_id])
      SubDirs.push_back( subfolder );
      subdir_id ++;
    }
  }

  string GetTabs() const{
    string ret;
    if(Parent == NULL)
      ret = "";
    else
      ret = (Parent)->GetTabs() + "\t";

    return ret;
  }

  string GetFullName() const{
    string ret;
    if(Parent == NULL)
      ret = this->Name ;
    else
      ret = (Parent)->GetFullName() + "/" + this->Name ;

    return ret;
  }

  void Print() const{
    cout << GetFullName() << endl;
    for( std::vector< SiStripDQMFolder* >::const_iterator subs = SubDirs.begin() ; subs != SubDirs.end() ; subs++ )
      (*subs)->Print();
  }

  std::string Name;
  std::vector<std::string> DetIdRanges;

  edm::VParameterSet SubDirInfo;
  std::vector< SiStripDQMFolder* > SubDirs;

  const SiStripDQMFolder* Parent;
};

void info::AnalyzeArgs(int argc, char** argv , std::string module_name) {
    // only allow one argument for this simple example which should be the
    // the python cfg file
    if (argc < 2) {
        std::cout << "Usage : " << argv[0] << " [parameters.py]" << std::endl;
        exit(EXIT_FAILURE);
    }

    // get the python configuration
    PythonProcessDesc builder(argv[1]);
    const edm::ParameterSet& param_test = builder.processDesc()->getProcessPSet()->getParameter<edm::ParameterSet > ("Test");
    SiStripDQMFolder a( param_test , NULL);
    a.Print();

    const edm::ParameterSet& params = builder.processDesc()->getProcessPSet()->getParameter<edm::ParameterSet > (module_name);

    info::TheInfo->Verbosity = params.getUntrackedParameter<int>("Verbosity", 0);
    info::TheInfo->IntLumi = params.getParameter<double>("IntLumi");
    info::TheInfo->Name = params.getUntrackedParameter<string > ("Name", "Analyze");
    info::TheInfo->NumberOfEvents = params.getUntrackedParameter<int>("TotalInput", -1);
    info::TheInfo->NumberOfEventsPerPrintLoop = params.getUntrackedParameter<int>("NumberOfEventsPerPrintLoop", 10000);
    info::TheInfo->PrintTriggerTable = params.getUntrackedParameter<bool>("PrintTriggerTable", false);
    info::TheInfo->outfilename = params.getParameter<string > ("OutFileName");
    info::TheInfo->CopyTree = params.getParameter<bool> ("CopyTree");
    info::TheInfo->CopyTreeFileName = params.getParameter<string > ("CopyTreeFileName");

    info::TheInfo->SelectorConfigs = params.getParameter<edm::ParameterSet > ("SelectorConfigs");
    //info::TheInfo->ElectronSelectionCuts.Set(params.getParameter<edm::ParameterSet > ("ElectronSelection"));
    //if (Verbosity > 2)
    //info::TheInfo->ElectronSelectionCuts.Print();

    //    info::TheInfo->EventSelectionCuts.Set();
    //    if (Verbosity > 2)
    //        info::TheInfo->EventSelectionCuts.Print();

    std::vector<string> inputs = params.getParameter< vector<string> > ("Inputs");

    for (std::vector<string>::const_iterator input = inputs.begin();
            input != inputs.end(); input++) {
        DataSet ds(builder.processDesc()->getProcessPSet()->getParameterSetVector(*input), *input);
        info::TheInfo->dataSets.push_back(ds);
    }

    if (info::TheInfo->Verbosity > 2) info::TheInfo->dataSets.Print();

    std::vector<string> triggers = params.getParameter< vector<string> > ("HLTs");

    for (std::vector<string>::const_iterator trigger = triggers.begin();
            trigger != triggers.end(); trigger++) {
        TriggerCutFullRunRange trgs(builder.processDesc()->getProcessPSet()->getParameterSetVector(*trigger), *trigger);
        info::TheInfo->triggerCuts.push_back(trgs);
    }
    info::TheInfo->triggerCuts.SetHistosEnable(params.getParameter< bool> ("TriggerHistosEnable"));
    info::TheInfo->triggerCuts.CreateAnalyzers();

    if (info::TheInfo->Verbosity > 2) info::TheInfo->triggerCuts.Print();

    info::TheInfo->Analyzers = params.getParameterSetVector("Analyzers");

    const edm::ParameterSet& RunSetup = builder.processDesc()->getProcessPSet()->getUntrackedParameter<edm::ParameterSet> ("RunSetup");
    info::CPU_Number = RunSetup.getUntrackedParameter<int > ("CPU_Number",0);
}

extern "C" {

    void initlibFWCorePython() {

    };
}

std::vector <std::string>* read_directory(const std::string& path, std::vector <std::string>* result) {
    dirent* de;
    DIR* dp;
    errno = 0;
    dp = opendir(path.empty() ? "." : path.c_str());
    if (dp) {
        while (true) {
            errno = 0;
            de = readdir(dp);
            if (de == NULL) break;
            string file_name(de->d_name);

            if (file_name != "." && file_name != "..")
                result->push_back(path + file_name);
        }
        closedir(dp);
        std::sort(result->begin(), result->end());
    }
    return result;
}

vector<string>* read_castor_directory(string castor_dir, vector<string>* ret) {
    FILE *fp = popen(("/usr/bin/nsls " + castor_dir).c_str(), "r");
    char buff[500];

    if (fp != NULL)
        while (fgets(buff, sizeof buff, fp) != NULL) {
            std::string __File = "castor:" + castor_dir + buff;
            ret->push_back(__File.substr(0, __File.size() - 1));
        } else
        cout << "it's NULL!" << endl;

    pclose(fp);
    return ret;
}

vector<string>* read_srm_directory(string srm_dir, vector<string>* ret, string lcg_ls) {
  string command = lcg_ls + " \"" + srm_dir +  "\"";
  cout << command << endl;
    FILE *fp = popen(command.c_str(), "r");
    char buff[500];

    if (fp != NULL)
        while (fgets(buff, sizeof buff, fp) != NULL) {
            std::string __File = buff;
            ret->push_back(__File.substr(0, __File.size() - 1));
        } else
        cout << "it's NULL!" << endl;

    pclose(fp);
    return ret;
}

void FileInfo::KillFile() {
    if (info::TheInfo->Verbosity > 0) cout << "going to close the file" << endl;
    __File->Close();
    if (info::TheInfo->Verbosity > 0) cout << "going to delete the file" << endl;
    delete __File;
    if (info::TheInfo->Verbosity > 0) cout << "OK" << endl;
}
