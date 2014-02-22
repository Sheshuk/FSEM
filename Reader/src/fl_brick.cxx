#include <cstdlib>
#include "FlData.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TVector3.h"
#include "TDatabasePDG.h"

#include "EdbVertex.h"
#include "EdbPattern.h"
#include "EdbSegP.h"
#include "GeomReader.h"
#include "EdbBrick.h"
#include "NiceLog.h"

#define _NiceVlev FVLEV
int FVLEV = 0;
TDatabasePDG gBD;

TObjArray *gVertices;
GeomReader Geometry;

const float heavyMass[7] = {0, 0, 5, 1.9, 2.8, 2.8, 3.7};


///-----------------------------------------------------------------------

EdbSegP* AddSegToFTrk(FlSeg &iseg, EdbTrackP* otrk) {
    _Log(3, "Adding segment:\n");
    _LogCmd(3, iseg.Print());
    EdbSegP* oseg = new EdbSegP(iseg.id, iseg.x, iseg.y, iseg.tx, iseg.ty, 22, iseg.pdg);
    oseg->SetZ(iseg.z);
    oseg->SetDZ(iseg.dz);
    oseg->SetP(iseg.p);
    oseg->SetPlate(iseg.plate-1);
    oseg->SetPID(iseg.plate-1);
    oseg->SetSide(iseg.side);
    if(iseg.side==FlSeg::kMTK)Geometry.FindSide(oseg);
    oseg->SetMC(1, iseg.trk_id);
    oseg->SetTrack(otrk->ID());
    otrk->AddSegment(oseg);
    _Log(3, "Created segment:\n");
    _LogCmd(3, oseg->PrintNice());
    return oseg;
}


///-----------------------------------------------------------------------

EdbTrackP* AddTrkToFVtx(FlTrk &it, EdbVertex *ov = 0) {
    _Log(3, "Adding track:\n");
    _LogCmd(3, it.Print());
    EdbTrackP* ot = new EdbTrackP;
    ot->SetID(it.id);
    ot->SetPDG(it.pdg);
    ot->SetX(it.x);
    ot->SetY(it.y);
    ot->SetZ(it.z);
    ot->SetP(it.p);
    ot->SetTX(it.tx);
    ot->SetTY(it.ty);
    ot->SetMC(1, it.id);
    ot->SetTrack(it.id);
    ot->SetM((it.pdg > 9990) ? heavyMass[ot->PDG() - 9990] : gBD.GetParticle(ot->PDG())->Mass());
    if (ov) {
        ///attach it as track end point
        EdbVTA* vta = new EdbVTA(ot, ov);
        vta->SetZpos(1);
        vta->SetFlag(2);
        vta->AddVandT();
    }
    return ot;
}
///------------------------------------------------------------------------

EdbVertex* AddVtxToFedraTrk(FlVtx &iv, EdbTrackP *trk = 0) {
    EdbVertex* v = new EdbVertex;
    v->SetXYZ(iv.x, iv.y, iv.z);
    v->SetID(iv.id);
    v->SetFlag(iv.flag);
    if (trk) {
        ///attach it as track end point
        EdbVTA* vta = new EdbVTA(trk, v);
        vta->SetZpos(0);
        vta->SetFlag(2);
        vta->AddVandT();
    }
    return v;
}
///------------------------------------------------------------------------

void ProcessAllToFedra(TObjArray* vts) {
    std::map <int, FlVtx*>::const_iterator itr = FlVtx::Map.begin();
    for (; itr != FlVtx::Map.end(); ++itr) {
        FlVtx *Vtx = itr->second;
        EdbVertex* fVtx = AddVtxToFedraTrk(*Vtx, 0);
        vts->Add(fVtx);
        for (int nt = 0; nt < Vtx->N(); ++nt) {
            FlTrk* Trk = Vtx->GetTrack(nt);
            EdbTrackP* fTrk = AddTrkToFVtx(*Trk, fVtx);
            for (int ns = 0; ns < Trk->N(); ++ns) {
                FlSeg* Seg = Trk->GetSegment(ns);
                //EdbSegP* fSeg=
                AddSegToFTrk(*Seg, fTrk);
//                _Log(0,"Added seg #%d",fTrk->N());
            }
        }
    }
}
///------------------------------------------------------------------------


FlRead reader;

long ReadLong(char* str) {
    char* endp;
    long n = strtol(str, &endp, 10);
    if (endp[0] == 'k')n *= 1000;
    if (endp[0] == 'm')n *= 1000000;
    if (endp[0] == 'g')n *= 1000000000;
    return n;
}

long N0 = 1;
long Ntot = -1;

void helpme() {
    printf("fl_brick - convert from FSEM simulated data to FEDRA vertices\n");
}
///-------------------------------------------------------------------------------

int main(int argc, char** argv) {
    FlRead::FlVerbose = 0;
    char* geofile=0;
    /// read arguments:
    try {
        if (argc < 2){throw 0;}
        for (int n = 1; n < argc - 2; ++n) {
            if (strncmp(argv[n], "-v=", 3) == 0) {
                FlRead::FlVerbose = atoi(argv[n] + 3);
                continue;
            } else if (strncmp(argv[n], "-n=", 3) == 0) {
                Ntot = ReadLong(argv[n] + 3);
                printf("Ntot=%ld\n", Ntot);
                continue;
            } else if (strncmp(argv[n], "-g=", 3) == 0) {
                geofile=argv[n]+3;
                continue;
            } else if (strncmp(argv[n], "-n0=", 4) == 0) {
                N0 = ReadLong(argv[n] + 4);
                printf("N0=%ld\n", N0);
                continue;
            }
            else throw n;
        }
    } catch(int ner){
        if(ner==0){
            helpme();
            return 1;
        }
        printf("Error in argument #%d = \'%s\'\n Now exit",ner,argv[ner]);
        return 1;
    }
    /// done reading arguments

    reader.fDoSaveSeg[FlSeg::kBTK] = 0;
    reader.fDoSaveSeg[FlSeg::kMTK] = 1;
    reader.fDoSaveSeg[FlSeg::kPB] = 0;
    reader.fDoSaveTrx = 1;
    reader.fDoSaveVtx = 1;
    char* ifname = argv[argc - 2];
    char* ofname = argv[argc - 1];
    printf("Input file = \'%s\'\n", ifname);
    printf("Output file = \'%s\'\n", ofname);
    //   printf("Verbose=%d\n",FlRead::FlVerbose);
    if (reader.Open(ifname) == 0) {
        printf("ERROR opening file %s\n", ifname);
        return 1;
    }

    gVertices = new TObjArray;
    

    TFile outf(ofname, "RECREATE");
    outf.cd();
    Geometry.ReadGeoFile(geofile);
    if(geofile) Geometry.Brick()->Write("Brick",TObject::kOverwrite);
    TTree* tri = new TTree("FluSim", "FlukaSIM output");
    tri->Branch("Vtx", "TObjArray", &gVertices, 128000, 0);
    tri->Write("FluSim", TObject::kOverwrite);

    long Nev = 0;
//    reader.SetSaveBits(63);
    if (reader.FindEvent(N0) == false)return 1;
    while (reader.ReadEvent()) {
        if (reader.fEvent >= N0) {
            Nev++;
            if (Nev % 1000 == 0) {
                reader.PrintEvt();
                Nev = 0;
            }
            ProcessAllToFedra(gVertices);
            tri->Fill();
            reader.ClearAll();
            gVertices->Clear("C");
        }
        if (reader.fEvent > N0 + Ntot)break;
    }
    //   reader.PrintStat();
    //   FlVtx::PrintAll(2);
    //   reader.ClearAll();
    tri->Write("FluSim", TObject::kOverwrite);
    reader.Close();

    outf.Close();
    return 0;
}
