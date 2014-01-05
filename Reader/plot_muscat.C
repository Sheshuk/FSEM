void plot_muscat(){
  THStack* hstk=new THStack;
  int Nbins=100;
  float MaxSlope=500;
  gStyle->SetHistLineWidth(2);
  TH1F* h0=new TH1F("hBS","Base",Nbins,0,MaxSlope);
  h0->SetLineColor(kBlue);
  TH1F* h1=new TH1F("hEM","Base",Nbins,0,MaxSlope);
  h1->SetLineColor(kRed);
  TH1F* h2=new TH1F("hPB","Base",Nbins,0,MaxSlope);
  h2->SetLineColor(kBlack);
  MuScat->Project("hBS","Kink","flag==0");
  MuScat->Project("hEM","Kink","flag==1");
  MuScat->Project("hPB","Kink","flag==2");
  hstk->Add(h0);
  hstk->Add(h1);
  hstk->Add(h2);
  hstk->Draw("nostack");
}
