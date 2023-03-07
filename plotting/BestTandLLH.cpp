#include <TH3D.h>
#include <TCanvas.h>
#include "TFile.h"
#include "TColor.h"
#include "TPaletteAxis.h"

#include <string>
#include <iostream>

void BestTandLLH(TString fname){

  TFile *f = TFile::Open(fname);

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,800);
  c1->SetRightMargin(0.15);
  TCanvas* c2 = new TCanvas("c2", "c2", 1000,800);
  c2->SetRightMargin(0.15);

  TH3* h_t;
  TH3* h_llh;
  int flag = 0;
  for(int i=0; i<190; i++){

    //int t = i-40;
    double t = 276.524 - 9 + 0.3*i;
    //double t = 255.837 - 9 + 0.3*i;
    //double n = 236.524347 + 80*i;
    //double n = 3180.681152 + 80*i;

    TString hname = Form("h_%d", i);
    std::cout << "getting " << hname << std::endl;

    TH3D* h = (TH3D*)f->Get(hname)->Clone();
    std::cout << "got " << hname << std::endl;
    if(flag==0){
      h_t = (TH3D*)f->Get(hname)->Clone();
      h_t->SetName("h_t");
      h_t->SetTitle("Emission Time");
      h_t->Reset();
      h_llh = (TH3D*)f->Get(hname)->Clone();
      h_llh->Reset();
      h_llh->SetName("h_llh");
      h_llh->SetTitle("");
      std::cout << "done 0 " << std::endl;
      flag = 1;
    }

    double max = h->GetMaximum();

    for(int j=0; j<h->GetXaxis()->GetNbins(); j++){
      for(int k=0; k<h->GetYaxis()->GetNbins(); k++){
      	for(int l=0; l<h->GetZaxis()->GetNbins(); l++){
	        if(h->GetBinContent(j, k, l) > h_llh->GetBinContent(j, k, l)){
	          if(h->GetBinContent(j, k, l) > 0.9*max){
              h_llh->SetBinContent(j, k, l, h->GetBinContent(j,k,l));
	            h_t->SetBinContent(j, k, l, t);
            }
	        }
	      }
      }
    }    
  }

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

  //TColor::InvertPalette();
  c1->cd();
  h_t->Draw("box2Z");
  gPad->Update();
  TPaletteAxis *palette = (TPaletteAxis*)h_t->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.88);
  palette->SetX2NDC(0.93);
  palette->SetY1NDC(0.22);
  palette->SetY2NDC(0.82);
  gPad->Update();
  h_t->SetMinimum(250);
  h_t->Draw("box2Z");
  g->Draw("AP same");

  c2->cd();
  h_llh->Draw("box2Z");
  gPad->Update();
  palette = (TPaletteAxis*)h_llh->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.88);
  palette->SetX2NDC(0.93);
  palette->SetY1NDC(0.22);
  palette->SetY2NDC(0.82);
  gPad->Update();

  h_llh->Draw("box2Z");
  g->Draw("AP same");

  TFile fout("output1mu.root","RECREATE");
  h_t->Write("h_t");
  h_llh->Write("h_llh");
  c1->Write("c_t");
  c2->Write("c_llh");
  
}
