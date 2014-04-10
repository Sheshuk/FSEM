
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "FlData.h"
#include "FlReaderApp.h"

long FlReaderApp::ReadLong(char* str){
  char* endp;
  long n=strtol(str,&endp,10);
  if(endp[0]=='k')n*=1000;
  if(endp[0]=='m')n*=1000000;
  if(endp[0]=='g')n*=1000000000;
  return n;
}

void FlReaderApp::HelpMe(){
  printf(" ** Fluka output reader application **\n");
  printf(" Usage:  <appname> [options] infile outfile\n");
  printf("options:\n");
  printf("\t-h \t - print this help\n");
  printf("\t-vNN \t - set verbosity (debug output) level to NN\n");
  printf("\t-N Number \t - first event to process\n");
  printf("\t-n Number \t - number of events to process\n");
  printf("\t-M Number \t - merge M events together\n");
  printf("example:\n <appname> -v3 -N 10k -n25k -M 5k FluSim.bin Output.txt\n");
}

void FlReaderApp::ReadArgs(int argc, char** argv){
  if(argc<=1){HelpMe(); Exit();}
  char c;
  while ((c = getopt (argc, argv, "han:N:M:g:v::")) != -1)
   switch (c)
     {
      case 'h': HelpMe(); Exit(); return;
      case 'a': fAppend=true; break;
      case 'v': fVerbose=optarg? atoi(optarg):1; break;
      case 'n': fNtotal=ReadLong(optarg); break;
      case 'N': fNstart=ReadLong(optarg); break;
      case 'M': fMerge =ReadLong(optarg); break;
      case 'g': fGeoFile=optarg; break;
      case '?': Exit();
     }
     if(optind<argc) fInFile=argv[optind]; optind++;
     if(optind<argc)fOutFile=argv[optind]; optind++;
     printf("InFile= %s, OutFile= %s (%s)\n",fInFile,fOutFile,fAppend?"append":"owerwrite");
     printf("Process events [%ld - %ld] merging %ld events together. Verbose=%d\n",fNstart,fNstart+fNtotal-1,fMerge,fVerbose);
  }

//------------------------------------------------------
void FlReaderApp::Init(){
  FlRead::FlVerbose=fVerbose;
  fReader.SetSaveBits("vtmb");
  if(fReader.Open(fInFile)==0){
    fprintf(stderr,"ERROR opening file %s\n",fInFile);
    throw fInFile;
  }
}
//------------------------------------------------------
void FlReaderApp::Run(){
  Start();
  Init();
  if(fRunning==false)Finish();
  unsigned long Nev=0;
  if(fNstart<1)fNstart=1;
  if(fReader.FindEvent(fNstart)==false) throw "Event #Nstart not found";
  //---------  main loop: ----------------
  while(IsRunning()){
    if(fReader.ReadEvent()){
    // if(fReader.Event()>=fNstart){
      Nev++;
      if(Nev==fMerge){
        ProcessEvent();
        fReader.ClearAll();
        Nev=0;
      }
    if(fReader.Event()%10000==0){printf("Evt #%d\n",fReader.Event());fflush(stdout);}
    if(fReader.Event()>=fNstart+fNtotal-1)Stop(); 
    }
    else Stop();
  }
  Finish();
}
//------------------------------------------------------
void FlReaderApp::Start(){
  fRunning=true;
}

//------------------------------------------------------
void FlReaderApp::Stop(){
  fRunning=false;
  printf("+++ Stop! +++\n");
}

//------------------------------------------------------
void FlReaderApp::Finish(){
 fReader.Close();
}
