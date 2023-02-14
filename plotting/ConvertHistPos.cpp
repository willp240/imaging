#include <TH3D.h>
#include <TCanvas.h>
#include "TFile.h"
#include "TColor.h"
#include "TF3.h"

#include <string>
#include <iostream>

void ConvertHistPos(TString fname){

  TFile *f = TFile::Open(fname);

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,800);
  c1->SetRightMargin(0.15);
  TCanvas* c2 = new TCanvas("c2", "c2", 1000,800);
  c2->SetRightMargin(0.15);

  TH3D* h_t = (TH3D*)f->Get("h_t")->Clone();
  h_t->SetName("h_t");
  h_t->SetTitle("h_t");
  TH3D* h_llh = (TH3D*)f->Get("h_llh")->Clone();
  h_t->SetName("h_llh");
  h_t->SetTitle("h_llh");

  std::cout << h_llh->Integral() << " " << h_t->Integral() << " " << h_llh->GetBinContent(16,16,16) << std::endl;

  TH3D* new_t = new TH3D("new_t", "new_t", 120, -6000, 6000, 120, -6000, 6000, 120, -6000, 6000);
  TH3D* new_llh = new TH3D("new_llh", "new_llh", 120, -6000, 6000, 120, -6000, 6000, 120, -6000, 6000);

  for(int i=0; i<=new_t->GetXaxis()->GetNbins(); i++){
    for(int j=0; j<=new_t->GetYaxis()->GetNbins(); j++){
      for(int k=0; k<=new_t->GetZaxis()->GetNbins(); k++){
	      new_llh->SetBinContent(i,j,k,0);
      }
    }
  }

  for(int cube=0; cube<12; cube++){
    std::cout << cube << std::endl;
    double bestllh = 0;
    for(int i=0; i<10; i++){
      for(int j=0; j<10; j++){
	      for(int k=cube*10; k<=(cube*10+10); k++){
	        double llh = h_llh->GetBinContent(90+(i), 60+(j), (k+1));
	        std::cout << llh << " in " << 90+(i) << " " << 60+(j) << " " << (k+1) << std::endl;
          std::cout << -6000 + (90+(i)) * 100 << " " << -6000 + (60+(j)) * 100 << " " << -6000 + (k+1)*100 << std::endl;
	        if(llh>bestllh)
	          bestllh=llh;
	      }
      }
    }

    std::cout << "max llh is " << bestllh << std::endl;

    for(int i=0; i<10; i++){
      for(int j=0; j<10; j++){
        for(int k=cube*10; k<(cube*10 +10); k++){
	        double llh = h_llh->GetBinContent(90+(i), 60+(j), (k+1));
	        double t = h_t->GetBinContent(90+(i), 60+(j), (k+1));
	        if(llh>bestllh*0.95 && llh>200 ){
            new_llh->SetBinContent(90+(i), 60+(j), k+1, llh);
	          new_t->SetBinContent(90+(i), 60+(j), k+1, t);
	          //std::cout << "Filling " << llh << " in " << 55+i << " " << 55+j << " " << k+1 << std::endl;
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

  //  TColor::InvertPalette();
  c1->cd();
  new_llh->Draw("box2Z");
  new_llh->SetTitle("Density");
  new_llh->GetXaxis()->SetTitle("X, mm");
  new_llh->GetXaxis()->SetTitleSize(0.03);
  //new_llh->GetXaxis()->SetTitleFont(42);
  new_llh->GetXaxis()->SetTitleOffset(1.9);
  new_llh->GetXaxis()->SetLabelSize(0.03);
  //new_llh->GetXaxis()->SetLabelFont(42);
  new_llh->GetYaxis()->SetTitle("Y, mm");
  new_llh->GetYaxis()->SetTitleSize(0.03);
  //new_llh->GetYaxis()->SetTitleFont(42);
  new_llh->GetYaxis()->SetTitleOffset(2.5);
  new_llh->GetYaxis()->SetLabelSize(0.03);
  //new_llh->GetYaxis()->SetLabelFont(42);
  new_llh->GetZaxis()->SetTitle("Z, mm");
  new_llh->GetZaxis()->SetTitleSize(0.03);
  //new_llh->GetZaxis()->SetTitleFont(42);
  new_llh->GetZaxis()->SetTitleOffset(1.7);
  new_llh->GetZaxis()->SetLabelSize(0.03);
  //new_llh->GetZaxis()->SetLabelFont(42);
  gPad->Update();
  TPaletteAxis *palette = (TPaletteAxis*)new_llh->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.9);
  palette->SetX2NDC(0.95);
  palette->SetY1NDC(0.2);
  palette->SetY2NDC(0.8);
  gPad->Update();
  g->Draw("AP SAME");


  c2->cd();
  new_t->Draw("box2Z");
  new_t->Draw("box2Z");
  new_t->SetTitle("Emission Time, ns");
  new_t->GetXaxis()->SetTitle("X, mm");
  new_t->GetXaxis()->SetTitleSize(0.03);
  //new_t->GetXaxis()->SetTitleFont(42);
  new_t->GetXaxis()->SetTitleOffset(1.9);
  new_t->GetXaxis()->SetLabelSize(0.03);
  //new_t->GetXaxis()->SetLabelFont(42);
  new_t->GetYaxis()->SetTitle("Y, mm");
  new_t->GetYaxis()->SetTitleSize(0.03);
  //new_t->GetYaxis()->SetTitleFont(42);
  new_t->GetYaxis()->SetTitleOffset(2.5);
  new_t->GetYaxis()->SetLabelSize(0.03);
  //new_t->GetYaxis()->SetLabelFont(42);
  new_t->GetZaxis()->SetTitle("Z, mm");
  new_t->GetZaxis()->SetTitleSize(0.03);
  //new_t->GetZaxis()->SetTitleFont(42);
  new_t->GetZaxis()->SetTitleOffset(1.7);
  new_t->GetZaxis()->SetLabelSize(0.03);
  //new_t->GetZaxis()->SetLabelFont(42);
  gPad->Update();
  palette = (TPaletteAxis*)new_t->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.9);
  palette->SetX2NDC(0.95);
  palette->SetY1NDC(0.2);
  palette->SetY2NDC(0.8);
  gPad->Update();
  g->Draw("AP SAME");
  
  //gPad->Update();
  TFile fout("outputPos.root","RECREATE");
  new_t->SetLineWidth(0);
  new_llh->SetLineWidth(0);
  new_t->Write("new_t");
  new_llh->Write("new_llh");
  c1->Write("llh_canv");
  c2->Write("t_canv");
}
