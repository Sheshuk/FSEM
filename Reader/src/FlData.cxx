#include <cassert>
#include <cmath>
#include "FlData.h"

long FlSeg::IDSEG=0;
 int FlTrk::IDTRK=0;
 int FlVtx::IDVTX=0;
std::map <long,FlSeg*> FlSeg::Map;
std::map <int, FlTrk*> FlTrk::Map;
std::map <int, FlVtx*> FlVtx::Map;

int FlRead::FlVerbose;

#define LOG(vlev) if(vlev<=FlRead::FlVerbose)printf("\033[1;32m |%s <%s>\033[0m\t",std::string(vlev*2, '-').data(),__PRETTY_FUNCTION__);if(vlev<=FlRead::FlVerbose)
#define Log(vlev,...) if(vlev<=FlRead::FlVerbose){printf("\033[1;32m |%s <%s>\033[0m\t",std::string(vlev*2, '-').data(),__PRETTY_FUNCTION__);printf(__VA_ARGS__);}
#define Err(vlev,...) if(vlev<=FlRead::FlVerbose){fprintf(stderr,"\033[1;31m<%s>\033[0m\t",__PRETTY_FUNCTION__);fprintf(stderr,__VA_ARGS__);}
#define ERR(vlev) if(vlev<=FlRead::FlVerbose)fprintf(stderr,"\033[1;31m<%s>\033[0m\t",__PRETTY_FUNCTION__);if(vlev<=FlRead::FlVerbose)

int CalcSegmSide(record *r0,record *r1){
  LOG(5)r0->print();
  LOG(5)r1->print();
  if(r0->desc!=kEM0 && r0->desc!=kEM1)return -1;
  if(r1->desc!=kEM0 && r1->desc!=kEM1)return -1;
  if(r0->desc==r1->desc)return -1;
  if(r0->pid!=r1->pid)  return -1;
  if(r0->num!=r1->num) return -1;
  if(r0->z==r1->z) return -1;
  if(std::abs(r0->par-r1->par)>1)return -1;
  if(r0->desc==kEM1 && r1->desc==kEM0)return FlSeg::kMTK;
  if(r0->desc==kEM0 && r1->desc==kEM1)
     return (r0->par!=r1->par)?(FlSeg::kPB):(FlSeg::kBTK);
  return -1;
}
int FLUKA2PDG(int code){
  //      To convert FLUKA particle code to PDG code
  if(code<-7)code=-7;
  switch(code) {
    case -7: return 9997;//1000020050;
    case -6: return 9996;//1000020040;
    case -5: return 9995;//1000020030;
    case -4: return 9994;//1000010030;
    case -3: return 9993;//1000010020;
    case -2: return 9992;//1000000000;
    case -1: return 9991;//50000050;
    case 0: return 0;
    case 1: return 2212;
    case 2: return -2212;
    case 3: return 11;
    case 4: return -11;
    case 5: return 12;
    case 6: return -12;
    case 7: return 22;
    case 8: return 2112;
    case 9: return -2112;
    case 10: return -13;
    case 11: return 13;
    case 12: return 130;
    case 13: return 211;
    case 14: return -211;
    case 15: return 321;
    case 16: return -321;
    case 17: return 3122;
    case 18: return -3122;
    case 19: return 310;
    case 20: return 3112;
    case 21: return 3222;
    case 22: return 3212;
    case 23: return 111;
    case 24: return 311;
    case 25: return -311;
    case 26: return 0;
    case 27: return 14;
    case 28: return -14;
    case 29: return 0;
    case 30: return 0;
    case 31: return -3222;
    case 32: return -3212;
    case 33: return -3112;
    case 34: return 3322;
    case 35: return -3322;
    case 36: return 3312;
    case 37: return -3312;
    case 38: return 3334;
    case 39: return -3334;
    case 40: return 2112;
    case 41: return -15;
    case 42: return 15;
    case 43: return 16;
    case 44: return -16;
    case 45: return 411;
    case 46: return -411;
    case 47: return 421;
    case 48: return -421;
    case 49: return 431;
    case 50: return -431;
    case 51: return 4122;
    case 52: return 4232;
    case 53: return 4132;
    case 54: return 4322;
    case 55: return 4312;
    case 56: return 4332;
    case 57: return -4122;
    case 58: return -4232;
    case 59: return -4132;
    case 60: return -4322;
    case 61: return -4312;
    case 62: return -4332;
    case 63: return -1;
    default  : {
      printf("What's #%i in FLUKA?\n",code);
      return -1;
    }
  }
};

//#define Log(vlev) if(vlev<=FlRead::FlVerbose)
///----------------------------------------------------------------
FlSeg::FlSeg(FlTrk* t,record r1, record r2, int Side):id(++IDSEG),side(Side){
//  assert(t!=NULL);
  assert(r1.desc==kEM0||r1.desc==kEM1);
  assert(r2.desc==kEM0||r2.desc==kEM1);
  assert(r1.desc!=r2.desc);
  ///register in map
  Map[id]=this; 
  plate=r1.par;
  
  if(r2.p>r1.p){record rt=r2; r2=r1; r1=rt;}
  double r1m =r1.tx*r1.tx+r1.ty*r1.ty+1.0;
  double r2m =r2.tx*r2.tx+r2.ty*r2.ty+1.0;
  double r1r2=r1.tx*r2.tx+r1.ty*r2.ty+1.0;
  kink=sqrt(1-r1r2*r1r2/(r1m*r2m));
  dz=r2.z-r1.z;
  tx=(r2.x-r1.x)/dz;
  ty=(r2.y-r1.y)/dz;
  x=0.5*(r1.x+r2.x);
  y=0.5*(r1.y+r2.y);
  z=0.5*(r1.z+r2.z);
  pdg=FLUKA2PDG(r1.pid);
  p=r1.p; 
  eloss=r1.p-r2.p;
  
  trk_id=r1.num;
  trk=t;
  Log(3,"Created a segment:\n");
  LOG(3)Print(0);
};
///----------------------------------------------------------------
FlSeg::~FlSeg(){
  Log(5,"I am being deleted! My id is %ld\n",id);
  Clear(); 
  if(Map[id]==this)
    Map.erase(id);
}
///----------------------------------------------------------------
void FlSeg::Clear(){trk=0;};
///----------------------------------------------------------------
void FlSeg::Print(int lev){
  printf("seg#%3ld pdg=%5d trk#%4d; Plt#%d side_%d, dz=%4.1f\t",id,pdg,trk_id,plate,side,dz);
  printf("pos=%6.1f %6.1f %6.1f [%5.4f %5.4f] P=%4.4f k=%g\n",x,y,z,tx,ty,p,kink*1e3);
};
///----------------------------------------------------------------
void FlSeg::PrintAll(int lev){
  printf("--------------%3ld Segments-----------------------\n",FlSeg::Map.size());
  std::map<long, FlSeg*>::const_iterator itr;
  for(itr=Map.begin();itr!=Map.end();++itr){
    printf("SEG [%3ld]: ",itr->first);
    if(itr->second)itr->second->Print(lev);
    else printf(" NULL\n");
  }
};
///----------------------------------------------------------------
///----------------------------------------------------------------
///----------------------------------------------------------------
FlTrk::FlTrk(FlVtx* v,record r):id(r.num),x(r.x),y(r.y),z(r.z),p(r.p),tx(r.tx),ty(r.ty),vtx0(v),vtx1(NULL){
  assert(r.desc==kOUT);
//   Log(2,"Adding trk with id=%d (0x%x)\n",id,this);
  Log(2,"Created a track:\n");
  LOG(2)Print(0);
  pdg=FLUKA2PDG(r.pid);
  Map[id]=this;
  fLastRec.desc=-1;
};
///----------------------------------------------------------------
FlTrk::~FlTrk(){
  Clear(); 
  if(Map[id]==this)
    Map.erase(id);
}
///----------------------------------------------------------------
void FlTrk::PrintAll(int lev){
  printf("--------------%3ld TRACKS-----------------------\n",FlTrk::Map.size());
  std::map<int, FlTrk*>::const_iterator itr;
  for(itr=Map.begin();itr!=Map.end();++itr){
    printf("TRK [%3d]: ",itr->first);
    if(itr->second)itr->second->Print(lev);
    else printf(" NULL\n");
  }
};
///----------------------------------------------------------------

void FlTrk::Clear(){
    vtx0=0; 
    vtx1=0; 
    segs.clear();
    Log(4,"Trk#%d: Clear%d segs\n",id,N());
};
///----------------------------------------------------------------
void FlTrk::Print(int lev){
  printf("trk#%3d pdg=%5d Ns=%ld\t",id,pdg,segs.size());
  printf("pos=[%6.1f %6.1f %6.1f] tt=[%5.4f %5.4f] P=%4.4f\n",x,y,z,tx,ty,p);
  if(lev==0)return;
  --lev;
  for(unsigned long n=0;n<segs.size();++n){
    printf("-- %3ld)",n);segs[n]->Print(lev);
  }  
};
///----------------------------------------------------------------
///----------------------------------------------------------------
///----------------------------------------------------------------
FlVtx::FlVtx(FlTrk* t,record r):id(IDVTX++),x(r.x),y(r.y),z(r.z),tx(r.tx),ty(r.ty),p(r.p),trk_in(t){
  assert(r.desc==kIN || r.desc==kEVT);
  flag=r.par;
  if(t!=NULL)t->vtx1=this;
  Map[id]=this; 
};
///----------------------------------------------------------------
FlVtx::~FlVtx(){
  Clear();
  if(Map[id]==this)
    Map.erase(id);
}
///----------------------------------------------------------------
void FlVtx::PrintAll(int lev){
  printf("--------------%3ld VERTICES-----------------------\n",FlVtx::Map.size());
  std::map<int, FlVtx*>::const_iterator itr;
  for(itr=FlVtx::Map.begin();itr!=Map.end();++itr)
  {
    printf("VTX [%3d]: ",itr->first);
    if(itr->second)itr->second->Print(lev);
    else printf(" NULL\n");
  }
};
///----------------------------------------------------------------
void FlVtx::Clear(){
    Log(4,"VTX#d: Clear%d tracks\n",id,N());
    tracks.clear();
};
///----------------------------------------------------------------
void FlVtx::Print(int lev){
  printf("Vtx: id=%3d flag=%3d Ntrk_out=%ld\t",id,flag,tracks.size());
  printf("pos=[%6.1f %6.1f %6.1f]\n",x,y,z);
  if(lev==0)return;
  --lev;
  if(trk_in){printf("IN:");trk_in->Print(lev);}
  for(unsigned long n=0;n<tracks.size();++n){
    printf("%3ld)",n);tracks[n]->Print(lev);

  }
}
///----------------------------------------------------------------
///----------------------------------------------------------------
///----------------------------------------------------------------
FlRead::FlRead():fDoSaveVtx(true),fDoSaveTrx(true),fFileName(0){
  fDoSaveSeg[FlSeg::kBTK]=1;
  fDoSaveSeg[FlSeg::kMTK]=0;
  fDoSaveSeg[FlSeg::kPB]=0;
  f_EvSize=-1;
  printf("Verbose=%d\n",FlRead::FlVerbose);
  Log(1,"New FlRead\n");
};
///----------------------------------------------------------------
int FlRead::Open(char* fname){
  fFileName=fname; 
  Log(1,"opening file \'%s\'\n",fFileName);
  fFile.open(fFileName); 
  if(!fFile.is_open()){
    Log(0,"Error opening file \"%s\"\n",fFileName);
    return 0;
  }
  fFile.seekg(0,std::ios_base::end);
  fFileLen=fFile.tellg();
  fFile.seekg(0,std::ios_base::beg);
  fFilePos=0;
  Log(0,"Length=%ld\n",fFileLen);
  return 1;
}
///----------------------------------------------------------------
void FlRead::Close(){
  //fFileLen=0; 
  fFilePos=0; 
  fFile.close();
  Log(1,"close file \'%s\'\n",fFileName);
}
///----------------------------------------------------------------
int FlRead::Next(){
  assert(fFile.is_open());
  
  Log(5,"reading rec\n");
  ///read fortran binary record
  if(fFile.eof())return 0;
  short int h[2];
  fFile.read((char*)&h,4);
  Log(5,"Record size=%d\n",h[0]);
  fFile.read((char*)&fRec1,h[0]);
  fFile.read((char*)&h,4);
  fRec1.x*=1e4; fRec1.y*=1e4; fRec1.z*=1e4;
  LOG(5)fRec1.print();
  if(fRec1.desc==kEVT){
      fRecE=fRec1;
      Log(4,"Next event is #%d\n",fRecE.num);
    }
  if(fFile.eof()){
    Log(1,"EOF reached\n");
    return 0;
    }
  Log(5,"Read record of size %d\n",h[0]);
  fRecSize=h[0]+8;
  fFilePos=fFile.tellg();
  return h[0];
}
///----------------------------------------------------------------
int FlRead::FindEvent(int NEv){
  Log(1,"Searching for Event#%d\n",NEv);
  int dEv=NEv-fRecE.num;
  while(Next()){
    if(fRec1.desc!=kEVT)continue;
    if(fRecE.num==NEv){
      Log(1,"Found eventd #%d\n",fRecE.num);
      return 1;
    }
    Log(2,"Event #%d -> Event #%d = %d events\n",fRecE.num,NEv,dEv);
    
    if(fRecE.num>5000){
      f_EvSize=(fFilePos*1./(fRecSize*fRecE.num));
      Log(5,"Average event size = %10.2f records (based on %d events)\n",f_EvSize,fRecE.num);
      }
    dEv=NEv-fRecE.num;
    ///"clever" finding
    if(std::fabs(dEv)>2000 && f_EvSize>0){
      Log(5,"Event #%d -> Event #%d = %d events\n",fRecE.num,NEv,dEv);
      long Shft=floor((dEv-100)*f_EvSize)*fRecSize;
      Log(6,"Shift = %ld\n",Shft);
      fFile.seekg(Shft,std::ios_base::cur);
      continue;
    }
  }
  Log(1,"Event#%d not found.\n",NEv);
  return 0;
}
///----------------------------------------------------------------
void FlRead::ClearSeg(){
  std::map<long, FlSeg*>::const_iterator itr;
  Log(3,"Killing %ld segments\n",FlSeg::Map.size());
  while(FlSeg::Map.empty()==false){
    itr=FlSeg::Map.begin();
  //for(itr=FlSeg::Map.begin();itr!=FlSeg::Map.end();++itr){
    if(itr->second!=0)
    delete (*itr).second;
    else FlSeg::Map.erase(itr->first);
  }
  FlVtx::Map.clear();
};
///----------------------------------------------------------------
void FlRead::ClearTrx(){
  std::map<int, FlTrk*>::const_iterator itr;
  Log(3,"Killing %ld tracks\n",FlTrk::Map.size());
  //FlTrk::PrintAll();
  while(FlTrk::Map.empty()==false){
    itr=FlTrk::Map.begin();
  //for(itr=FlTrk::Map.begin();itr!=FlTrk::Map.end();++itr){
    if(itr->second!=0)
    delete (*itr).second;
    else FlTrk::Map.erase(itr->first);
  }
  FlTrk::Map.clear();
  
};
///----------------------------------------------------------------
void FlRead::ClearVts(){
  Log(3,"Killing %ld vertices\n",FlVtx::Map.size());
  std::map<int, FlVtx*>::const_iterator itr;
  while(FlVtx::Map.empty()==false){
    itr=FlVtx::Map.begin();
  //for(itr=FlVtx::Map.begin();itr!=FlVtx::Map.end();++itr){
    if(itr->second!=0)
    delete (*itr).second;
    else FlVtx::Map.erase(itr->first);
  }
  FlVtx::Map.clear();
};
///----------------------------------------------------------------

///----------------------------------------------------------------
int FlRead::ReadEvent(){
  fRec0.desc=-1;
  fEvent=fRecE.num; 
  fE0=fRecE.p; 
  Log(2,"Reading evt#%d\n",fEvent);
  while(Next()){
    switch(fRec1.desc){
      case kEVT:
	Log(2,"Finished reading event.\n"); 
	return 1;
      case kIN:
	if(fDoSaveVtx==false)continue;
	Log(4,"Making VTX #%d\n",fRec1.num);
	fTrkCur=FlTrk::Map[fRec1.num];
	fVtxCur=new FlVtx(fTrkCur,fRec1);
	break;
      case kOUT:
	if(fDoSaveTrx==false)continue;
	if(FlVtx::Map.empty()){Log(2,"Create ZeroVtx!\n"); fVtxCur=new FlVtx(0,fRecE);}
	fTrkCur=FlTrk::Map[fRec1.num];
	///merge elastic scattering tracks e.t.c.
// 	if(fTrkCur==0)
	/*if((fVtxCur->flag==100 || fVtxCur->flag==103)&&(fVtxCur->trk_in->pdg==FLUKA2PDG(fRec1.pid))){
	  fTrkCur=fVtxCur->trk_in;
          Log(5,"Continue track#%d with track#%d\n",fTrkCur->id,fRec1.num);
	  fTrkCur->id=fRec1.num;
          FlTrk::Map[fRec1.num]
          
	  }*/
	///--------------------------------------
	if(fTrkCur==0){
	  ///track is new
	  Log(4,"Making TRK #%d\n",fRec1.num)
	  fVtxCur->tracks.push_back(new FlTrk(fVtxCur,fRec1));
	  //LOG(4)FlTrk::PrintAll();
	}
	else{
	  ///track already exsists. False vertex. 
	}
	break;
      case kEM0:
      case kEM1: 
         ///FIXME 
         //if(fRec1.pid!=11)break;
         ///dirty workaround for muon scattering study
         
         if(fTrkCur==0||fTrkCur->id!=fRec1.num)
	    fTrkCur=FlTrk::Map[fRec1.num];
	 
	 if(fTrkCur==0){
	   Err(2,"No track#%d!\n",fRec1.num);
	   ERR(2)fRec1.print();
	   ERR(2)FlTrk::PrintAll();
	   break;
	 }
	 //fRec0=fTrkCur->fLastRec;
	 int side=CalcSegmSide(&fRec1,&fRec0);
	 Log(5,"Side=%d\n",side);
	 Log(5,"DoSaveSeg[%d%d%d]\n",fDoSaveSeg[0],fDoSaveSeg[1],fDoSaveSeg[2]);
	 if(side>=0 && fDoSaveSeg[side]){
	   FlSeg* segm = new FlSeg(fTrkCur,fRec0,fRec1,side);
  	   Log(4,"Making segment in trk#%d\n",fTrkCur->id);
	   fTrkCur->segs.push_back(segm);
	 }
	 //fTrkCur->fLastRec=fRec1;
	 fRec0=fRec1;
         break;
    }
    if(fFile.eof()){
      Log(1,"EOF reached\n");
      return 0;
    }
    
    ///read vertex:
    
  }
  return 0;
}
///----------------------------------------------------------------
