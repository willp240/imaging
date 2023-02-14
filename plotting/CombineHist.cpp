#include <TH3D.h>
#include <TCanvas.h>
#include "TFile.h"
#include "TColor.h"
#include "TF3.h"

#include <string>
#include <iostream>

void CombineHist(TString fname1, TString fname2){

  TFile *f1 = TFile::Open(fname1);

  TH3D* h_t1 = (TH3D*)f1->Get("new_t")->Clone();
  h_t1->SetName("h_t1");
  h_t1->SetTitle("h_t1");

  TFile *f2 = TFile::Open(fname2);
  TH3D* h_t2 = (TH3D*)f2->Get("new_t")->Clone();
  h_t2->SetName("h_t2");
  h_t2->SetTitle("h_t2");

  h_t1->Add(h_t2);

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,800);
  c1->SetRightMargin(0.15);

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

  h_t1->Draw("box2Z");
  h_t1->Draw("box2Z");
  h_t1->SetTitle("Emission Time, ns");
  h_t1->GetXaxis()->SetTitle("X, mm");
  h_t1->GetXaxis()->SetTitleSize(0.03);
  //h_t1->GetXaxis()->SetTitleFont(42);
  h_t1->GetXaxis()->SetTitleOffset(1.9);
  h_t1->GetXaxis()->SetLabelSize(0.03);
  //h_t1->GetXaxis()->SetLabelFont(42);
  h_t1->GetYaxis()->SetTitle("Y, mm");
  h_t1->GetYaxis()->SetTitleSize(0.03);
  //h_t1->GetYaxis()->SetTitleFont(42);
  h_t1->GetYaxis()->SetTitleOffset(2.5);
  h_t1->GetYaxis()->SetLabelSize(0.03);
  //h_t1->GetYaxis()->SetLabelFont(42);
  h_t1->GetZaxis()->SetTitle("Z, mm");
  h_t1->GetZaxis()->SetTitleSize(0.03);
  //h_t1->GetZaxis()->SetTitleFont(42);
  h_t1->GetZaxis()->SetTitleOffset(1.7);
  h_t1->GetZaxis()->SetLabelSize(0.03);
  //h_t1->GetZaxis()->SetLabelFont(42);
  gPad->Update();
  TPaletteAxis* palette = (TPaletteAxis*)h_t1->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.9);
  palette->SetX2NDC(0.95);
  palette->SetY1NDC(0.2);
  palette->SetY2NDC(0.8);
  gPad->Update();
  g->Draw("AP SAME");

}
