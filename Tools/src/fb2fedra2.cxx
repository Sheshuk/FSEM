///This file contains methods to read FLUKA ascii output and make a FEDRA tree from them

#include "EdbPVGen.h"
#include "EdbPattern.h"
#include "FlReader2.h"
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
int vlevel=0;
int Nevents=1000;
int N0event=0;
///------------------------------------------------------------------
void fb2fedra(char* fname,char* outfname,bool binar=false,char* geofile="../brick.geo"){
    ///process FlukaSim file to tree with EdbPVGen
//     printf("load libs\n");
//     LoadLibs();
    printf("Processing %s \"%s\" to root \"%s\". Taking event from #%d to #%d\n",binar?"binary":"ascii",fname,outfname,N0event,Nevents);
    EdbPVGen* pvg=new EdbPVGen();
    printf("made PVG\n");
    EdbPatternsVolume* pv=new EdbPatternsVolume();
    pvg->SetVolume(pv);
    printf("setVolume\n");
    FlReader* reader=new FlReader();
    printf("made FlReader\n");
    reader->fTracks=new TObjArray();
    printf("made fTracks\n");
    reader->fVertices=new TObjArray();
    printf("made fVertices\n");
    pvg->eTracks=reader->fTracks;
    pvg->eVTX=reader->fVertices;
    printf("!!!\n");
    /// output tree:
    TFile outF(outfname,"RECREATE");
    TTree* tri=new TTree("FL_tree","FlukaSIM output tree");
//     tri->Branch("PVG","EdbPVGen",&pvg,128000,0);
    tri->Branch("PV","EdbPatternsVolume",&pv,128000,0);
//     tri->Branch("Trx","TObjArray",&(reader->fTracks));
    tri->Branch("Vtx","TObjArray",&(reader->fVertices));
    tri->Write("FL_tree",TObject::kOverwrite);
    ///fill patterns
    printf("fill patterns\n");
    reader->fVerbose=vlevel;
    reader->fVolume=pvg->GetVolume();
    reader->FillPatterns(geofile,true);
    FILE* ff = 0;
    ifstream* is= 0;
    if(binar)is=new ifstream(fname);
    else ff = fopen(fname,"r");

    bool okay=true;
    printf("reading 0\n");
    if(binar)reader->GotoEventB(is,N0event);
    else reader->GotoEvent(ff,N0event);
    printf("done\n");
    if(!reader->fEvt)printf("no evt read!\n");
    reader->fEvt->Dump();
    int EvtNum=reader->fEvt->n;
    while(okay){
        EvtNum++;
	printf("reading event #%d\n",EvtNum);
//         if(EvtNum%50==0||(vlevel>0))printf("reading event #%d\n",EvtNum);
	fflush(stdout);
//         for(int k=0;k<pvg->eVTX->GetEntries();k++){
//             ((EdbVertex*)pvg->eVTX->At(k))->Print();
//         };
        if(vlevel>=2)printf("resetting pvg\n");
        ResetPV(pvg->GetVolume());
        if(vlevel>=2)printf("resetting vtx\n");
        if(pvg->eVTX)pvg->eVTX->Delete();
        if(vlevel>=2)printf("resetting trx\n");
        if(pvg->eTracks)pvg->eTracks->Delete();
        if(vlevel>=2)printf("now process event...\n");
        if(binar)okay=reader->ProcessEventB(is);
        else okay=reader->ProcessEvent(ff);
        if(vlevel>0)printf("pvg has %d track and %d vertices\n", pvg->eTracks->GetEntries(),pvg->eVTX->GetEntries());
        tri->Fill();
        if(EvtNum<Nevents)continue;
//         pvg->SmearSegments();
        if((Nevents!=0)&&(EvtNum>=Nevents))break;
    }
    tri->Write("FL_tree",TObject::kOverwrite);
    if(binar)is->close();
    else fclose(ff);
    outF.Close();
}
///------------------------------------------------------------------
void helpme(){
    printf("------------------\n");
    printf("fb2fedra by ASh\n");
    printf("Usage: fb2fedra [-v=Vlevel -N=Nev] 'inputfile.out' 'outputfile.root'\n");
    printf("---------------------------------------------------------\n");
    printf("   -v=Vlevel  - set verbosity level (0 - quiet, 3 - full)\n");
    printf("   -n=Nev     - set number of events to process\n");
    printf("                Note: to process single event #M run with -n=-M \n");
    printf("                Note: to process all events run with -n=0 \n");
    printf("   -a         - process ascii file \n");
    printf("   -b         - process binary file(default)\n");
    printf("---------------------------------------------------------\n");
    printf(" Enjoy!\n");
}

int main(int argc,char** argv){
    if(argc<3){printf("need at least 2 arguments:\n");helpme();return 0;}
    char* infle=0;
    char* outfle=0;
    bool binary=true;
    for(int i=1;i!=argc;i++){
        if(!strncmp(argv[i],"-v=XX",3)) {sscanf(argv[i],"-v=%d",&vlevel ); continue;}
        if(!strncmp(argv[i],"-n=XX",3)) {sscanf(argv[i],"-n=%d",&Nevents); continue;}
        if(!strncmp(argv[i],"-n0=XX",4)) {sscanf(argv[i],"-n0=%d",&N0event); continue;}
        if(!strcmp (argv[i],"-a")) {binary=false;continue;}
        if(!strcmp (argv[i],"-b")) {binary=true; continue;}
        if(!infle) {printf("Set INPUT file:  \"%s\"\n",argv[i]); infle =argv[i]; continue;}
        if(!outfle){printf("Set OUTPUT file: \"%s\"\n",argv[i]); outfle=argv[i]; continue;}
        printf("ERROR! UNKNOWN OPTION \"%s\"! Abort\n",argv[i]);
	helpme();
	return -1;
    }
    fb2fedra(infle,outfle,binary);
    return 0;
}
///------------------------------------------------------------------
