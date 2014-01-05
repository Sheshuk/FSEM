///This file contains methods to read FLUKA ascii output and make a FEDRA tree from them

#include "EdbPVGen.h"
#include "EdbPattern.h"
#include "FlReader.h"
#include "EdbDataStore.h"
#include "TFile.h"


///-------------------------------------------------------------------------------
void ResetPV(EdbPatternsVolume* pv){
    EdbPattern* pat;
    for(int i=0;i<pv->Npatterns();i++){
        pat=pv->GetPattern(i);
        pat->GetSegments()->Clear("C");
    }
};
///------------------------------------------------------------------
void DumpPatterns(EdbPatternsVolume* pv){
  EdbPattern* p=0;
  int npat=pv->Npatterns();
  for(int n=0;n<npat;n++){
    p=pv->GetPattern(n);
    printf("pat %d/%d:  z=%2.2f plt=%d side=%d\n",n,npat,p->Z(),p->Plate(),p->Side());
  }
}
///------------------------------------------------------------------
int vlevel=0;
int Nevents=1000;
int N0event=0;
int Nev_Mrg=1;
char Geofile[20]="../brick.geo";
int Format_Mode=0;
bool do_save_raw;
///------------------------------------------------------------------
void fb2fedra(char* fname,char* outfname){
    ///process FlukaSim file to tree with EdbDataStore
//     printf("load libs\n");
//     LoadLibs();
    printf("Processing \"%s\" to root \"%s\". Taking event from #%d to #%d\n",fname,outfname,N0event,Nevents);
    printf("setVolume\n");
    FlReader* reader=new FlReader();
    reader->fDoMakeRaw=do_save_raw;
    printf("made FlReader\n");
    reader->fBrick=new EdbBrickP;
    reader->fTracks=new TObjArray;
    reader->fVertices=new TObjArray;
    reader->fPVRaw=new EdbPatternsVolume;
    reader->fPVSeg=new EdbPatternsVolume;    
    ///fill patterns
    printf("fill patterns\n");
    reader->fVerbose=vlevel;
    reader->FillPatterns(Geofile);
    DumpPatterns(reader->fPVSeg);
    TFile outF(outfname,"RECREATE");
    outF.cd();
    reader->fBrick->Write("Brick",TObject::kOverwrite);

    if(Nevents==0){    
      outF.Close();
      return;
      }
    /// output tree:
    
    TTree* tri=new TTree("FluSim","FlukaSIM output fmt#0");
    tri->Branch("Vtx","TObjArray",&reader->fVertices,128000,0);
    tri->Write("FluSim",TObject::kOverwrite);
    
    ifstream* is= 0;
    is=new ifstream(fname);

    bool okay=true;
    printf("reading 0\n");
    reader->GotoEventB(is,N0event);
    printf("done\n");
    if(!reader->fEvt)printf("no evt read!\n");
    reader->fEvt->Dump();
    int EvtNum=reader->fEvt->n;
    printf("Merge %d events\n",Nev_Mrg);
    while(okay){
        EvtNum++;
        if(EvtNum%100==0||(vlevel>0))printf("reading event #%d\n",EvtNum);
        if(vlevel>=2)printf("resetting...\n");
        okay=reader->ProcessEventB(is);
	if(vlevel>0){
	  printf("%s okay\n",okay?"":"NOT");
	  int Nsegs=0;
	  for(int np=0;np<reader->fPVSeg->Npatterns();np++)
	    Nsegs+=reader->fPVSeg->GetPattern(np)->N();
	  printf("pvg has %d segments in %d patterns, %d tracks and %d vertices\n",
		 Nsegs,reader->fPVSeg->Npatterns(),reader->fTracks->GetEntries(),reader->fVertices->GetEntries());
	}
	if(EvtNum%Nev_Mrg==0){
	  reader->DumpEvt(vlevel,1);
	  tri->Fill();
	  reader->fVertices->Clear();
	  reader->fTracks->Clear();
	  reader->ClearPV();
	}
// 	else{DS.ClearTracks(0); DS.ClearVTX();}
// 	if(vlevel>=2)DS.PrintBrief();
        if(EvtNum<Nevents)continue;
//         pvg->SmearSegments();
        if((Nevents!=0)&&(EvtNum>=Nevents))break;
    }
    if(EvtNum % Nev_Mrg !=0)tri->Fill();
    is->close();
    tri->Write("FluSim",TObject::kOverwrite);
    outF.Close();
}

///------------------------------------------------------------------
void helpme(){
    printf("------------------\n");
    printf("fb2fedra by ASh\n");
    printf("Usage: fb2fedra [-v=Vlevel -N=Nev -N0=Nev0 --onev -g=geofile] 'inputfile.out' 'outputfile.root'\n");
    printf("---------------------------------------------------------\n");
    printf("   -v=Vlevel  - set verbosity level (0 - quiet, 3 - full)\n");
    printf("   -n=Nev     - set number of events to process\n");
    printf("                Note: to process single event #M run with -n=-M \n");
    printf("                Note: to process all events run with -n=0 \n");
    printf("   -n0=Nev0   - set number of starting event\n");
    printf("   -m=N       - store all N events as ONE event (def: 1)\n");
    printf("   -g=geofile - read geometry from file \"geofile.geo\" (def: brick.geo)\n");
    printf("   -f=MODE    - set output format mode (0,1):\n");
    printf("                MODE=0 - save EdbDataStore,\n");
    printf("                MODE=1- save Vertices and EdbPatternsVolume\n");
    printf("---------------------------------------------------------\n");
    printf(" Enjoy!\n");
}

int main(int argc,char** argv){
    if(argc<3){printf("need at least 2 arguments:\n");helpme();return 0;}
    char* infle=0;
    char* outfle=0;
    for(int i=1;i!=argc;i++){
      printf("argv[%d]=\'%s\'\n",i,argv[i]);
        if(!strncmp(argv[i],"-v=XX",3)) {sscanf(argv[i],"-v=%d",&vlevel ); continue;}
        if(!strncmp(argv[i],"-n=XX",3)) {sscanf(argv[i],"-n=%d",&Nevents); continue;}
        if(!strncmp(argv[i],"-n0=XX",4)) {sscanf(argv[i],"-n0=%d",&N0event); continue;}
        if(!strncmp(argv[i],"-m=XX",3)) {sscanf(argv[i],"-m=%d",&Nev_Mrg); continue;}
        if(!strncmp(argv[i],"-g=XX",3)) {sscanf(argv[i],"-g=%s",Geofile); continue;}
        if(!strncmp(argv[i],"-raw",4)) {do_save_raw=true; continue;}
        if(!infle) {printf("Set INPUT file:  \"%s\"\n",argv[i]); infle =argv[i]; continue;}
        if(!outfle){printf("Set OUTPUT file: \"%s\"\n",argv[i]); outfle=argv[i]; continue;}
        printf("ERROR! UNKNOWN OPTION \"%s\"! Abort\n",argv[i]);
	helpme();
	return -1;
    }
    printf("geometry from file %s\n",Geofile);
    fb2fedra(infle,outfle);
    return 0;
}
///------------------------------------------------------------------
