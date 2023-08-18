#include <TH3D.h>
#include <TCanvas.h>
#include "TFile.h"
#include "TColor.h"
#include "TF3.h"
#include "TGraph2D.h"
#include "TPaletteAxis.h"
#include "TStyle.h"

#include <string>
#include <iostream>

void PlotItThen(TString fname){

  TFile *f = TFile::Open(fname);

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,800);
  c1->SetRightMargin(0.15);
  TCanvas* c2 = new TCanvas("c2", "c2", 1000,800);
  c2->SetRightMargin(0.15);

  TH3D* h_t = (TH3D*)f->Get("EmissionT")->Clone();
  h_t->SetName("h_t");
  h_t->SetTitle("h_t");
  TH3D* h_llh = (TH3D*)f->Get("Overlap")->Clone();
  h_t->SetName("h_llh");
  h_t->SetTitle("h_llh");

  std::cout << h_llh->Integral() << " " << h_t->Integral() << " " << h_llh->GetBinContent(16,16,16) << std::endl;

  TH3D* new_t = new TH3D("new_t", "new_t", 120, -6000, 6000, 120, -6000, 6000, 120, -6000, 6000);
  TH3D* new_llh = new TH3D("new_llh", "new_llh", 120, -6000, 6000, 120, -6000, 6000, 120, -6000, 6000);

  for(int i=0; i<=new_t->GetXaxis()->GetNbins(); i++){
    for(int j=0; j<=new_t->GetYaxis()->GetNbins(); j++){
      for(int k=0; k<=new_t->GetZaxis()->GetNbins(); k++){
	      //	new_llh->SetBinContent(i,j,k,0);
      }
    }
  }
  
  for(int i=0; i<12; i++){
    for(int j=0; j<12; j++){
      for(int k=0; k<12; k++){
	
	      double bestllh = 0;
	      for(int x=0; x<10; x++){
	        for(int y=0; y<10; y++){
	          for(int z=0; z<10; z++){
	      
	            double llh = h_llh->GetBinContent( i*10+x, j*10+y, k*10+z );
	            if( llh > bestllh)
		            bestllh = llh;
	            }
	          }
	        }

	      for(int x=0; x<10; x++){
          for(int y=0; y<10; y++){
            for(int z=0; z<10; z++){

              double llh = h_llh->GetBinContent( i*10+x, j*10+y, k*10+z );
	            double t = h_t->GetBinContent( i*10+x, j*10+y, k*10+z );
	            if( llh > 0.9*bestllh ){
		            new_llh->SetBinContent( i*10+x, j*10+y, k*10+z, llh );
		            new_t->SetBinContent( i*10+x, j*10+y, k*10+z, t );
	            }
	      
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
  //new_llh->SetMinimum(800);
  gPad->Update();
  TPaletteAxis *palette = (TPaletteAxis*)new_llh->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.88);
  palette->SetX2NDC(0.93);
  palette->SetY1NDC(0.22);
  palette->SetY2NDC(0.82);
  gPad->Update();
  g->Draw("AP SAME");
  //new_llh->Draw("box2Z same");

  c2->cd();
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
  //new_t->SetMinimum(270);
  gPad->Update();
  palette = (TPaletteAxis*)new_t->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.88);
  palette->SetX2NDC(0.93);
  palette->SetY1NDC(0.22);
  palette->SetY2NDC(0.82);
  gPad->Update();
  g->Draw("AP SAME");
  //new_t->Draw("box2Z same");
  
  //gPad->Update();
  TFile fout("test.root","RECREATE");
  new_t->SetLineWidth(0);
  new_llh->SetLineWidth(0);
  new_t->Write("new_t");
  new_llh->Write("new_llh");
  c1->Write("llh_canv");
  //c2->Write("t_canv");

}


void StraightPlot(TString fname){

  TFile *f = TFile::Open(fname);

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,800);
  c1->SetRightMargin(0.15);
  TCanvas* c2 = new TCanvas("c2", "c2", 1000,800);
  c2->SetRightMargin(0.15);

  TH3D* h_t = (TH3D*)f->Get("EmissionT")->Clone();
  h_t->SetName("h_t");
  h_t->SetTitle("h_t");
  TH3D* h_llh = (TH3D*)f->Get("Overlap")->Clone();
  h_t->SetName("h_llh");
  h_t->SetTitle("h_llh");

  TH3D* new_t = (TH3D*)f->Get("EmissionT")->Clone("new_t");//new TH3D("new_t", "new_t", 238, -5975, 5975, 238, -5975, 5975, 238, -5975, 5975);
  TH3D* new_llh = (TH3D*)f->Get("Overlap")->Clone("new_llh");//new TH3D("new_llh", "new_llh", 238, -5975, 5975, 238, -5975, 5975, 238, -5975, 5975);

  new_t->Reset();
  new_llh->Reset();

  //std::vector< int > tvec(100,0);
  //std::vector< int > tx(100,-999);
  //std::vector< int > ty(100,-999);
  //std::vector< int > tz(100,-999);

  for(int i=0; i<=new_t->GetXaxis()->GetNbins(); i++){
    for(int j=0; j<=new_t->GetYaxis()->GetNbins(); j++){
      for(int k=0; k<=new_t->GetZaxis()->GetNbins(); k++){
   
        //double t = h_t->GetBinContent(i,j,k);
        //int tbin = floor(t) - 275;
        //if(tbin < 0) {tbin = 0;}
        //if(tbin>0)
          //std::cout << tbin << " " << tvec.at(tbin) << " " << h_llh->GetBinContent(i,j,k) << std::endl;
       	if( h_llh->GetBinContent(i,j,k)>0){// && h_llh->GetBinContent(i,j,k) > tvec.at(tbin) ){
	        new_llh->SetBinContent(i,j,k, h_llh->GetBinContent(i,j,k));
	        new_t->SetBinContent(i,j,k, h_t->GetBinContent(i,j,k));
          /*
          if(tvec.at(tbin) > 0){
            new_llh->SetBinContent(tx.at(tbin),ty.at(tbin),tz.at(tbin),0);
	          new_t->SetBinContent(tx.at(tbin),ty.at(tbin),tz.at(tbin),0);
          }

          tvec.at(tbin) = h_llh->GetBinContent(i,j,k);
          tx.at(tbin) = i;
          ty.at(tbin) = j;
          tz.at(tbin) = k;*/
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
  new_llh->SetMinimum(0);
  gPad->Update();
  TPaletteAxis *palette = (TPaletteAxis*)new_llh->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.88);
  palette->SetX2NDC(0.93);
  palette->SetY1NDC(0.22);
  palette->SetY2NDC(0.82);
  gPad->Update();
  new_llh->Draw("box2z");
  g->Draw("AP SAME");

  c2->cd();
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
  new_t->SetMinimum(250);
  gPad->Update();
  palette = (TPaletteAxis*)new_t->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.88);
  palette->SetX2NDC(0.93);
  palette->SetY1NDC(0.22);
  palette->SetY2NDC(0.82);
  gPad->Update();
  new_t->Draw("box2z");
  g->Draw("AP SAME");

  gPad->Update();
  TFile fout("outputconv90.root","RECREATE");
  new_t->SetLineWidth(0);
  new_llh->SetLineWidth(0);
  new_t->Write("new_t");
  new_llh->Write("new_llh");
  c1->Write("llh_canv");
  c2->Write("t_canv");

}
