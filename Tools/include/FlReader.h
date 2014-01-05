#ifndef FLREADER__H
#define FLREADER__H
// class EdbID;
// class EdbScanCond;
// class EdbVertex;
#include <fstream>
#include <TDatabasePDG.h>
#include "EdbPattern.h"
#include "EdbVertex.h"
#include "EdbBrick.h"

///read status:
enum { kCom,
        kEVT,
        kIN,
        kOUT,
        kEM0,
        kEM1,
        kNO
};

///source event:
typedef struct{
    int n;
    int pid;
    double p,tx,ty;
    void Dump(){
        printf("Evt (n=%i, pid=%i, p=%g, tx=%g, ty=%g)\n",n,pid,p,tx,ty);
    };
} EvtHdr;

///binary record:
#pragma pack(push, 1)
typedef struct{
  short int desc; 
  int num;
  short int pid,par;
  double P,tx,ty,x,y,z;
  void print(){
    printf("%d) #%d PID=%d par=%d ",desc,num,pid,par);
    printf("P=%g T=(%g %g) [%g %g %g]\n",P,tx,ty,x,y,z);
  };
} record;
#pragma pack(pop)

///point of track:
typedef struct{
    int n;   ///track id
    int pid; ///particle code
    int vid; ///volume id (plate #)
    int par; ///parent track id (par=n=0 for primary, and par=n for EM showers)
    double p; ///momentum
    double x,y,z;     ///point position
    double tx,ty,tz;  ///particle direction
    void Dump(){
        printf("point:\tn=%i, pid=%i, vid=%i p=%g, x=%g y=%g z=%g\t tx=%g ty=%g\n",n,pid,vid,p,x,y,z,tx/tz,ty/tz);
    };
    void Set(record rec){
      n=rec.num;
      pid=rec.pid;
      vid=rec.par;
      par=0;
      p=rec.P;
      tx=rec.tx;    ty=rec.ty;    tz=1;
      x=rec.x*1e4;  y=rec.y*1e4;  z=rec.z*1e4;
    }
} point;


///------------------------------------------------------
///  A class to read OPERA FLUKASim output ASCII files
///  and pass them to FEDRA for reconstruction
///------------------------------------------------------
class FlReader{
    public:
      FlReader():fTracks(0),fVertices(0),fPVRaw(0),fPVSeg(0),fVerbose(0),fEvt(0),fBrick(0),fVtxCurr(0),
                 fDoMakeTrx(1),fDoMakeVtx(1),fDoReadSeg(1),fDoMakeSeg(1),fDoMakeRaw(0),fLenMin(10.)
		 {new TDatabasePDG();};
        virtual ~FlReader(){};
        ///static methods:
        static int FLUKA2PDG(int code); ///convert FLUKA particle code to PDG code
	
        EdbSegP* MakeSeg(point *p1,point *p2,bool);
        void DumpEvt(int vlevel, bool detail=0);
        void Say(TString msg,int vlevel=0);
        EdbSegP* AddSegToPV(EdbSegP* Seg,int side);
	int FindSide(EdbSegP*);
        void FillPatterns(char* geofile);
	void FillBrick(char* geofile);
	int ConvertCode(int code);
        /// clear
        void ClearPV();
	void ClearPV(EdbPatternsVolume* vol);
        ///processing BINARY:
        int READ_BINREC(ifstream* is,record* rec); ///read 1 binary record
        bool GotoEventB(ifstream* is,int evt_num);
        bool ProcessEventB(ifstream* is,bool MakeBtr=true);
        ///managing tracks and vertices:
        EdbTrackP* AddTrack(record*);
        void AddVertex();
	EdbVertex* MakeVertex(record*,EdbTrackP* );
        EdbTrackP* FindTrack (int id);
        EdbVertex* FindVertex(int id);
        void KillEmptyVts();
	int NVertices(){return fVertices->GetEntries();}
	int NTracks(){return fTracks->GetEntries();}
	EdbVertex* GetVertex(int n){return (EdbVertex*)fVertices->At(n);}
	EdbTrackP* GetTrack (int n){return (EdbTrackP*)fTracks->At(n);}
    public:
      TObjArray *fTracks;
      TObjArray *fVertices;
      EdbPatternsVolume* fPVSeg;
      EdbPatternsVolume* fPVRaw;
      EdbBrickP* fBrick; /// geometry layout
      
      EvtHdr* fEvt;
      EdbVertex* fVtxCurr; ///current vertex
      int fVerbose;
//       char fPCodeFlag; ///0 - FLUKA, 1 - Geant3, 2 - Geant4
      bool fDoMakeVtx;
      bool fDoMakeTrx;
      bool fDoReadSeg;
      bool fDoMakeSeg;
      bool fDoMakeRaw;
      double fLenMin;
      
//     ClassDef(FlReader,1);
};
#endif
