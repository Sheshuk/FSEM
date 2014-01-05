
// #include <ios>
// #include <iostream>
#include "FlReader.h"
#include <map>
#include <assert.h>
// ClassImp(FlReader);

int UnGetN(ifstream* is,int Nbt){
       for(int i=0;i<Nbt;i++)
           is->unget();
//     std::streampos POS=(int)is->tellg()-Nbt;
//     is->seekg(POS);
    return Nbt;
}

int GetN(ifstream* is,char* dst,int Nbt){
    //read N bytes from is to dst
    is->read(dst,Nbt);
    return Nbt;
}
bool is_compat(point p1,point p2){return (p1.n==p2.n)&&(p1.vid==p2.vid);}
///---------------------------------------------------------------------------
const int heavycode[7]={0,50000050,1000000000,1000010020,1000010030,1000020040,1000020050};
// const int heavycode[7]={0,0,9085,9081,9082,9084,9083};
const int heavyMass[7]={0,0,5,1.9,2.8,2.8,3.7};


///---------------------------------------------------------------------------
int FlReader::FLUKA2PDG(int code){
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
// int FLUKA2G3(int code){
//  int pdg=FLUKA2PDG(code);
//  if(pdg>9990)return pdg;
//  else return db->ConvertPdgToGeant3(pdg);
// }
TDatabasePDG* bd=0;
///---------------------------------------------------------------------------|
void FlReader::ClearPV(){
  ClearPV(fPVSeg);
  ClearPV(fPVRaw);
}
///---------------------------------------------------------------------------|
void FlReader::ClearPV(EdbPatternsVolume* vol){
	if(vol==0)return;
        int N=vol->Npatterns();
        if(!N)return;
        TObjArray* Sgs=0;
        for(int k=0;k<N;k++) {
            Sgs=vol->GetPattern(k)->GetSegments();
            if(Sgs)Sgs->Delete();
//             fVolume->GetPattern(k)->Reset();
//             delete fVolume->GetPattern(k);
        }
        //if(fVolume->ePatterns)fVolume->ePatterns->Clear();
}
///---------------------------------------------------------------------------|
void FlReader::KillEmptyVts(){
  int Nvtot=NVertices();
  for(int nv=0;nv<Nvtot;++nv){
    EdbVertex* v=GetVertex(nv);
    Say(Form("v[%d]=0x%x\n",nv,v),3);
    Say(Form("%d/%d) v#%d (%d) : %d trx\n",nv,Nvtot,v->ID(),v->Flag(),v->N()),3);
    if(v->N()==0){
      fVertices->RemoveAt(nv);
      delete v;
      nv--;
      Nvtot=NVertices();
      Say(Form("kill (%d of %d)\n",nv,Nvtot),3);
    }
  }
}
///---------------------------------------------------------------------------|
void FlReader::Say(TString msg,int vlevel){
        if(fVerbose>=vlevel)printf(">>FlReader>>:%s",msg.Data());
}
///---------------------------------------------------------------------------
void FlReader::DumpEvt(int vlevel, bool detail){
    if(fVerbose>=vlevel){
        if(fEvt) fEvt->Dump();
        else Say("No event yet!",vlevel);
    }
    if(!detail)return;
    EdbVertex* v;
    EdbTrackP* t;
    for(int nv=0;nv<NVertices();++nv){
      v=GetVertex(nv);
      if(vlevel>2)printf("+-- Vtx#%d [%d]: %d trx\n",v->ID(),v->Flag(),v->N());
      for(int nt=0;nt<v->N();++nt){
	t=v->GetTrack(nt);
	if(vlevel>2)printf("+----- trk#%d [%d] (%s --> %s), p=%3.3f tt=[%2.3f %2.3f]: %d seg\n",t->ID(),t->PDG(),
	       t->VertexS()?Form("vs#%d",t->VertexS()->ID()):"?",
	       t->VertexE()?Form("ve#%d",t->VertexE()->ID()):"?",
	       t->P(), t->TX(),t->TY(),t->N());
      }
    }
}
///---------------------------------------------------------------------------
int FlReader::READ_BINREC(ifstream* is,record* rec){
    ///read binary record
    ///printf("Read BINREC! (pos size=%d, off size=%d)\n",sizeof(fpos_t),sizeof(off_t));
    ///Say(Form("--- Reading binary record at %ld\n",is->tellg()),6);
    short int h[2];
    GetN(is,(char*)&h,4);
    Say(Form("--- BinRec size = %d\n",h[0]),7);
//     if(sizeof(data)!=h[0])printf("ohno! %d!=%d\n",sizeof(data),h[0]);
    GetN(is,(char*)rec,h[0]);
    GetN(is,(char*)&h,4);
//     if(fVerbose>3)rec->print();
    if(is->eof()){
      Say("--- EOF reached\n",4);
      return 0;
    }
    rec->pid=ConvertCode(rec->pid);
    if(rec->desc==0){
        if(!fEvt)fEvt=new EvtHdr;
        fEvt->n=rec->num-1;
        fEvt->pid=rec->pid;
        fEvt->p=rec->P;
        fEvt->tx=rec->tx;
        fEvt->ty=rec->ty;
    }
    Say(Form("-- read %d bytes\n",h[0]+8),6);
    return h[0]+8;
}

///---------------------------------------------------------------------------
int FlReader::ConvertCode(int flka_code){
  ///convert FLUKA particle code to...
//   switch (fPCodeFlag){
//     case 0: ///fluka! no conversion
//       return flka_code;
//     case 1: ///geant3
//       return FLUKA2G3 (flka_code);
//     case 2: ///PDG
      return FLUKA2PDG(flka_code);
//   }
}
///---------------------------------------------------------------------------
char calcW(point* p1){
//   char w=0;
  float beta=0;
  int pdg=p1->pid;
  static const char _cw[2]={32,22};
//   if(p1->pid>9990){
//     pdg=heavycode[p1->pid-9990];
//     printf("heavy %d = %d\n",p1->pid,pdg);
//   }
  if(p1->pid>9990){
    beta=heavyMass[p1->pid-9990];
  }else beta=bd->GetParticle(pdg)->Mass();
    beta=beta/p1->p;
  beta=1./sqrt(beta*beta+1.);///velocity
  if(p1->pid<9990){
    if(bd->GetParticle(pdg)->Charge()==0)return(pdg==22)?-2:0;
//     if(abs(pdg)==13) return (p1->p>0.03)?16:6;
  }
//   printf("beta=%2.4f\n",beta);
  if(beta<0.25) ///black track
  return _cw[0];
  else if(beta>0.8) ///MIP
  return _cw[1];
  else return round(_cw[0]-(beta-0.25)*(_cw[0]-_cw[1])/0.55);
  return 0;
}
///---------------------------------------------------------------------------
EdbSegP* FlReader::MakeSeg(point *p01,point *p02,bool mt=false){
    assert(is_compat(*p01,*p02));
    Say(Form("making %s for trk#%d (%d)\n",mt?"MICROTRACK":"BASETRACK",p01->n, p01->pid),3);
    point *p1,*p2;
    if(p02->z>p01->z){p1=p01; p2=p02;}
    else{p1=p02; p2=p01;}
    char w=calcW(p1); if(w<=0)return 0;
    double dz=p2->z-p1->z;
     if(dz==0) return 0;
    double tx=(p2->x-p1->x)/dz;
    double ty=(p2->y-p1->y)/dz;
    if(fabs(tx)>50||fabs(ty)>50)return 0;
//     EdbSegP* seg=new EdbSegP(p1->n,mt?p1->x:(p1->x+p2->x)/2.,mt?p1->y:(p1->y+p2->y)/2.,tx,ty);
//     seg->SetZ(mt?p1->z:p1->z+dz/2.);
    EdbSegP* seg=new EdbSegP(p1->n,(p1->x+p2->x)/2,(p1->y+p2->y)/2,tx,ty);
    seg->SetZ((p1->z+p2->z)/2);
    seg->SetDZ(dz);
    seg->SetDZem(mt?45:90);
    seg->SetMC(fEvt->n,p1->n);
    seg->SetPID(p1->vid-1);
    seg->SetPlate(p1->vid-1);
    seg->SetP(p1->p);
//     seg->SetFlag(mt?2:1);
    seg->SetFlag(p1->pid);
    //if(dz<100)w=w/2;
    seg->SetW(mt?0.5*w:w);
//     printf("pid=%d p=%2.2g w=%d\n",p1->pid,p1->p,w);
    Say(Form("add seg W=%2.1f to plate#%d\n",seg->W(),seg->PID()),3);
    Say("Done\n",3);
    return seg;
};
///---------------------------------------------------------------------------
EdbPattern* NewPatt(double* z, float plate, float side){
  EdbPattern* pat=new EdbPattern;
  pat->SetScanID(EdbID(0,plate,0,0));
  pat->SetPID(plate*10+side);
  pat->SetID((side==0)?plate:plate*2+side-1);
  int ind=1;
  if(side==1)ind=0; ///for BOT - take min z.
  if(side==2)ind=2; ///form TOP - take max z.
  
  pat->SetZ((z[ind]+z[ind+1])/2.);
//   pat->SetDZ(z[ind+1]-z[ind]);
  pat->SetSide(side);
  printf("created pat (%d.%d) at z=%2.4f\n",pat->Plate(),pat->Side(),pat->Z());
  return pat;
}
///---------------------------------------------------------------------------
void FlReader::FillBrick(char* geofile){
  if(!fBrick)fBrick=new EdbBrickP;
  Say(Form("Filling BRICK from file\"%s\"\n",geofile),0);
  FILE* ff = fopen(geofile,"r");
  char line[256];
  double x0,x1,y0,y1,z0,z1;
  while(1){
    if(fgets(line,256,ff)==NULL)break;
//     printf("line=%s",line);
      if(sscanf(line,"RPP Brick %lg %lg %lg %lg %lg %lg",&x0,&x1,&y0,&y1,&z0,&z1)>0){
	x0*=1e4; x1*=1e4; y0*=1e4; y1*=1e4; z0*=1e4; z1*=1e4;
        fBrick->SetXY(.5*(x1+x0),.5*(y1+y0));
        fBrick->SetDXDY(.5*(x1-x0),.5*(y1-y0));
	fBrick->SetZlayer(.5*(z1+z0),z0,z1);
      }
  }
  printf("Brick lims=[%2.2f %2.2f] [%2.2f %2.2f] [%2.2f %2.2f]\n",
	 fBrick->Xmin(),fBrick->Xmax(),fBrick->Ymin(),fBrick->Xmax(),fBrick->Zmin(),fBrick->Zmax());
    fclose(ff);
}
///---------------------------------------------------------------------------
void FlReader::FillPatterns(char* geofile){
///
///fill patterns volume with patterns, reading their Z position from fluka geometry file
///
    Say(Form("Filling patterns from file\"%s\"\n",geofile),0);
    FILE* ff = fopen(geofile,"r");
    char line[256];
    double z[4];
    double lim[4];
    int num=-1;
//     int side=0;
//     EdbPattern* pat=0;
    while(1){
        if(fgets(line,256,ff)!=NULL) {
	 printf("line=%s\n",line);
	  if(sscanf(line,"RPP emul_%d %lg %lg %lg %lg %lg %lg",&num,lim,lim+1,lim+2,lim+3,z,z+3)>0)
	    continue;
	  if(sscanf(line,"RPP base_%d %lg %lg %lg %lg %lg %lg",&num,lim,lim+1,lim+2,lim+3,z+1,z+2)>0){
		    for(int i=0;i<4;i++){
		      lim[i]*=1.e4;
		      z[i]*=1.e4;
		    }
		    Say(Form("read TOP #%d: z=%2.4g, dz=%2.4g!\n",num,z[0],z[1]-z[0]),1);
		    Say(Form("read BASE#%d: z=%2.4g, dz=%2.4g!\n",num,z[1],z[2]-z[1]),1);
		    Say(Form("read BOT #%d: z=%2.4g, dz=%2.4g!\n",num,z[2],z[3]-z[2]),1);
		    fPVRaw->AddPattern(NewPatt(z,num,1));
		    fPVRaw->AddPattern(NewPatt(z,num,2));
		    fPVSeg->AddPattern(NewPatt(z,num,0));
		    
		    EdbPlateP* plt=new EdbPlateP;
		    plt->SetID(num);
		    plt->SetZ((z[1]+z[2])/2.);
		    plt->SetXY  ((lim[1]+lim[0])/2.,(lim[3]+lim[2])/2.);
		    plt->SetDXDY((lim[1]-lim[0])/2.,(lim[3]-lim[2])/2.);
		    plt->SetPlateLayout(z[2]-z[1],z[1]-z[0],z[3]-z[2]);
		    plt->SetAffXY(1,0,0,1,-lim[0],-lim[2]);
		    fBrick->AddPlate(plt);
		    Say("-----------plate added-------------------\n",1);
		    
                }
            }
        else break;
    }
    fBrick->SetDXDY((lim[1]-lim[0])/2.,(lim[3]-lim[2])/2.);
    fclose(ff);
}
///--------------------------------------------------------------------------
EdbSegP* FlReader::AddSegToPV(EdbSegP* Seg,int side){
    ///find pattern where this segment is
    assert(Seg);
    assert(side>=0 && side<3);
    Seg->SetSide(side);
//     Seg->SetY(Seg->Y()+Seg->TY()*dz);
//     Seg->SetZ(Seg->Z()+dz);
    if((side==0 && !fDoMakeSeg)||(side>0 && !fDoMakeRaw))return 0;//EdbSegP(Seg->Clone());
    Say("adding segment to pv...\n",3);
    EdbPatternsVolume* pv=side?fPVRaw:fPVSeg;
//     pv->Print();
    EdbPattern* pat=pv->GetPatternByPlate(Seg->Plate(),side);
    assert(pat);
    assert(fabs(Seg->Z()-pat->Z())<1000);
    return pat->AddSegment(*Seg);
}

///---------------------------------------------------------------------------
bool FlReader::GotoEventB(ifstream* is,int evt_num){
    Say(Form("Skipping to evt#%d\n",evt_num),1);
//     printf("fEvt=0x%x\n",fEvt);
    if(fEvt)Say(Form("current evt is #%d",fEvt->n),2);
    if(fEvt && fEvt->n>evt_num){
        Say("rewinding file pointer\n",2);
        is->seekg (0);
    }
    record rec;
    rec.desc=-1;
    while (READ_BINREC(is,&rec)) {
        if(rec.desc==0){
            Say(Form("read EVT %d\n",rec.num),2);
        }
        else continue;
        if(fEvt && fEvt->n==evt_num){
            Say(Form("Event #%d found:\n",fEvt->n),1);
            return true;
         }
    }
    Say(Form("oh no! end of file!Event %d not found (last event is #%d)\n",evt_num,fEvt?fEvt->n:-999),1);
    return false;
}

///---------------------------------------------------------------------------
EdbTrackP* FlReader::FindTrack (int id){
    if(!fTracks)return 0;
    for(int i=0;i<fTracks->GetEntries();i++){
        EdbTrackP* trk=(EdbTrackP*)fTracks->At(i);
	if(trk->MCTrack()==id && trk->MCEvt()==fEvt->n)return trk;
    }
    return 0;
};
///---------------------------------------------------------------------------
EdbVertex* FlReader::FindVertex(int id){
  if(id==-1)return GetVertex(0);
  if(!fVertices)return 0;
  return FindTrack(id)->VertexE();
    
//     for(int i=0;i<fVertices->GetEntries();i++){
//         EdbVertex* v=(EdbVertex*)fVertices->At(i);
//         if(v->ID()==id && v->MCEvt()==fEvt->n)return v;
//     }
    return 0;
};
///---------------------------------------------------------------------------
EdbTrackP* FlReader::AddTrack(record* rec){
    ///first check if track exsists:
    EdbTrackP* trk=FindTrack(rec->num);
    if(trk){
      Say(Form("Track #%d:\tpdg=%d already exsists!\n",trk->ID(),trk->PDG()),3);
      if(trk->VertexE()){
          //trk->VertexE()->SetFlag();
//           printf("detach it from its end vertex #%d (flg %d)!\n",trk->VertexE()->ID(),trk->VertexE()->Flag());
          EdbVTA* vta=trk->VTAE();
// 	  printf("vta_zpos=%d\n",vta->Zpos());
//           trk->VertexE()->RemoveVTA(vta);
//           trk->ClearVTA(vta);
// 	  printf("starting v#%d (%d) has %d trx\n",trk->VertexS()->ID(),trk->VertexS()->Flag(),trk->VertexS()->N());
          delete vta;
        }
        
        return trk;
    }
    ///create track
    trk=new EdbTrackP();
    if(fTracks)fTracks->Add(trk);
    trk->SetPDG(rec->pid);
    trk->SetP (rec->P);
    trk->SetTX(rec->tx);
    trk->SetTY(rec->ty);
    trk->SetX (rec->x*1e4);
    trk->SetY (rec->y*1e4);
    trk->SetZ (rec->z*1e4);
    trk->SetMC(fEvt->n,rec->num);
    trk->SetID(rec->num);
    double mass=0;
    mass=(trk->PDG()>9990)?heavyMass[trk->PDG()-9990]:bd->GetParticle(trk->PDG())->Mass();
    trk->SetM(mass);
    if(rec->desc==0){
        trk->SetID(0);
        trk->SetMC(fEvt->n,0);
    }
    Say(Form("Add Track #%d:\tpdg=%d\t parent=%d\tP=%g\ttx,ty=[%g %g]\tx,y,z=[%g %g %g]\n",
     trk->ID(),trk->PDG(),rec->par,trk->P(),trk->TX(),trk->TY(),trk->X(),trk->Y(),trk->Z()),2);
    ///attach it to vertex
    if(rec->desc==0 || (!fDoMakeVtx) )return trk;
    if(rec->par==0)rec->par=-1;
    EdbVertex* vtx=fVtxCurr;
    if(!vtx||vtx->ID()!=rec->par)
//     if(!vtx)
//         vtx=FindVertex(rec->par);
    if(!vtx){
      vtx=fVtxCurr;
      if(vtx) Say(Form("No VTX #%d! Adding to the vtx #%d\n",rec->par,vtx->ID()),1);
      else{
	Say(Form("No VTX #%d! Create it!\n",rec->par),1);
        record rec1=*rec;
        rec1.pid=0;
        rec1.num=rec->par;
        rec1.par=0;
        vtx=MakeVertex(&rec1,0);
	fVtxCurr=vtx;
      }
    }
    EdbVTA* vta=new EdbVTA(trk,vtx);
    vta->SetZpos(1);
    vta->SetFlag(2);
    vta->AddVandT();
    Say(Form("track attached, vertex #%d now has %d tracks\n",vtx->ID(),vtx->N()),2);
    return trk;
}
///---------------------------------------------------------------------------
void FlReader::AddVertex(){
  if(fVtxCurr==0)return;
  if(fVtxCurr->N()==0){
    Say(Form("discard vts #%d (flg=%d)\n",fVtxCurr->ID(),fVtxCurr->Flag()),2);
    delete fVtxCurr;
    fVtxCurr=0;
    return;
  }
  if(fVtxCurr->Flag()>100){
    Say(Form("skip vts #%d (flg=%d) with %d trx\n",fVtxCurr->ID(),fVtxCurr->Flag(), fVtxCurr->N()),2);
    return;
  }
  fVtxCurr->SetFlag(-fVtxCurr->Flag());
  fVertices->Add(fVtxCurr);
  Say(Form("Vertex #%d with %d trx added\n",fVtxCurr->ID(),fVtxCurr->N()),3);
  Say(Form("Now we have %d vertices\n",fVertices->GetEntries()),3);
  fVtxCurr=0;
}
///---------------------------------------------------------------------------
EdbVertex* FlReader::MakeVertex(record* rec,EdbTrackP* trk=0){
  if(trk){
    double dx=rec->x*1e4-trk->VertexS()->X();
    double dy=rec->y*1e4-trk->VertexS()->Y();
    double dz=rec->z*1e4-trk->VertexS()->Z();
    double L2=dx*dx+dy*dy+dz*dz;
    Say(Form("Trk #%d (%d) Len=%2.4f\n",trk->ID(),trk->PDG(),L2),2);
    if(L2<fLenMin*fLenMin){
      Say(Form("Merge vertex with previous (flg=%d), len2=%2.3f!\n",rec->par,L2),1);
      fVtxCurr=trk->VertexS();
      if(rec->par==102 || rec->par==101){
	if(fTracks){
	  fTracks->Remove(trk);
	  fTracks->Compress();
	}
	EdbVTA* vta=trk->VTAS();
	delete vta;
	delete trk;
      }
      return fVtxCurr;
    }
  }
  AddVertex();
  ///create vertex
  EdbVertex* v=new EdbVertex();
  v->SetXYZ(rec->x*1e4,rec->y*1e4,rec->z*1e4);
  //     printf("par=%d\n",rec->par);
  if(rec->par==0)v->SetID(-1);
  else v->SetID(-2);
  
  v->SetFlag(-rec->par);
  ///attach it as track end point
  if(trk!=0){
    EdbVTA* vta=new EdbVTA(trk,v);
    vta->SetZpos(0);
    vta->SetFlag(2);
    vta->AddVandT();
    v->SetID(trk->MCTrack());
    v->SetMC(fEvt->n);
  }
  Say(Form("Made VTX #%d:\tpdg=%d\tx,y,z=[%g %g %g]\n",v->ID(),rec->par,rec->x,rec->y,rec->z),2);
  fVtxCurr=v;
  return v;
}
///---------------------------------------------------------------------------
int FlReader::FindSide(EdbSegP* seg){
  assert(seg);
  Say(Form("seg [%2.2f %2.2f %2.2f] in plate#%d\n",seg->X(),seg->Y(),seg->Z(),seg->PID()),5);
  EdbPlateP* plt=fBrick->GetPlate(seg->PID()); ///assume plate position from segPID
  bool found=true;
  double c[3]={seg->X(),seg->Y(),seg->Z()};
  if(!plt->IsInside(c[0],c[1],c[2])){
    found=false;
    ///or search in all plates:
    for(int np=0;np<fBrick->Npl();++np){
      plt=fBrick->GetPlate(np);
      Say(Form("CHK: plate #%d - z=%2.2f\n",plt->ID(),plt->Z()),5);
      if(plt->IsInside(c[0],c[1],c[2])){found=true; break;}
    }
  }
  if(!found)return -1;
  c[2]-=plt->Z();
  int layer=-2;
  EdbLayer* lyr=0;
  for(int nl=0;nl<3;nl++){
    lyr=plt->GetLayer(nl);
    //lyr->Print();
    if(lyr->IsInside(c[0],c[1],c[2])){layer=nl;break;}
  }
  Say(Form("Seg_z=%2.1f is in plate#%d layer %d (id=%d)\n",seg->Z(),seg->PID(),layer,lyr->ID()),5);
  return layer;
}
///---------------------------------------------------------------------------
bool FlReader::ProcessEventB(ifstream* is,bool MakeBtr){
  if(!bd)bd=TDatabasePDG::Instance();
  Say(Form("ProcessEventB #%d making %stracks\n",fEvt?fEvt->n:-1,MakeBtr?"BASE":"MICRO"),1);
  point p1,p2;
  record rec;
  EdbSegP* Seg=0;
  DumpEvt(2);
  EdbTrackP* trk=0;
  fVtxCurr = 0;
  Say("Clearing all...\n",3);
  UnGetN(is,sizeof(rec)+8);
  bool inside=false;
  std::map <int,point> PntMap;
  
  while(READ_BINREC(is,&rec)){
    if(is->eof())break;
    Seg=0;
    if(fVerbose>3)rec.print();
    switch(rec.desc){
      case 0: ///event header
	if(inside){
	  if(fVerbose>=2)printf("----------event end reached------------\n");
	  AddVertex();
// 	  printf("THere are %d vertices and %d trx in total\n",fVertices->GetEntries(),fTracks->GetEntries());
// 	  printf("in first vtx %d trx\n",GetVertex(0)->N());
	  return true;
	}
	inside=true;
	DumpEvt(2);
// 	if(fDoMakeVtx){
// 	  vtx=AddVertex(&rec,0);
// 	  vtx->SetID(-1);
// 	}
	break;
      case 1: ///IN track
	if(!fDoMakeTrx)break;
	trk=FindTrack(rec.num);
// 	if(!trk){Say(Form("OHNO! Cannot find track#%d\n",rec.num),1); rec.print();}
	if(fDoMakeVtx) MakeVertex(&rec,trk);
	break;
      case 2: ///OUT track
	if(!fDoMakeTrx)break;
	trk=AddTrack(&rec);
	break;
      case 3: ///em IN
      case 4: ///em OUT
        if(!fDoReadSeg)break;
// 	if(rec.n==0)rec.n=1;
	p2.Set(rec);
	p1=PntMap[rec.num];
	Seg=0;
	if(!is_compat(p1,p2)){
	  ///new trk start
	  PntMap[rec.num]=p2;
	  continue;
	}
	Seg=MakeSeg(&p1,&p2,(abs(p2.z-p1.z)<60));
	PntMap[rec.num]=p2;
	break;	    
      default:
	Say(Form("WRONG descriptor: %d !!!\n",rec.desc),0);
	rec.print();
	break;
    };
    
    if(Seg){
      
      Say("adding seg to PV\n",3);
      int side=FindSide(Seg);
      if(side==1)Seg->PropagateTo(Seg->Z()+Seg->DZ()/2);
      if(side==2)Seg->PropagateTo(Seg->Z()-Seg->DZ()/2);
      Say(Form("side=%d\n",side),4);
      if(side>=0){
	EdbSegP* sg1=AddSegToPV(Seg,side);
	if(sg1!=0){
	if(fDoMakeTrx){
	  Say(Form("adding seg to trk#%d\n",Seg->MCTrack()),3);
	  trk=FindTrack(Seg->MCTrack());
	  if(trk){
	    Say("track found!\n",4);
// 	    if(!(((trk->PDG()==22 || abs(trk->PDG())==11) && (abs(p2.pid)==11)) || (trk->PDG()==2112 && p2.pid==2212)))
// 	      assert(trk->PDG()==p2.pid);
	  //                         if(fVerbose>3)trk->PrintNice();
	    trk->AddSegment(sg1);
	  }
	}
	}
      }
      delete Seg;Seg=0;
    }
  }
  return false;
}
///---------------------------------------------------------------------------


