#include <cstdlib>
#include "FlData.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TVector3.h"

using namespace std;

FlRead reader;
TTree* outtree=0;
TFile* outfile=0;
bool full=false;
long Ntot=-1;
struct{
  int Ne, flg, Nc;
  int code0,code1;
  float Vz;
  float p0,p1;
  float kink, w;
}fScat;

long NSeg[3]={0,0,0};

void InitTree(char* outfname){
  outfile=new TFile(outfname,"UPDATE");
  outtree=(TTree*)outfile->Get("MuScat");
  if(outtree!=0){
    printf("Outtree read with %d events already. I'll append it\n",outtree->GetEntries());
    outtree->SetBranchAddress("scat",&fScat);
  }
  else{
    outfile->cd();
    outtree=new TTree("MuScat","Tree with muon scattering events");
    outtree->Branch("scat",&fScat,"Nev/I:flag:Nc:code0:code1:Vz/F:P0:P1:Kink:W");
  }
}

void CloseTree(){
  outfile->cd();
  outtree->Write("MuScat",TObject::kOverwrite);
  outfile->Close();
}

TH1F* hspec=0;

void ReadSpec(char* fname){
  TFile *f=new TFile(fname,"READ");
  f->ls();
  hspec=(TH1F*)f->Get("htemp");
  
//  f->Close();
  printf("Spectrum histogram has %f entries\n",hspec->Integral());
}

void FillTree(){
  std::map<long,FlSeg*>::const_iterator it;
  for(it=FlSeg::Map.begin();it!=FlSeg::Map.end();++it){
    FlSeg* seg=it->second;
    if(seg->p<=1)continue;
    if(seg->pdg!=13)continue;
    NSeg[seg->side]++;
    if(!full && seg->kink<1e-2)continue;
    fScat.Ne=reader.fEvent;
    fScat.p0=seg->p;
    fScat.p1=seg->p-seg->eloss;
    fScat.flg=seg->side;
    fScat.kink=seg->kink*1e3;
    fScat.Vz=seg->z;
    fScat.code0=seg->trk_id;
    fScat.code1=seg->pdg;
    if(hspec){
      int nbin=hspec->FindBin(fScat.p0);
      fScat.w=14.0*hspec->GetBinContent(nbin)/(hspec->GetBinWidth(nbin)*hspec->GetEntries());
//      printf("W(%2.2f)=%2.4f\n",fScat.p0,fScat.w);
    }
    else fScat.w=-999;
    outtree->Fill();
  }
}

int main(int argc, char** argv){
  FlRead::FlVerbose=0;  
  bool test=false;
  for(int n=1; n<argc-2; ++n){
    if(strncmp(argv[n],"-v=",  3)==0){FlRead::FlVerbose=atoi(argv[n]+3); continue;}
    if(strncmp(argv[n],"-n=",  3)==0){
      char* endp;
      Ntot=strtol(argv[n]+3,&endp,10);
      printf("Ntot base=%ld\n",Ntot);
      if(endp[0]=='k')Ntot*=1e3;
      if(endp[0]=='m')Ntot*=1e6;
      if(endp[0]=='g')Ntot*=1e9;
      printf("Ntot=%ld\n",Ntot);
      continue;
      }
    if(strncmp(argv[n],"-full",5)==0){full=true; continue;}
  }
  reader.fDoSaveSeg[FlSeg::kBTK]=1;
  reader.fDoSaveSeg[FlSeg::kMTK]=1;
  reader.fDoSaveSeg[FlSeg::kPB]=1;
  reader.fDoSaveTrx=1;
  reader.fDoSaveVtx=0;
  ReadSpec("MuSpec.h.root");
  printf("Verbose=%d\n",FlRead::FlVerbose);
  InitTree(argv[argc-1]);
  if(reader.Open(argv[argc-2])==0){
    printf("ERROR opening file %s\n",argv[argc-2]);
    return 1;
  }
  while(reader.ReadEvent()){
   if(reader.fEvent%10000==0){
    printf("Evt #%d. Tree has %ld entries\n",reader.fEvent, outtree->GetEntriesFast());
    printf("Read muonic segments:\n%ld in base\n%ld in emulsion\n%ld in lead\n",NSeg[0],NSeg[1],NSeg[2]);
    }
    if(reader.fEvent>Ntot)break;
//     printf("----\n");
//     reader.PrintStat();
    //FlSeg::PrintAll();
    FillTree();
    reader.ClearAll();
    fflush(stdout);
  }
  printf("Read %d events with segments:\n%ld in base\n%ld in emulsion\n%ld in lead\n",reader.fEvent,NSeg[0],NSeg[1],NSeg[2]);
  CloseTree();
  reader.Close();
  return 0;
}
