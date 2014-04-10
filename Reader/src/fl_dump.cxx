
#include "FlData.h"
#include "FlReaderApp.h"

class FlDumpApp: public FlReaderApp{
    virtual void Init(){
      FlReaderApp::Init();
      FlRead::outfile=(fOutFile==0)?stdout:fopen(fOutFile,"w");
    }
    virtual void Finish(){
      FlReaderApp::Finish();
      fclose(FlRead::outfile);
      FlRead::outfile=stdout;
    }
    virtual void ProcessEvent(){
      fReader.PrintEvt();
      fReader.PrintStat();
      FlVtx::PrintAll();
    }
    //printing
    virtual void HelpMe(){printf("This is FlukaDump application\n");FlReaderApp::HelpMe();}
};

int main(int argc, char **argv)
{
  FlDumpApp app;
  app.ReadArgs(argc,argv);

  app.Run();
  return 0;
}

