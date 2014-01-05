const float Em[]={980, 1273, 2253, 2546};
const float Bs[]={1024,1229, 2297, 2502};

float Ltot=Em[3];
float Lbs=Bs[1]-Bs[0]+Bs[3]-Bs[2];
float Lem=Em[1]-Em[0]+Em[3]-Em[2]-Lbs;
float Lpb=Ltot-Lem-Lbs;

TCut cBs=Form("(Vz>%2.4f && Vz<%2.4f) || (Vz>%2.4f && Vz<%2.4f)",Bs[0],Bs[1],Bs[2],Bs[3]);
TCut cEm0=Form("(Vz>%2.4f && Vz<%2.4f) || (Vz>%2.4f && Vz<%2.4f)",Em[0],Em[1],Em[2],Em[3]);
TCut cEm=cEm0&&(!cBs);
TCut cPb=!cEm0;
TCut ctSide[4]={"1",cBs,cEm,cPb};
TCut ctPmin="(P0>=1 && P0<=15 && code1==13)";
TCut ctPt  ="(P1*sin(Kink*0.001)>0.25)";
TCut ctKink="Kink>20";
TCut cW="1";
long N000=30e6;
long NNN[]={
  93318636 +93319794 +92598937 +66795494 +35380294+22247015,
  186637093+186639448+185197690+133590870+70760526+44493976,
  46661659 +46662267 +46301684 +33399342 +17691006+11124040,
  0
};
float LLL[]={210,45,980,0};
//TChain *MuScat;

char* LName[4]={"all layers","BASE","EMULSION","LEAD"};
const int NM=1e8;

void OutputHist(TH1F* h, char* fname, float Norm){
  FILE* f=fopen(fname,"w");
  fprintf(f,"* total selected = %2.4f from %ld muons\n",Norm,N000);
  fprintf(f,"Lower,\tHigher,\tprobability\n");
  for(int i=1;i<h->GetNbinsX();++i){
    float x0=h->GetBinLowEdge(i)/1000;
    float x1=h->GetBinLowEdge(i+1)/1000;
    float c=h->GetBinContent(i);
    fprintf(f,"%3.2f, %3.2f, %4.6f\n",x0,x1,c);
  }
  fclose(f);
}

void CompareP(TCut cut){
   TCanvas *c=new TCanvas("cP","Momentum spectrum",400,300);
   TH1F* hP0 =new TH1F("hP0","Unweighted; P, GeV",50,1,14);   
   TH1F* hPw=new TH1F("hPw","Weighted; P, GeV",50,1,14);
   
   MuScat->Project("hP0","P0",cut,"",NM);
   MuScat->Project("hPw","P0",cW*cut,"",NM);
   /*TH1F* hP0=(TH1F*)gDirectory->Get("hP0");
   TH1F* hPw=(TH1F*)gDirectory->Get("hPw");*/
   hP0->SetLineStyle(2);
   hPw->SetLineStyle(1);
   hP0->Print();
   hPw->Print();
   THStack* stk=new THStack("hs","Momentum spectrum reweight; P, GeV");
   stk->Add(hP0);
   stk->Add(hPw);
   stk->Draw("nostack");
   c->SetLogy();
   c->Print("P0.eps");
}

TH1F* hK0[4];
TH1F* hKw[4];
TH1F* hPt0[4];
TH1F* hPtw[4];
int Col[4]={kBlack,kBlue,kRed,kGreen+3};

void PlotKink(TCut cut, int side){
   printf("Begin Plot Kink_%d\n",side);
   cout << cut.GetTitle()<<endl;
   TCut cSide="";
   if(side!=-1)cSide=ctSide[side];//Form("flag==%d",side);
   cut=cut&&cSide;
   TString nme=Form("hK_%d",side);
   TH1F* h0=new TH1F(nme+"0","Unweighted",30, 0,600);
   TH1F* hw=new TH1F(nme+"w","Weighted",  30, 0,600);
   hK0[side+1]=h0;
   hKw[side+1]=hw;
   MuScat->Project(h0->GetName(),"Kink",cut,"",NM);
   MuScat->Project(hw->GetName(),"Kink",cW*cut,"",NM);
   
   h0->SetLineColor(Col[side+1]);
   h0->SetFillColor(Col[side+1]);
//    h0->SetLineStyle(2);
   
   hw->SetLineColor(Col[side+1]);
   hw->SetFillColor(Col[side+1]);
   hw->SetLineStyle(1);
   h0->Print();
   hw->Print();
//    TCanvas *c=new TCanvas(Form("cKink_%d",side),"Kink angle",600,300);
//    THStack* stk=new THStack("hsk",Form("Kink angle in %s; #theta_{Kink}, mrad",LName[side+1]));
//    stk->Add(hK0[side+1]);
//    stk->Add(hKw[side+1]);
//    stk->Draw("nostack");
//    c->SetLogy();
//    c->Print(Form("Kink_%d.eps",side));
}

void PlotPt(TCut cut, int side){
  printf("Begin Plot Pt_%d\n",side);
  cout << cut.GetTitle()<<endl;
  if(side!=-1){
    TCut cSide=ctSide[side];//Form("flag==%d",side);
    cut=cut&&cSide;
  }
//   TCanvas *c=new TCanvas(Form("cPt_%d",side),"Transverse momentum",600,300);
  TString nme=Form("hPt_%d",side);
  TH1F* h0=new TH1F(nme+"0","Unweighted",30,0,1500);
  TH1F* hw=new TH1F(nme+"w","Weighted",  30,0,1500);
  hPt0[side+1]=h0;
  hPtw[side+1]=hw;
  MuScat->Project(h0->GetName(),"1e3*P1*sin(Kink*0.001)",cut,"",NM);
  MuScat->Project(hw->GetName(),"1e3*P1*sin(Kink*0.001)",cW*cut,"",NM);
  
  h0->SetLineColor(Col[side+1]);
  h0->SetFillColor(Col[side+1]);
//   h0->SetLineStyle(2);
  
  
  hw->SetLineColor(Col[side+1]);
  hw->SetFillColor(Col[side+1]);
  hw->SetLineStyle(1);
  h0->Print();
  hw->Print();
//   THStack* stk=new THStack("hspt",Form("Transverse momentum in %s; P_{T}, MeV",LName[side+1]));
//   stk->Add(hPt0[side+1]);
//   stk->Add(hPtw[side+1]);
//   stk->Draw("nostack");
//   c->SetLogy();
//   c->Print(Form("Pt_%d.eps",side));
}

void DoMuonAnalysis(bool sig=false){
/*  MuScat=new TChain("MuScat");
  MuScat->Add("Mu1_50m.root");
  MuScat->Add("MuN.root");*/
//   MuScat->Add("MuDirty5.root");
//   MuScat->Add("Mu.msc.root");
  /*MuScat->Add("Mu.msc.1.root");
  MuScat->Add("Mu.msc.2.root");
  MuScat->Add("Mu.msc.3.root");
  MuScat->Add("Mu.msc.4.root");
  MuScat->Add("Mu.msc.5.root");
  MuScat->Add("Mu.msc.6.root");
  MuScat->Add("Mu.msc.7.root");*/
  
  
  gROOT->SetStyle("Plain");
  gStyle->SetHistLineWidth(2);
  gStyle->SetOptStat(0);
  CompareP(ctPmin);
  TCut cCut=ctPmin&&ctKink;
   if(sig)cCut=cCut&&ctPt;
  PlotKink(cCut,-1);
  PlotKink(cCut,0);
  PlotKink(cCut,1);
  PlotKink(cCut,2);
  
  PlotPt(cCut,-1);
  PlotPt(cCut,0);
  PlotPt(cCut,1);
  PlotPt(cCut,2);
  
  float NN1[4]={hKw[1]->Integral(),hKw[2]->Integral(),hKw[3]->Integral(),0};
  
  NNN[3]=NNN[0]+NNN[1]+NNN[2];
  NN1[3]=NN1[0]+NN1[1]+NN1[2];
  
  float Lint[4];
  float Prob[4];
  for(int n=0;n<4;++n){
    int in=(n<3)?(n+1):0;
    printf("NNN[%d]=%d\t NN1[%d]=%g\n",n,NNN[n],n,NN1[n]);
    Prob[n]=NN1[n]/NNN[n];
    Lint[n]=LLL[n]*1e-6/Prob[n];
    printf("hKw_wdt[%d]=%f\n",n,hKw[in]->GetBinWidth(1));
    printf("hPtw_wdt[%d]=%f\n",n,hPtw[in]->GetBinWidth(1));
    
    hKw[in] ->Print();
//     float w=1;
//     if(n==0)w=0.5;
//     else if(n==1)w=0.25;
//     hKw[n]->Scale(w);hPtw[n]->Scale(w);
//     hKw[n] ->Print();
     /*hKw[in] ->Scale(1./(NNN[n]*hKw [n]->GetBinWidth(1)));
     hPtw[in]->Scale(1./(NNN[n]*hPtw[n]->GetBinWidth(1)));*/
     hKw[in]  ->Scale(1./hKw [in]->Integral());
     hPtw[in] ->Scale(1./hPtw [in]->Integral());
     OutputHist(hPtw[in],Form("ptw_%d.hist",n),NN1[n]);
  }
  
  
  printf("No cut       & %12d & %12d & %12d & %12d\n",NNN[3], NNN[0], NNN[1], NNN[2]);
  printf("Kink>20 mrad & %12g & %12g & %12g & %12g\n",NN1[3], NN1[0], NN1[1], NN1[2]);
  printf("Probability  & %12g & %12g & %12g & %12g\n",Prob[3],Prob[0],Prob[1],Prob[2]);
  printf("Int length, m& %12g & %12g & %12g & %12g\n",Lint[3],Lint[0],Lint[1],Lint[2]);
  TCanvas* c=new TCanvas("cK_0","Kink0",800,300);
  THStack* stk=new THStack("hsk_0","Kink angle; #theta_{Kink}, mrad; PDF, mrad^{-1}");
  //   stk->Add(hKw[0]);
  stk->Add(hK0[3]); hK0[3]->SetFillStyle(3002);
  stk->Add(hK0[1]); hK0[1]->SetFillStyle(3001);
  stk->Add(hK0[2]); hK0[2]->SetFillStyle(3002);
  
  stk->Draw("nostack");
  c->SetLogy();
  c->Print("Kink0.eps");

  TCanvas* c=new TCanvas("cK_w","Kink0",800,300);
  THStack* stk=new THStack("hsk_w","Kink angle; #theta_{Kink}, mrad; PDF, mrad^{-1}");
//   stk->Add(hKw[0]);
  stk->Add(hKw[3]); hKw[3]->SetFillStyle(3002);
  stk->Add(hKw[1]); hKw[1]->SetFillStyle(3001);
  stk->Add(hKw[2]); hKw[2]->SetFillStyle(3002);
  
  stk->Draw("nostack");
  c->SetLogy();
  c->Print("KinkW.eps");
  TLegend *leg=new TLegend(0.7,0.7,0.9,0.9); 
  leg->AddEntry(hPtw[3],"Lead","F");
  leg->AddEntry(hPtw[1],"Base","F");
  leg->AddEntry(hPtw[2],"Emulsion","F");
  TCanvas* c=new TCanvas("cPt_0","Pt0",800,300);
  THStack* stk=new THStack("hspt_0","Transverse momentum PDF;P_{T}, MeV;  PDF, MeV^{-1}");
  //   stk->Add(hPtw[0]);
  stk->Add(hPt0[3]); hPt0[3]->SetFillStyle(3002);
  stk->Add(hPt0[1]); hPt0[1]->SetFillStyle(3001);
  stk->Add(hPt0[2]); hPt0[2]->SetFillStyle(3002);
  
  stk->Draw("nostack");
  leg->Draw();
  c->SetLogy();
  c->Print("Pt0.eps");
  TCanvas* c=new TCanvas("cPt_w","Pt0",800,300);
  THStack* stk=new THStack("hspt_w","Transverse momentum PDF;P_{T}, MeV;  PDF, MeV^{-1}");
//   stk->Add(hPtw[0]);
  stk->Add(hPtw[3]); hPtw[3]->SetFillStyle(3002);
  stk->Add(hPtw[1]); hPtw[1]->SetFillStyle(3001);
  stk->Add(hPtw[2]); hPtw[2]->SetFillStyle(3002);
  
  
  stk->Draw("nostack");
  leg->Draw();
  c->SetLogy();
  c->Print("PtW.eps");

}
