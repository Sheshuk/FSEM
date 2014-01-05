#include <TFile.h>
#include <TTree.h>
#include <EdbDataStore.h>
TFile* gInFile=0;
TFile* gOutFile=0;
TTree* gInTree=0;
TTree* gOutTree=0;
int curtrk=-1;
struct{
  int Nev,id,trk_id,pdg;
  float x,y,z,tx,ty;
  float p,w;
  void set(EdbSegP* s,EdbDataStore* ds=0){
    id=s->ID();
    Nev=s->MCEvt();
    trk_id=s->MCTrack();
    x=s->X();  y=s->Y(); z=s->Z();
    tx=s->TX(); ty=s->TY();
    p=s->P();
    w=s->W();
//     if(trk_id==0)trk_id=1;
//     printf("there are %d tracks in DS\n",ds->Nt());
    if(ds){EdbTrackP* trk=ds->FindTrack(trk_id);
     pdg=trk->PDG();
    }
  }
  void Dump(){
    printf("ev#%d id=%d trk=%d [%2.2f %2.2f %2.2f] %2.4f %2.4f P=%2.2f\n",Nev,id,trk_id,x,y,z,tx,ty,p);
  }
} gSeg;


void CopyArr(TObjArray* a1, TObjArray* a2){
  for(int i=0;i<a1->GetEntries();++i)
    a2->Add(a1->At(i));
}

int main(int argc, char** argv){
//   EdbPattern* plt=0;
  EdbSegP* seg=0;
  EdbVertex *v=0;
  EdbTrackP *t=0;
  gInFile =TFile::Open(argv[1],"READ");
  gOutFile=TFile::Open(argv[2],"RECREATE");
  gOutFile->cd();
  printf("begin! %s -> %s\n",argv[1],argv[2]);
  TObjArray *vts=new TObjArray;
  gOutTree=new TTree("Fluka_sim","Fluka_sim");
  gOutTree->Branch("seg",&gSeg,"Nev/I:id:trk_id:pdg:x/f:y:z:tx:ty:P:W");
  gOutTree->Write("Fluka_sim",TObject::kOverwrite);
  gInTree=(TTree*)gInFile->Get("FluSim");
  gInTree->GetBranch("Vtx")->SetAddress(&vts);
  for(int n=0;n<gInTree->GetEntries();++n){
    if(n%100==0)
    printf("ev#%d %2.4f%%\n",n,(n*100./gInTree->GetEntries()));
    fflush(stdout);
    gInTree->GetEntry(n);
//     printf("Nv=%d\n",vts->GetEntries());
    for(int nv=0;nv<vts->GetEntries();++nv){
      v=(EdbVertex*)vts->At(nv);
//       printf(" * v#%d - %d tracks\n",nv,v->N());
      for(int nt=0;nt<v->N();++nt){
	t=v->GetTrack(nt);
	if(t->N()==0)continue;
// 	printf(" * * t#%d - %d segments\n",nt,t->N());
	for(int ns=0;ns<t->N();++ns){
	        seg=t->GetSegment(ns);
	        if(seg->P()<0.01)continue;
		gSeg.set(seg);
		gSeg.pdg=t->PDG();
		if(gSeg.trk_id!=curtrk)
	    	gOutTree->Fill();
	}
      }
    }
//     DS.SetOwnVertices(1);
//     DS.SetOwnTracks(1);
//     DS.Clear();
//     printf("done, now CLEAR\n");
    vts->SetOwner();
    vts->Clear("C");

    
  }
  gInFile->Close();
  gOutTree->Write("Fluka_sim",TObject::kOverwrite);
  gOutFile->Close();
}