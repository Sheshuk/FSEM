#ifndef _FLDATA__H
#define _FLDATA__H


#include <fstream>
#include <vector>
#include <map>
///read status:
enum {
    kEVT = 0,
    kIN,
    kOUT,
    kEM0,
    kEM1,
    kNO
};

///binary record:
#pragma pack(push, 1)
typedef struct {
    short int desc;
    int num;
    short int pid, par;
    double p, tx, ty, x, y, z;

    void print();
} record;
#pragma pack(pop)

class FlSeg;
class FlTrk;
class FlVtx;

///------------------------------------------------------------

class FlSeg {
public:

    enum {
        kBTK = 0,
        kMTK,
        kPB,
        k__Tot
    };

public:
    static long IDSEG;
    static std::map <long, FlSeg*> Map;
    static void PrintAll(int lev = 0);
public:
    FlSeg(FlTrk* t, record r1, record r2, int Side);
    ~FlSeg();
    void Clear();
    void Print(int lev = 0);
public:
    long id;
    int pdg;
    float x, y, z, dz;
    float p, tx, ty;
    float kink, eloss;
    int trk_id;
    FlTrk* trk;
    char plate, side;
};
///------------------------------------------------------------

class FlTrk {
public:
    static int IDTRK;
    static std::map <int, FlTrk*> Map;
    static void PrintAll(int lev = 0);

public:
    FlTrk(FlVtx* v, record r);
    ~FlTrk();
    void Clear();
    void Print(int lev = 0);
    void AddSegment(FlSeg*);

    int N() {
        return segs.size();
    }

    FlSeg* GetSegment(int n) {
        return segs[n];
    }
public:
    int id, pdg;
    float x, y, z;
    float p, tx, ty;
    FlVtx *vtx0, *vtx1;
    std::vector<FlSeg*> segs;
    record fLastRec;
};
///------------------------------------------------------------

class FlVtx {
public:
    static int IDVTX;
    static std::map <int, FlVtx*> Map;
    static void PrintAll(int lev = 1);
public:
    FlVtx(FlTrk* t, record r);
    ~FlVtx();
    void Clear();
    void Print(int lev = 0);
    void AddTrack(FlTrk*);

    int N() {
        return tracks.size();
    }

    FlTrk* GetTrack(int n) {
        return tracks[n];
    }
public:
    int id, flag;
    float x, y, z;
    float tx, ty, p;
    FlTrk* trk_in;
    std::vector<FlTrk*> tracks;
};


///------------------------------------------------------------

class FlRead {
public:
    FlRead();
    ///clear methods
    void ClearSeg();
    void ClearTrx();
    void ClearVts();

    void ClearAll() {
        ClearVts();
        ClearTrx();
        ClearSeg();
        FlSeg::IDSEG = 0;
        FlTrk::IDTRK = 0;
        FlVtx::IDVTX = 0;
    }

    void PrintEvt();
    void PrintStat();

    void PrintSeg();
    int FindEvent(int EvNum);
    int ReadEvent();
public:
    int Open(char* fname);
    void Close();
    int Next();

public:
    bool fDoSaveVtx;
    bool fDoSaveTrx;
    bool fDoSaveSeg[FlSeg::k__Tot];

    void SetSaveBits(char vtbel) {
        fDoSaveSeg[FlSeg::kPB ] = vtbel & 1;
        fDoSaveSeg[FlSeg::kMTK] = vtbel & 2;
        fDoSaveSeg[FlSeg::kBTK] = vtbel & 4;
        fDoSaveTrx = vtbel & 8;
        fDoSaveVtx = vtbel & 16;
    }

    ///reading file
    int fEvent;
    float fE0;
    char* fFileName;
    long fFilePos;
    long fFileLen;
    int fRecSize;
    //event size estimation
    float f_EvSize;
    std::ifstream fFile;
    ///records
    record fRec1; ///current record
    record fRec0; ///previous EM_cross record
    record fRecE; ///Event hdr record    
    /// current:
    FlTrk *fTrkCur;
    FlVtx *fVtxCur;

    static int FlVerbose;
};
#endif
