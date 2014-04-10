#include <cstdlib>
#include "FlData.h"
#include "FlReaderApp.h"
#include "GeomReader.h"

///root includes
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TVector3.h"
#include "TDatabasePDG.h"

//fedra includes
#include "EdbVertex.h"
#include "EdbPattern.h"
#include "EdbSegP.h"
#include "EdbBrick.h"


#include "NiceLog.h"
#include <vector>

class FlBrickApp: public FlReaderApp{
public:
    virtual void Init();
    virtual void Finish();
    virtual void ProcessEvent();
    //printing
    virtual void HelpMe();

private:
    EdbSegP*   AddSegToFTrk(const FlSeg &iseg, EdbTrackP* otrk);
    EdbTrackP* AddTrkToFVtx(const FlTrk &it, EdbVertex* ov = 0);
    EdbVertex* AddVtxToFTrk(const FlVtx &iv, EdbTrackP* trk = 0);
    void ClearFedra();

    TDatabasePDG PdgDB;
    TFile * OutFile = 0;
    TTree * OutTree = 0;
    TObjArray  *Vertices;
    TObjArray  Tracks;
    TObjArray  Segments;
    GeomReader Geometry;


};

#define _NiceVlev fVerbose

const float heavyMass[7] = {0, 0, 5, 1.9, 2.8, 2.8, 3.7};

///-----------------------------------------------------------------------
void FlBrickApp::Init(){
    FlReaderApp::Init();
    fReader.SetSaveBits("mvt");
    Vertices=new TObjArray();
    OutFile=new TFile(fOutFile,fAppend?"UPDATE":"RECREATE");
    OutFile->cd();
    if(fGeoFile){
        //read geometry from file
        Geometry.ReadGeoFile(fGeoFile);
        Geometry.Brick()->Write("Brick",TObject::kOverwrite);  
    }
    if(fAppend){
        if(fGeoFile==0)Geometry.SetBrick((EdbBrickP*)OutFile->Get("Brick"));
        OutTree=(TTree*)OutFile->Get("FluSim");
        OutTree->SetBranchAddress("Vtx",&Vertices);
    }else{
        OutTree=new TTree("FluSim", "FFSEM to FEDRA");
        OutTree->Branch("Vtx", "TObjArray", &Vertices, 128000, 0);
        OutTree->Write("FluSim",TObject::kOverwrite);
    }
  }
///-----------------------------------------------------------------------
void FlBrickApp::Finish(){
    FlReaderApp::Finish();
    OutTree->Write("FluSim",TObject::kOverwrite);
    OutFile->Close();
    delete Vertices;
  }
///-----------------------------------------------------------------------
void FlBrickApp::HelpMe(){
        printf("This is FlukaBrick application:\n");
        printf("convert FLUKA output to FEDRA vertices tree\n");
        FlReaderApp::HelpMe();
    }
///-----------------------------------------------------------------------
EdbSegP* FlBrickApp::AddSegToFTrk(const FlSeg &iseg, EdbTrackP* otrk) {
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
    Segments.Add(oseg);
    return oseg;
}


///-----------------------------------------------------------------------

EdbTrackP* FlBrickApp::AddTrkToFVtx(const FlTrk &it, EdbVertex *ov) {
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
    ot->SetM((it.pdg > 9990) ? heavyMass[ot->PDG() - 9990] : PdgDB.GetParticle(ot->PDG())->Mass());
    if (ov) {
        ///attach it as track end point
        EdbVTA* vta = new EdbVTA(ot, ov);
        vta->SetZpos(1);
        vta->SetFlag(2);
        vta->AddVandT();
    }
    Tracks.Add(ot);
    return ot;
}
///------------------------------------------------------------------------

EdbVertex* FlBrickApp::AddVtxToFTrk(const FlVtx &iv, EdbTrackP *trk) {
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

void FlBrickApp::ProcessEvent() {
    for(auto&& itr_v : FlVtx::Map) {
        FlVtx* Vtx = itr_v.second;
        if(Vtx->N()==0)continue;
        EdbVertex* fVtx = AddVtxToFTrk(*Vtx, 0);
        Vertices->Add(fVtx);
        for (auto&& Trk : Vtx->tracks) {
            if(Trk->N()==0)continue;
            EdbTrackP* fTrk = AddTrkToFVtx(*Trk, fVtx);
            for (auto&& Seg : Trk->segs)
                AddSegToFTrk(*Seg, fTrk);
        }
    }
    OutTree->Fill();
    ClearFedra();
}
///------------------------------------------------------------------------
void FlBrickApp::ClearFedra(){
    Vertices->Clear("C");
    Tracks.Clear("C");
    Segments.Clear("C");
}
///------------------------------------------------------------------------


int main(int argc, char **argv)
{
    printf("hello!\n");
  FlBrickApp app;
  app.ReadArgs(argc,argv);
  app.Run();
  return 0;
}
