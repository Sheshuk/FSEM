#include <cstdlib>
#include "FlData.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TVector3.h"

FlRead reader;

long ReadLong(char* str){
  char* endp;
  long n=strtol(str,&endp,10);
  if(endp[0]=='k')n*=1000;
  if(endp[0]=='m')n*=1000000;
  if(endp[0]=='g')n*=1000000000;
  return n;
}

long N0=1;
long Ntot=-1;
long M=1;

void helpme(){
  printf("fl_dump: Dump Fluka output to command line. A tool for reading FSEM FluSim.bin files\n");
  printf(" Usage:  ./fl_dump FluSim.bin -v=VLEV -n=Ntot -n0=N0 -m=M\n");
  printf("arguments:\n");
  printf(" >      VLEV - verbose output level (debug)\n");
  printf(" >      Ntot - number of events to process\n");
  printf(" >      N0 - first event to process\n");
  printf(" >      M - merge M events together\n");
}

int main(int argc, char** argv){
  FlRead::FlVerbose=0;  
  bool test=false;
  if(argc<=1){helpme(); return 0;}

  for(int n=1; n<argc-1; ++n){
    if(strncmp(argv[n],"-h",   2)==0){helpme(); return 0; }
    if(strncmp(argv[n],"-v=",  3)==0){FlRead::FlVerbose=atoi(argv[n]+3); continue;}
    if(strncmp(argv[n],"-n=",  3)==0){
      Ntot=ReadLong(argv[n]+3);
      printf("Ntot=%ld\n",Ntot);
      continue;
      }
    if(strncmp(argv[n],"-n0=",  4)==0){
      N0=ReadLong(argv[n]+4);
      printf("N0=%ld\n",N0);
      continue;
      }
    if(strncmp(argv[n],"-m=",  3)==0){
      M=ReadLong(argv[n]+3);
      printf("M=%ld\n",M);
      continue;
      }
  }
  /*reader.fDoSaveSeg[FlSeg::kBTK]=0;
  reader.fDoSaveSeg[FlSeg::kMTK]=0;
  reader.fDoSaveSeg[FlSeg::kPB]=0;
  reader.fDoSaveTrx=0;
  reader.fDoSaveVtx=0;*/
  
  printf("Verbose=%d\n",FlRead::FlVerbose);
  if(reader.Open(argv[argc-1])==0){
    printf("ERROR opening file %s\n",argv[argc-1]);
    return 1;
  }
  long Nev=0;
//   reader.SetSaveBits(63);
  if(reader.FindEvent(N0)==false)return 1;
  while(reader.ReadEvent()){
    if(reader.fEvent>=N0){
      Nev++;
      reader.PrintEvt();
      fflush(stdout);
      /*fDoSaveSeg[FlSeg::kBTK]=1;
      reader.fDoSaveSeg[FlSeg::kMTK]=1;
      reader.fDoSaveSeg[FlSeg::kPB]=1;
      reader.fDoSaveTrx=1;
      reader.fDoSaveVtx=1;*/
      if(Nev==M){
        FlVtx::PrintAll(2);
        reader.ClearAll();
	      Nev=0;
      }
    }
    if(reader.fEvent%10000==0){printf("Evt #%d\n",reader.fEvent);      fflush(stdout);}
    if(reader.fEvent>N0+Ntot)break; 
  }
  reader.Close();
  return 0;
}
