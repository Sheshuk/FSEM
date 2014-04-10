#include <TFile.h>
#include <TTree.h>
#include "FlData.h"
#include "FlReaderApp.h"

//------------------------------------------------------
class FlToSimple: public FlReaderApp{
    virtual void Init();
    virtual void Finish();
    virtual void ProcessEvent();
    //printing
    virtual void HelpMe(){printf("This is Fluka2Fedra Raw!\n");FlReaderApp::HelpMe();}
    // virtual void ReadArgs(int argc, char** argv);
  private:
    TFile* OutFile=0;
    TTree* OutTree=0;
    FlSeg OutSeg;
    struct {
        unsigned Num;
        float    Energy;
        int Pdg;
    } Evt;
};
//------------------------------------------------------
void FlToSimple::Init(){
    FlReaderApp::Init();
    fReader.SetSaveBits("mblvt");
    OutFile=new TFile(fOutFile,fAppend?"UPDATE":"RECREATE");
    OutFile->cd();
    if(fAppend){
      OutTree=(TTree*)OutFile->Get("Fluka_sim");
      OutTree->SetBranchAddress("seg",&OutSeg);
      OutTree->SetBranchAddress("evt",&Evt);
    }else{
      OutTree=new TTree("Fluka_sim","Fluka_sim");
      OutTree->Branch("seg",&OutSeg,"id/L:pdg/I:x/f:y:z:dz:P:tx:ty:kink:eloss:trk_id/I:plate/B:side");
      OutTree->Branch("evt",&Evt,"num/i:E/F:Pdg/I");
    }
    OutTree->Write("Fluka_sim",TObject::kOverwrite);
  }
//------------------------------------------------------
void FlToSimple::Finish(){
    FlReaderApp::Finish();
    OutTree->Write("Fluka_sim",TObject::kOverwrite);
    OutFile->Close();
  }
//------------------------------------------------------
void FlToSimple::ProcessEvent(){
    Evt.Num=fReader.Event();
    Evt.Energy=fReader.Energy();
    Evt.Pdg=fReader.PDG();

    for(auto&& itr : FlSeg::Map) {
      OutSeg=*itr.second;
      OutTree->Fill();
    }
    // fReader.PrintStat();
  }
//------------------------------------------------------
int main(int argc, char **argv)
{
  FlToSimple app;
  app.ReadArgs(argc,argv);
  app.Run();
  return 0;
}