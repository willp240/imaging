#include <TH3D.h>
#include <TCanvas.h>
#include "TFile.h"

#include <string>
#include <iostream>

void PlotAll(TString fname){

  TFile *f = TFile::Open(fname);

  TCanvas* c1 = new TCanvas("c1", "c1", 1600,800);
  c1->Divide(6,5);

  double maxint = -999;
  int maxt;
  
  for(int i=0; i<30; i++){

    int t = i-15;

    TString hname = Form("h_%d", t);
    std::cout << "getting " << hname << std::endl;

    TH3D* h = (TH3D*)f->Get(hname)->Clone();

    double intgrl = h->Integral();
    if(intgrl > maxint){
      maxint = intgrl;
      maxt = t;
    }
    TString htitle = Form("Fit Time + %d ns, Integral = %f", t, intgrl);
    h->SetTitle(htitle);
    
    c1->cd(i+1);

    h->Draw("box2Z");
  }
  std::cout << "Max integral is " << maxint << " for t " << maxt << std::endl;
}

void Plot1(TString fname, int plotNum){

  TFile *f = TFile::Open(fname);

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetRightMargin(0.13);

  int t = plotNum - 15;

  TString hname = Form("h_%d", plotNum);
  std::cout << "getting " << hname << std::endl;

  TH3D* h = (TH3D*)f->Get(hname)->Clone();

  double intgrl = h->Integral();
  
  TString htitle = Form("Fit Time + %d ns, Integral = %f", t, intgrl);
  h->SetTitle(htitle);

  h->Draw("box2Z");

}
