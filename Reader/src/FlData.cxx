
#include <cassert>
#include <cmath>
#include <cstdio>

#include "NiceLog.h"
#include "FlData.h"

#define _NiceVlev FlRead::FlVerbose

long FlSeg::IDSEG=0;
int FlTrk::IDTRK=0;
int FlVtx::IDVTX=0;
std::map <long,FlSeg*> FlSeg::Map;
std::map <int, FlTrk*> FlTrk::Map;
std::map <int, FlVtx*> FlVtx::Map;

int FlRead::FlVerbose;
FILE* FlRead::outfile=stdout;


//FlData printing to file
#define Fprintf(...) fprintf(FlRead::outfile,__VA_ARGS__)

float FlMethods::CalcKinkSquared(float tx1,float ty1,float tx2,float ty2){
  double r1m =tx1*tx1+ty1*ty1+1.0;
  double r2m =tx2*tx2+ty2*ty2+1.0;
  double r1r2=tx1*tx2+ty1*ty2+1.0;
  return 1-r1r2*r1r2/(r1m*r2m);
}

float FlMethods::CalcKink(float tx1,float ty1,float tx2,float ty2){
  double r1m =tx1*tx1+ty1*ty1+1.0;
  double r2m =tx2*tx2+ty2*ty2+1.0;
  double r1r2=tx1*tx2+ty1*ty2+1.0;
  return sqrt(1-r1r2*r1r2/(r1m*r2m));
}

float FlMethods::CalcIP(FlVtx* v, FlSeg* s){
  double r0[3]={s->x-v->x, s->y-v->y, s->z-v->z};
  double vv[3]={s->tx,s->ty, 1};
  double r0v=r0[0]*vv[0]+r0[1]*vv[1]+r0[2]*vv[2];
  double vv2=vv[0]*vv[0]+vv[1]*vv[1]+vv[2]*vv[2];
  double r[3];
  for(int i=0;i<3;++i)
    r[i]=r0[i]-vv[i]*r0v/vv2;
  return sqrt(r[0]*r[0]+r[1]*r[1]+r[2]*r[2]);
}

float FlMethods::CalcIP(FlSeg* s1, FlSeg* s2){
  double vx=s1->ty-s2->ty;
  double vy=s2->tx-s1->tx;
  double vz=s1->tx*s2->ty-s2->tx*s1->ty;
  double v=sqrt(vx*vx+vy*vy+vz*vz);
  double r1v=s1->x*vx+s1->y*vy+s1->z*vz;
  double r2v=s2->x*vx+s2->y*vy+s2->z*vz;
  double ip=(r1v-r2v)/v;
  return ip;
}

void record::print() {
        Fprintf("%d) #%d PID=%d par=%d ", desc, num, pid, par);
        Fprintf("P=%g T=(%g %g) [%g %g %g]\n", p, tx, ty, x, y, z);
    };

int CalcSegmSide(record *r0,record *r1){
  _LogCmd(5,r0->print());
  _LogCmd(5,r1->print());
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
      Fprintf("What's #%i in FLUKA?\n",code);
      return -1;
    }
  }
};

//#define Log(vlev) if(vlev<=FlRead::FlVerbose)
///----------------------------------------------------------------
FlSeg::FlSeg():id(-1),pdg(0),x(0),y(0),z(0),trk(NULL){
};
///----------------------------------------------------------------
FlSeg::FlSeg(FlTrk* t, record r1, record r2, int Side):id(++IDSEG),side(Side){
//  assert(t!=NULL);
  assert(r1.desc==kEM0||r1.desc==kEM1);
  assert(r2.desc==kEM0||r2.desc==kEM1);
  assert(r1.desc!=r2.desc);
  ///register in map
  Map[id]=this; 
  plate=r1.par;
  
  if(r2.p>r1.p){std::swap(r1,r2);}
  kink=FlMethods::CalcKink(r1.tx,r1.ty,r2.tx,r2.ty);
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
  _Log(3,"Created a segment:\n");
  _LogCmd(3,Print(0));
};
///----------------------------------------------------------------
FlSeg::~FlSeg(){
  _Log(5,"I am being deleted! My id is %ld\n",id);
  Clear(); 
  if(Map[id]==this)
    Map.erase(id);
}
///----------------------------------------------------------------
void FlSeg::Clear(){trk=0;};
///----------------------------------------------------------------
void FlSeg::Print(int lev) const {
  Fprintf("seg#%3ld pdg=%5d trk#%4d; Plt#%d side_%d, dz=%4.1f, eloss=%5.3g\t",id,pdg,trk_id,plate,side,dz,eloss);
  Fprintf("pos=%6.1f %6.1f %6.1f [%5.4f %5.4f] P=%4.4f k=%g\n",x,y,z,tx,ty,p,kink*1e3);
};
///----------------------------------------------------------------
void FlSeg::PrintAll(int lev){
  Fprintf("--------------%3ld Segments-----------------------\n",FlSeg::Map.size());
  std::map<long, FlSeg*>::const_iterator itr;
  for(auto && itr: Map){
    Fprintf("SEG [%3ld]: ",itr.first);
    if(itr.second)itr.second->Print(lev);
    else Fprintf(" NULL\n");
  }
};
///----------------------------------------------------------------
///----------------------------------------------------------------
///----------------------------------------------------------------
FlTrk::FlTrk(FlVtx* v,const record& r):id(r.num),x(r.x),y(r.y),z(r.z),p(r.p),tx(r.tx),ty(r.ty),vtx0(v),vtx1(NULL){
  assert(r.desc==kOUT);
//   _Log(2,"Adding trk with id=%d (0x%x)\n",id,this);
  _Log(2,"Created a track:\n");
  pdg=FLUKA2PDG(r.pid);
  _LogCmd(2,Print(0));
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
  Fprintf("--------------%3ld TRACKS-----------------------\n",FlTrk::Map.size());
  std::map<int, FlTrk*>::const_iterator itr;
  for(itr=Map.begin();itr!=Map.end();++itr){
    Fprintf("TRK [%3d]: ",itr->first);
    if(itr->second)itr->second->Print(lev);
    else Fprintf(" NULL\n");
  }
};
///----------------------------------------------------------------

void FlTrk::Clear(){
  vtx0=0; 
  vtx1=0; 
  segs.clear();
  _Log(4,"Trk#%d: Clear%d segs\n",id,N());
};
///----------------------------------------------------------------
void FlTrk::Print(int lev) const {
  Fprintf("trk#%3d pdg=%5d\t",id,pdg);
  Fprintf("NS=%d\t",N());
  Fprintf("pos=[%6.1f %6.1f %6.1f] tt=[%5.4f %5.4f] P=%4.4f\n",x,y,z,tx,ty,p);
  if(lev==0)return;
  --lev;
  for(auto&& seg: segs){
    seg->Print(lev);
  }  
};
///----------------------------------------------------------------
///----------------------------------------------------------------
///----------------------------------------------------------------
FlVtx::FlVtx(FlTrk* t,const record &r):id(IDVTX++),x(r.x),y(r.y),z(r.z),tx(r.tx),ty(r.ty),p(r.p),trk_in(t){
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
  Fprintf("--------------%3ld VERTICES-----------------------\n",Map.size());
  for(auto&& itr: Map)
  {
    if(itr.second)itr.second->Print(lev);
    else Fprintf(" NULL\n");
  }
};
///----------------------------------------------------------------
void FlVtx::Clear(){
  _Log(4,"VTX#%d: Clear %d tracks\n",id,N());
  tracks.clear();
};
///----------------------------------------------------------------
void FlVtx::Print(int lev) const {
  // if(flag<0)return;
  Fprintf("Vtx: id=%3d flag=%3d Ntrk_out=%ld\t",id,flag,tracks.size());
  Fprintf("pos=[%6.1f %6.1f %6.1f]\n",x,y,z);
  if(lev==0)return;
  --lev;
  if(trk_in){Fprintf("IN:");if(flag>0)trk_in->Print(lev);else Fprintf("trk#%d\n",trk_in->id);}
  for(unsigned long n=0;n<tracks.size();++n){
    Fprintf("%3ld)",n);tracks[n]->Print(lev);

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
  // Fprintf("Verbose=%d\n",FlRead::FlVerbose);
  _Log(1,"New FlRead\n");
};
///----------------------------------------------------------------
int FlRead::Open(char* fname){
  fFileName=fname; 
  _Log(1,"opening file \'%s\'\n",fFileName);
  fFile.open(fFileName); 
  if(!fFile.is_open()){
    _Log(0,"Error opening file \"%s\"\n",fFileName);
    return 0;
  }
  fFile.seekg(0,std::ios_base::end);
  fFileLen=fFile.tellg();
  fFile.seekg(0,std::ios_base::beg);
  fFilePos=0;
  _Log(0,"Length=%ld\n",fFileLen);
  return 1;
}
///----------------------------------------------------------------
void FlRead::Close(){
  //fFileLen=0; 
  fFilePos=0; 
  fFile.close();
  _Log(1,"close file \'%s\'\n",fFileName);
}
///----------------------------------------------------------------
int FlRead::Next(){
  assert(fFile.is_open());
  
  _Log(5,"reading rec\n");
  ///read fortran binary record
  if(fFile.eof())return 0;
  short int h[2];
  fFile.read((char*)&h,4);
  _Log(5,"Record size=%d\n",h[0]);
  fFile.read((char*)&fRec1,h[0]);
  fFile.read((char*)&h,4);
  fRec1.x*=1e4; fRec1.y*=1e4; fRec1.z*=1e4;
  _LogCmd(5,fRec1.print());
  if(fRec1.desc==kEVT){
    fRecE=fRec1;
    _Log(4,"Next event is #%d\n",fRecE.num);
  }
  if(fFile.eof()){
    _Log(1,"EOF reached\n");
    return 0;
  }
  _Log(5,"Read record of size %d\n",h[0]);
  fRecSize=h[0]+8;
  fFilePos=fFile.tellg();
  return h[0];
}
///----------------------------------------------------------------
int FlRead::FindEvent(int NEv){
  _Log(1,"Searching for Event#%d\n",NEv);
  int dEv=NEv-fRecE.num;
  while(Next()){
    if(fRec1.desc!=kEVT)continue;
    if(fRecE.num==NEv){
      _Log(1,"Found eventd #%d\n",fRecE.num);
      return 1;
    }
    _Log(2,"Event #%d -> Event #%d = %d events\n",fRecE.num,NEv,dEv);
    
    if(fRecE.num>5000){
      f_EvSize=(fFilePos*1./(fRecSize*fRecE.num));
      _Log(5,"Average event size = %10.2f records (based on %d events)\n",f_EvSize,fRecE.num);
    }
    dEv=NEv-fRecE.num;
    ///"clever" finding
    if(std::fabs(dEv)>2000 && f_EvSize>0){
      _Log(5,"Event #%d -> Event #%d = %d events\n",fRecE.num,NEv,dEv);
      long Shft=floor((dEv-100)*f_EvSize)*fRecSize;
      _Log(6,"Shift = %ld\n",Shft);
      fFile.seekg(Shft,std::ios_base::cur);
      continue;
    }
  }
  _Log(1,"Event#%d not found.\n",NEv);
  return 0;
}
///----------------------------------------------------------------
void FlRead::ClearSeg(){
  _Log(3,"Killing %ld segments\n",FlSeg::Map.size());
  // for(auto&& itr : FlSeg::Map) {
  while(!FlSeg::Map.empty()){
      auto itr=*FlSeg::Map.begin();
      if(itr.second!=0)delete itr.second;
      itr.second=0;
  }
  FlSeg::Map.clear();
};
///----------------------------------------------------------------
void FlRead::ClearTrx(){

  _Log(3,"Killing %ld tracks\n",FlTrk::Map.size());
  // for(auto&& itr : FlTrk::Map) {
  while(!FlTrk::Map.empty()){
      auto itr=FlTrk::Map.begin();
      // _Log(4,"Trk[%d]:\n",itr.first);
      // _LogCmd(4,itr.second->Print(0));
      if(itr->second!=0){
        delete itr->second;
      }
      else FlTrk::Map.erase(FlTrk::Map.begin());
      // else FlTrk::Map.erase(itr);
  }
  FlTrk::Map.clear();
};
///----------------------------------------------------------------
void FlRead::ClearVts(){
  _Log(3,"Killing %ld vertices\n",FlVtx::Map.size());
  // for(auto&& itr : FlVtx::Map) {
  while(!FlVtx::Map.empty()){
      auto itr=FlVtx::Map.begin();
      if(itr->second!=0){
        delete itr->second;
        itr->second=0;
      }
      else FlVtx::Map.erase(FlVtx::Map.begin());
  }
  FlVtx::Map.clear();
};
///----------------------------------------------------------------
void FlRead::PrintEvt(){
        Fprintf("+++ File \"%s\": Event#%d pos=%2.1f%%\n", fFileName, fEvtNum, (fFilePos * 100.) / fFileLen);
    }
//----------------------------------------------------------------
void FlRead::PrintStat(){
        Fprintf("%2ld Vts; %3ld Trx; %4ld Seg\n", FlVtx::Map.size(), FlTrk::Map.size(), FlSeg::Map.size());
    }
//------------------------------------------------------
void FlRead::SetSaveBits(const char* options){
    ///allowed options: 
    // LMBTV
    fDoSaveSeg[FlSeg::kPB ] = false;
    fDoSaveSeg[FlSeg::kMTK] = false;
    fDoSaveSeg[FlSeg::kBTK] = false;
    fDoSaveTrx = false;
    fDoSaveVtx = false;
    for(auto&& c : std::string(options)) {
        switch (c){
        case 'L': case 'l':
            fDoSaveSeg[FlSeg::kPB ] = true;
            break;
        case 'M': case 'm':
            fDoSaveSeg[FlSeg::kMTK ] = true;
            break;
        case 'B': case 'b':
            fDoSaveSeg[FlSeg::kBTK ] = true;
            break;
        case 'T': case 't':
            fDoSaveTrx = true;
            break;
        case 'V': case 'v':
            fDoSaveVtx = true;
            break;
        }
    }
}

///----------------------------------------------------------------
int FlRead::ReadEvent(){
  fRec0.desc=-1;
  fEvtNum=fRecE.num; 
  fEvtEnergy=fRecE.p;
  fEvtPdg=FLUKA2PDG(fRecE.pid);
  _Log(1,"Reading evt#%d\n",Event());
  while(Next()){
    switch(fRec1.desc){
      case kEVT:
          _Log(2,"Finished reading event.\n"); 
          return 1;
      case kIN:
          if(fDoSaveVtx==false)continue;
          _Log(4,"Making VTX #%d\n",fRec1.num);
          fTrkCur=FlTrk::Map[fRec1.num];
          fVtxCur=new FlVtx(fTrkCur,fRec1);
          break;
      case kOUT:
          if(fDoSaveTrx==false)continue;
          if(FlVtx::Map.empty()){_Log(2,"Create ZeroVtx!\n"); fVtxCur=new FlVtx(0,fRecE);}
          fTrkCur=FlTrk::Map[fRec1.num];

    	///--------------------------------------
          if(fTrkCur==0){
        	  ///track is new
            _Log(4,"Making TRK #%d\n",fRec1.num);
            fVtxCur->tracks.push_back(new FlTrk(fVtxCur,fRec1));
        	  //_LogCmd(4,FlTrk::PrintAll());
          }else{
      	  ///track already exsists. False vertex. 
            fVtxCur->flag*=-1;
          }
         break;
       case kEM0:
       case kEM1:  
           if(fTrkCur==0||fTrkCur->id!=fRec1.num)
             fTrkCur=FlTrk::Map[fRec1.num];

           if(fTrkCur==0){
            _Err(2,"No track#%d!\n",fRec1.num);
            _ErrCmd(2,fRec1.print());
            _ErrCmd(2,FlTrk::PrintAll());
            // break;
          }
  	 //fRec0=fTrkCur->fLastRec;
          int side=CalcSegmSide(&fRec1,&fRec0);
          _Log(5,"Side=%d\n",side);
          _Log(5,"DoSaveSeg[%d%d%d]\n",fDoSaveSeg[0],fDoSaveSeg[1],fDoSaveSeg[2]);
          if(side>=0 && fDoSaveSeg[side]){
            FlSeg* segm = new FlSeg(fTrkCur,fRec0,fRec1,side);
            _Log(4,"Making segment in trk#%d\n",fTrkCur->id);
            if(fTrkCur)fTrkCur->segs.push_back(segm);
          }
  	 //fTrkCur->fLastRec=fRec1;
          fRec0=fRec1;
        break;
      }
      if(fFile.eof()){
        _Log(1,"EOF reached\n");
        return 0;
      }

    ///read vertex:

    }
    return 0;
  }
///----------------------------------------------------------------
