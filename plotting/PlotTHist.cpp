#include <TH3D.h>
#include <TCanvas.h>
#include "TFile.h"
#include "TColor.h"

#include <string>
#include <iostream>

void PlotTHist(TString fname){

  TFile *f = TFile::Open(fname);

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,800);
  c1->SetRightMargin(0.12);

  c1->Print("test2.pdf[");

  TColor::InvertPalette();

  TGraph2D* g = new TGraph2D(10000);
  int point = 0;
  for(double th=0; th<2*TMath::Pi(); th+=2*TMath::Pi()/50){
    for(double phi=0; phi<TMath::Pi(); phi+=TMath::Pi()/50){
      double x = 6000*TMath::Sin(th)*TMath::Cos(phi);
      double y = 6000*TMath::Sin(th)*TMath::Sin(phi);
      double z = 6000*TMath::Cos(th);
      g->SetPoint(point, x, y, z);
      point++;
    }
  }

  for(int i=0; i<100; i++){

    int t = i-50;

    TString hname = Form("h_%d", i);
    std::cout << "getting " << hname << std::endl;

    TH3D* h = (TH3D*)f->Get(hname)->Clone();

    double emitT = 276.524+t;
    TString htitle = Form("EmissionTime_%fns", emitT);

    std::cout << "got " << hname << std::endl;
    h->SetName(htitle);
    h->SetTitle(htitle);
    h->Draw("box2z");
    g->Draw("AP same");
    c1->Print("test2.pdf");
  }
  
  c1->Print("test2.pdf]");
}
