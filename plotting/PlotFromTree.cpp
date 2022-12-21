#include <TH3D.h>
#include <TCanvas.h>
#include "TFile.h"
#include "TColor.h"
#include "TTree.h"

#include <string>
#include <iostream>
#include <sstream>

void PlotFromTree(TString fname){

  TFile *f = TFile::Open(fname);

  /// Load in the tree
  TTree* chain = (TTree*)f->Get("posteriors")->Clone("tree");

  TCanvas* c1 = new TCanvas("c1", "c1", 1600,800);
  c1->SetRightMargin(0.12);
  TCanvas* c2 = new TCanvas("c2", "c2", 1600,800);
  c2->SetRightMargin(0.12);

  TH3D* h_t = new TH3D( "h_T", "Emission Time", 120, -6000, 6000, 120, -6000, 6000, 120, -6000, 6000 );
  h_t->GetXaxis()->SetTitle("X, mm  ");
  h_t->GetXaxis()->SetTitleOffset(1.5);
  h_t->GetYaxis()->SetTitle("Y, mm");
  h_t->GetYaxis()->SetTitleOffset(2.0);
  h_t->GetZaxis()->SetTitle("Z, mm");
  h_t->GetZaxis()->SetTitleOffset(1.3);
  h_t->SetLineWidth(0);
  TH3D* h_llh = new TH3D( "h_LLH", "Step Density", 120, -6000, 6000, 120, -6000, 6000, 120, -6000, 6000 );
  h_llh->GetXaxis()->SetTitle("X, mm  ");
  h_llh->GetXaxis()->SetTitleOffset(1.5);
  h_llh->GetYaxis()->SetTitle("Y, mm");
  h_llh->GetYaxis()->SetTitleOffset(2.0);
  h_llh->GetZaxis()->SetTitle("Z, mm");
  h_llh->GetZaxis()->SetTitleOffset(1.3);
  h_llh->SetLineWidth(0);

  for(int j=0; j<h_t->GetXaxis()->GetNbins(); j++){
    for(int k=0; k<h_t->GetYaxis()->GetNbins(); k++){
      for(int l=0; l<h_t->GetZaxis()->GetNbins(); l++){
	h_t->SetBinContent(j,k,l,0);
	h_llh->SetBinContent(j,k,l,0);
      }
    }
  }



  for( int t = 235; t<315; t++){

    double llh = 0;
    std::stringstream ss;
    ss << "T > " << t << " &&  T < " << t+1 << " && Step > 500000";
    std::string cut = ss.str();
    std::cout << cut << std::endl;

    chain->Draw("X", cut.c_str() );
    TList *primitives = c2->GetListOfPrimitives();
    if(!primitives->FindObject("htemp"))
      continue;
    TH1 *hx = (TH1*)gPad->GetPrimitive("htemp");
    double x = hx->GetMean();
    double xllh = hx->GetMaximum();
    if(xllh>llh)
      llh = xllh;

    std::cout << "ok x" << std::endl;

    chain->Draw("Y", cut.c_str() );
    primitives = c2->GetListOfPrimitives();
    if(!primitives->FindObject("htemp"))
      continue;
    TH1 *hy = (TH1*)gPad->GetPrimitive("htemp");
    double y = hy->GetMean();
    double yllh = hy->GetMaximum();
    if(yllh>llh)
      llh = yllh;

    std::cout << "ok y" << std::endl;

    chain->Draw("Z", cut.c_str() );
    primitives = c2->GetListOfPrimitives();
    if(!primitives->FindObject("htemp"))
      continue;
    TH1 *hz = (TH1*)gPad->GetPrimitive("htemp");
    double z = hz->GetMean();
    double zllh = hz->GetMaximum();
    if(zllh>llh)
      llh = zllh;

    std::cout << "ok z" << std::endl;
    
    int bin = h_t->FindBin(x,y,z);
    if(llh > h_llh->GetBinContent(bin)) {
      h_t->SetBinContent(bin, t+0.5);
      h_llh->SetBinContent(bin, llh);
      std::cout << "llh ok" << std::endl;
    }
  }

  /*
  for(int j=0; j<h_t->GetXaxis()->GetNbins(); j++){
    for(int k=0; k<h_t->GetYaxis()->GetNbins(); k++){
      for(int l=0; l<h_t->GetZaxis()->GetNbins(); l++){
	
	// Calc x y and z for this bin (centre of bin)
	double minX = 500*j;
	double maxX = 500*(j+1);
	double midX = minX + (maxX-minX)/2;
	double minY = 500*k;
	double maxY = 500*(k+1);
	double midY = minY + (maxY-minY)/2;
	double minZ = 100*l;
	double maxZ = 100*(l+1);
	double midZ = minZ + (maxZ-minZ)/2;

	std::cout << midX << " " << midY << " " << midZ << std::endl;
	if(midX < 0 || midY < 0 || midZ < 0)
	  continue;
	std::stringstream ss;
	ss << "X < " << maxX << " &&  X > " << minX << " && Y < " << maxY << " &&  Y > " << minY << " && Z < " << maxZ << " && Z > " << minZ;
	std::string cut = ss.str();
	std::cout << cut << std::endl;
	// Plot T over this xyz range
	chain->Draw("T", cut.c_str() );
	TList *primitives = c1->GetListOfPrimitives();
	if(!primitives->FindObject("htemp"))
	  continue;
	TH1 *htemp = (TH1*)gPad->GetPrimitive("htemp");
	if(htemp->Integral())
	  continue;
	double t = htemp->GetMean();
	double llh = htemp->GetMaximum();
	
	// Fill h_T and h_llh
	h_t->Fill( midX, midY, midZ, t);
	h_llh->Fill( midX, midY, midZ, llh);
      }
    }
  }    
  */
  TColor::InvertPalette();
  c1->cd();
  h_t->GetXaxis()->SetRangeUser(-1000,1000);
  h_t->GetYaxis()->SetRangeUser(-1000,1000);
  h_t->GetZaxis()->SetRangeUser(-1000,1000);
  h_t->Draw("box2Z");
  c2->cd();
  h_llh->GetXaxis()->SetRangeUser(-1000,1000);
  h_llh->GetYaxis()->SetRangeUser(-1000,1000);
  h_llh->GetZaxis()->SetRangeUser(-1000,1000);
  h_llh->Draw("box2Z");

  TFile *outfile = TFile::Open( "t_llPlots.root", "RECREATE");
  h_t->Write();
  h_llh->Write();
  
}
