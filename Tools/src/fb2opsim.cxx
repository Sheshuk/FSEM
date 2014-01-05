#include <iostream>
#include "EdbPVGen.h"
#include "EdbPattern.h"
#include "FlReader.h"
#include "EdbDataStore.h"

int N0event=0,Nevents=100; 
int vlevel=0;

bool DecCodes=0;
bool DoSelect=0;
bool DoSaveSegments=0;
bool DoCount=0;
TDatabasePDG gDB;
double lenmin=0;
double lenmax=1e10;
double Pmin=2;
const int heavycode[7]={0,0,9085,9081,9082,9084,9083};
char Geofile[20]="../brick.geo";

/// ***** decay length counting *****
double Lint[2]={0,0}; int Nint[2]={0,0};
///  *******************************

EdbTrackP* FindIncTrk(EdbVertex* v){
  for(int nt=0;nt<v->N();++nt)
    if(v->Zpos(nt)==0)return v->GetTrack(nt);
    return 0;
}

int Conv2G3(int code){
  if(code>9991)return heavycode[code-9991];
  return gDB.ConvertPdgToGeant3(code);
}

int SelVertices(EdbVertex* v0,TObjArray* all, TObjArray* sel,float dX, float dZ){
  EdbVertex *v;
  sel->Clear();
  sel->Add(all->At(0));
  int Ntrx=0;
  for(int nv=1;nv<all->GetEntries();++nv){
    v=(EdbVertex*) all->At(nv);
    v->SetQuality(-1.);
    if(v->Zpos(0)==1) continue; ///first track is not incoming
    if(v->Flag()==102 && abs(v->GetTrack(0)->PDG())==211)continue; ///pion decay  - discard
    double dd[3]={v->X()-v0->X(),v->Y()-v0->Y(),v->Z()-v0->Z()};
    if(vlevel>0)printf("vtx %d of %d: #%d\t dz=%2.2f\n",nv,all->GetEntries(),v->ID(),dd[2]);
//     if(dd[0]*dd[0]+dd[1]*dd[1]+dd[2]*dd[2]>lenmax*lenmax)continue;
    if(!DoSelect ||(dd[2]>=0 && dd[2]<dZ && fabs(dd[0])<dX && fabs(dd[1])<dX)){
      sel->Add(v);
      Ntrx+=v->N()-1;
      v->SetQuality(sqrt(dd[0]*dd[0]+dd[1]*dd[1]+dd[2]*dd[2]));
      if(vlevel>0)printf("=== sel#%d (0x%x) ===\n",sel->GetEntries(),v);
    }
  }
  return Ntrx;
}
///--------------------------------------------------------------------
double GetTrkLen(EdbTrackP* t, EdbBrickP* b){
  double r[3]={t->VertexS()->X(),t->VertexS()->Y(),t->VertexS()->Z()};
  if(t->VertexE()){
    if(t->VertexE()->Quality()>0)return t->VertexE()->Quality();
    r[0]=t->VertexE()->X()-r[0];
    r[1]=t->VertexE()->Y()-r[1];
    r[2]=t->VertexE()->Z()-r[2];
    return -sqrt(r[0]*r[0]+r[1]*r[1]+r[2]*r[2]);
  }
  ///no end vertex: find exit point
//   printf("no end vertex\n");
//   printf("b=0x%x\n",b);
  if(!b)return 0;
  double dz=((t->P()>0)?(b->Zmax()):(b->Zmin()))-t->Z();
//   printf("dz=%2.2f (%2.2f %2.2f %2.2f)\n",dz,b->Zmin(),t->Z(),b->Zmax());
  double dx=(t->TX()*dz);
//   printf("dx=%2.2f (%2.2f %2.2f %2.2f)\n",dx,b->Xmin(),t->X(),b->Xmax());
  double dy=(t->TY()*dz);
//   printf("dy=%2.2f (%2.2f %2.2f %2.2f)\n",dy,b->Ymin(),t->Y(),b->Ymax());
  double zx=dz;
  double zy=zx;
  if(t->X()+dx>b->Xmax()){dx=b->Xmax()-t->X();zx=dx/t->TX();}
  if(t->X()+dx<b->Xmin()){dx=b->Xmin()-t->X();zx=dx/t->TX();}
  
  if(t->Y()+dy>b->Ymax()){dy=b->Ymax()-t->Y(); zy=dy/t->TY();}
  if(t->Y()+dy<b->Ymin()){dy=b->Ymin()-t->Y(); zy=dy/t->TY();}
  
//   if(vlevel>1){
//     printf("trk#%d (%2.2f %2.2f %2.2f) [%2.4f %2.4f]\n",t->ID(),t->X(),t->Y(),t->Z(),t->TX(),t->TY());
//     printf("zx=%2.1f zy=%2.1f\n",zx,zy);
//   }
  double zz=(abs(zx)>abs(zy))?zx:zy;
  dx=(t->TX()*zz);
  dy=(t->TY()*zz);
//   printf("dr=[%2.1f %2.1f %2.1f]\n",dx,dy,zz);
  return -sqrt(dx*dx+dy*dy+zz*zz);
}
///--------------------------------------------------------------------
void fb2opsim(const char* fname, const char* outfname){
  ///process FlukaSim file to tree with EdbDataStore
  printf("Processing \"%s\" to opsim beamfile \"%s\". Taking event from #%d to #%d\n",fname,outfname,N0event,Nevents);
  FlReader* reader=new FlReader();
  printf("made FlReader\n");
  reader->fTracks=new TObjArray;
  reader->fVertices=new TObjArray;
  reader->fPVRaw=0;
  reader->fPVSeg=0;
  reader->fBrick=0;
  
  reader->fDoMakeTrx=1;
  reader->fDoMakeVtx=1;
  reader->fDoReadSeg=DoSaveSegments;
  reader->fDoMakeSeg=1;
  reader->FillBrick(Geofile);
  reader->fLenMin=lenmin;
  ///fill patterns
  printf("Merge Vts closer than %2.2f microns\n",reader->fLenMin);
  reader->fVerbose=vlevel;
  
  ifstream* is=new ifstream(fname);
  
  FILE* fff=fopen(outfname,"w+");
  
  bool okay=true;
  printf("reading 0\n");
  reader->GotoEventB(is,N0event);
  printf("done\n");
  if(!reader->fEvt)printf("no evt read!\n");
  reader->fEvt->Dump();
  EvtHdr ev0;
  int EvtNum=reader->fEvt->n;
  int NEv_proc=0;
  int NEv_sel=0;
  int NVtSec=0;
  double DzMax=0;
  const double zer=0;
  double Lll;
  TH1F * hmul=new TH1F("hmul","hmul",100,0,100);
  
  while(okay){
    if(vlevel>=2)printf("reseting...\n");
    reader->fVertices->Clear();
    reader->fTracks->Clear();
    EvtNum++;
    ev0=(*reader->fEvt);
    NEv_proc++;
    
    if(EvtNum%1000==0||(vlevel>0))
      printf("==== reading event #%d ====\n",EvtNum);
    okay=reader->ProcessEventB(is);
    
    //if(EvtNum<Nevents)continue;
    if((Nevents!=0)&&(EvtNum>=Nevents))break;
    
    if(vlevel>0) printf("%s okay\n",okay?"":"NOT");
//     printf("THERE are %d vertices and %d trx in total\n",reader->fVertices->GetEntries(),reader->fTracks->GetEntries());
    ///output
    //   WRITE(30,114)NEV_CURR,EV_TYPE(1),EV_TYPE(2),0,
    //   &      NVTX_TOT,NTRK_TOT,EV_ETOT
    char line[500];
//     if(reader->NVertices()<2)continue;
    TObjArray selVts;
    EdbVertex* v0=reader->GetVertex(0);
    int nselT=SelVertices(v0,reader->fVertices,&selVts,5000,3600);
    int nsel= selVts.GetEntries();
    ///----------------count lengths and interactions-------------
    for(int nt=0;nt<v0->N();++nt){
      if(v0->Zpos(nt)==0)continue;
      EdbTrackP* t=v0->GetTrack(nt);
      if(t->P()<Pmin)continue;
	/// get length
	Lll=GetTrkLen(t,reader->fBrick);
	switch(abs(t->PDG())){
	  case 211:
	  case 2212:
	  case 321:
	  case 411:
	  case 13:
	  case 15:
	  case 3112:
	  case 3222:
	  case 4122:
	  case 431:
	    printf("CHARGED trk#%d=%d (%d seg), L=%2.2f\n",t->ID(),t->PDG(),t->N(),Lll);
	    Lint[0]+=fabs(Lll);
	    Nint[0]+=(Lll>0)?1:0;
	    break;
	  case 2112:
	  case 111:
	  case 130:
	  case 310:
	  case 421:
	  case 221:
	  case 3212:
	  case 3322:
	    Lint[1]+=fabs(Lll);
	    Nint[1]+=(Lll>0)?1:0;
	    printf("NEUTRAL trk#%d=%d (%d seg), L=%2.2f\n",t->ID(),t->PDG(),t->N(),Lll);
	    break;
	  default:
	    break;
	}
    }
    ///-----------------------------------------------------------
    hmul->Fill(nsel);
    if(nsel<2)continue;
    NEv_sel++;
//     printf("selected %d vts of %d\n",selVts.GetEntries(),reader->NVertices());
    fprintf(fff,"    % 5.1d% 5.1d% 5.1d% 5.1d% 5.1d% 5.1d% 10.5f\n",ev0.n+1,Conv2G3(ev0.pid),4,0,nsel,nselT+v0->N(),ev0.p);
    printf("z0: %d %2.1f\n",EvtNum,v0->Z());
    for(int nv=0;nv<selVts.GetEntries();++nv){
      EdbVertex* v=(EdbVertex*)selVts.At(nv);
      EdbTrackP* t_in=FindIncTrk(v);
      int v_pid=4;
      int v_id=0;
      int n_trk=v->N();
      int Nt=0;
      int t_pdg=0;
      double px,py,pz;
      if(t_in){
	v_pid=Conv2G3(t_in->PDG());
	if(DecCodes)v_pid+=(v_pid>0)?9100:-9100;
	v_id=t_in->ID();
	n_trk--;
      }
      else if(nv!=0)continue; ///skip vertices with no incoming trk
      if(v!=v0)printf("z1: #%d %2.1f %2.1f %d (%d), L=%2.4f\n",v->ID(),v0->Z(),v->Z(),v->Flag(),v->GetTrack(0)->PDG(),v->Quality());
      if(v->Z()-v0->Z()>DzMax)DzMax=v->Z()-v0->Z();
      if(nv!=0)NVtSec++;
//        printf("Output vertex#%d (flg %d) with %d tracks (%d) dz=%2.1f z=%2.1f\n",v->ID(),v->Flag(),v->N(),v->Nn(),v->Z()-v0->Z(),v->Z());
      fprintf(fff," % 5.1d% 5.1d% 5.1d% 5.1d% 10.5f % 10.5f % 10.5f\n",nv+1,n_trk,v_id,v_pid,v->X()*.001,v->Y()*.001,v->Z()*.001);
      for(int nt=0;nt<v->N();++nt){
	if(v->Zpos(nt)==0)continue;
	EdbTrackP* t=v->GetTrack(nt);
	t_pdg=Conv2G3(t->PDG());
	if(DecCodes){
	  ///change code if particle is decayed
	  if(t->VertexE() && t->VertexE()->Quality()>0)t_pdg+=(t_pdg>0)?9100:-9100;
	};
	pz=t->P()/sqrt(1.+t->TX()*t->TX()+t->TY()*t->TY());
	px=pz*t->TX();
	py=pz*t->TY();
	//          WRITE(30,113),
	//          &      b_num,NTRK_CURR,NVTX_CURR,b_pid,
	//          &      b_P*b_TX,b_P*b_TY,b_P*b_TZ,
	//          &      0.,0.,0.,0.,0.,0.,0.
	fprintf(fff," % 5.1d% 5.1d% 5.1d% 5.1d % 10.5f % 10.5f % 10.5f",t->ID(),++Nt,nv+1,t_pdg,px,py,pz);
	if(DecCodes)fprintf(fff," % 10.5f % 10.5f % 10.5f % 10.5f % 10.5f % 10.5f % 10.5f\n",zer,zer,zer,zer,zer,zer,zer);
	else fprintf(fff," %d\n",t->N());
	if(DoSaveSegments==false)continue;
	for(int ns=0;ns<t->N(); ++ns){
	  EdbSegP* s=t->GetSegment(ns);
	  fprintf(fff,"% 5.1d% % 10.5f % 10.5f % 10.5f % 10.5f % 10.5f % 10.5f\n",s->Plate(),s->X(),s->Y(),s->Z(),s->TX(),s->TY(),s->P());
	}
      }
    }
  }
  fprintf(fff,"*  Selected %d of %d events whith at least 1 secondary vertex %s\n",NEv_sel,NEv_proc,DoSelect?"around PV":"in all volume");
//   fprintf(fff,"*  Maximal interaction length along Z = %2.4f => L_int=%2.6g cm\n",DzMax, DzMax*NEv_proc*1e-4/NEv_sel);
  fprintf(fff,"*  CHARGED (p>%2.1f GeV): L=%2.4g m (%2.4g mm per event), N=%d (%2.4g int per event) lambda=%2.4g cm\n",
	  Pmin,Lint[0]*1e-6,Lint[0]*1e-3/NEv_proc,Nint[0],Nint[0]*1./NEv_proc,Lint[0]*1e-4/Nint[0]);
  fprintf(fff,"*  NEUTRAL (p>%2.1f GeV): L=%2.4g m (%2.4g mm per event), N=%d (%2.4g int per event) lambda=%2.4g cm\n",
	  Pmin,Lint[1]*1e-6,Lint[1]*1e-3/NEv_proc,Nint[1],Nint[1]*1./NEv_proc,Lint[1]*1e-4/Nint[1]);
//   fprintf(fff,"*  Total there are %d secondary vertices (on average %2.4f per event)\n",NVtSec,NVtSec*1./NEv_proc);
  fclose(fff);
  TCanvas c1;
  c1.SetLogy();
  hmul->Draw();
  c1.Print("hmul.eps");
}
///------------------------------------------------------------------
void helpme(){
  printf("------------------\n");
  printf("fb2opsim by ASh\n");
  printf("Usage: fb2fedra [-v=Vlevel -N=Nev -N0=Nev0] 'inputfile.out' 'outputfile.beamfile'\n");
  printf("---------------------------------------------------------\n");
  printf("   -v=Vlevel  - set verbosity level (0 - quiet, 3 - full)\n");
  printf("   -n=Nev     - set number of events to process\n");
  printf("                Note: to process single event #M run with -n=-M \n");
  printf("                Note: to process all events run with -n=0 \n");
  printf("   -g=geofile - read geometry from file \"geofile.geo\" (def: brick.geo)\n");
  printf("   -n0=Nev0   - set number of starting event\n");
  printf("---------------------------------------------------------\n");
  printf(" Enjoy!\n");
}
///------------------------------------------------------------------
int main(int argc, char** argv){
  if(argc<3){printf("need at least 2 arguments:\n");helpme();return 0;}
  char* infle=0;
  char* outfle=0;
  for(int i=1;i!=argc;i++){
    printf("argv[%d]=\'%s\'\n",i,argv[i]);
    if(!strncmp(argv[i],"-v=XX",3)) {sscanf(argv[i],"-v=%d",&vlevel ); continue;}
    if(!strncmp(argv[i],"-n=XX",3)) {sscanf(argv[i],"-n=%d",&Nevents); continue;}
    if(!strncmp(argv[i],"-n0=XX",4)) {sscanf(argv[i],"-n0=%d",&N0event); continue;}
    if(!strncmp(argv[i],"-l0=XX",4)) {sscanf(argv[i],"-l0=%lf",&lenmin); continue;}
    if(!strncmp(argv[i],"-l1=XX",4)) {sscanf(argv[i],"-l1=%lf",&lenmax); continue;}
    if(!strncmp(argv[i],"-g=XX",3)) {sscanf(argv[i],"-g=%s",Geofile); continue;}
    if(!strncmp(argv[i],"-p=XX",3)) {sscanf(argv[i],"-p=%lf",&Pmin); continue;}
    if(!strncmp(argv[i],"-d",2)) {DecCodes=1; continue;}
    if(!strncmp(argv[i],"-s",2)) {DoSelect=1; continue;}
    if(!strncmp(argv[i],"-S",2)) {DoSaveSegments=1; continue;}
    if(!strncmp(argv[i],"-c",2)) {DoCount=1; continue;}
    if(!infle) {printf("Set INPUT file:  \"%s\"\n",argv[i]); infle =argv[i]; continue;}
    if(!outfle){printf("Set OUTPUT file: \"%s\"\n",argv[i]); outfle=argv[i]; continue;}
    printf("ERROR! UNKNOWN OPTION \"%s\"! Abort\n",argv[i]);
    helpme();
    return -1;
  }
  fb2opsim(infle,outfle);
}