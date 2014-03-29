#include "FlData.h"
#include "FlReaderApp.h"

class FlFedraRaw: public FlReaderApp{
    virtual void Init(){printf("INit!\n"); FlReaderApp::Init();}
    virtual void Finish(){printf("Finish!\n"); FlReaderApp::Finish();}
    virtual void ProcessEvent(){FlVtx::PrintAll();}
    //printing
    virtual void HelpMe(){printf("This is Fluka2Fedra Raw!\n");FlReaderApp::HelpMe();}
    // virtual void ReadArgs(int argc, char** argv);
};

int main(int argc, char **argv)
{
  FlFedraRaw app;
  app.ReadArgs(argc,argv);
  app.Run();
  return 0;
}