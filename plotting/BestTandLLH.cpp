#include <TH3D.h>
#include <TCanvas.h>
#include "TFile.h"
#include "TColor.h"

#include <string>
#include <iostream>

void PlotAll(TString fname){

  TFile *f = TFile::Open(fname);

  TCanvas* c1 = new TCanvas("c1", "c1", 1600,800);
  c1->SetRightMargin(0.12);
  TCanvas* c2 = new TCanvas("c2", "c2", 1600,800);
  c2->SetRightMargin(0.12);

  TH3* h_t;
  TH3* h_llh;

  for(int i=0; i<80; i++){

    int t = i-40;

    //double n = 236.524347 + 80*i;
    //double n = 3180.681152 + 80*i;

    TString hname = Form("h_%d", i);
    std::cout << "getting " << hname << std::endl;

    TH3D* h = (TH3D*)f->Get(hname)->Clone();
    std::cout << "got " << hname << std::endl;
    if(i==0){
      h_t = (TH3D*)f->Get(hname)->Clone();
      h_t->SetName("h_t");
      h_t->SetTitle("Emission Time");
      h_t->Reset();
      h_llh = (TH3D*)f->Get(hname)->Clone();
      h_llh->Reset();
      h_llh->SetName("h_llh");
      h_llh->SetTitle("");
      std::cout << "done 0 " << std::endl;
    }

    for(int j=0; j<h->GetXaxis()->GetNbins(); j++){
      for(int k=0; k<h->GetYaxis()->GetNbins(); k++){
      	for(int l=0; l<h->GetZaxis()->GetNbins(); l++){
	        if(h->GetBinContent(j, k, l) > h_llh->GetBinContent(j, k, l)){
	          h_llh->SetBinContent(j, k, l, h->GetBinContent(j,k,l));
	          //if(h->GetBinContent(j, k, l) > 600)
	          h_t->SetBinContent(j, k, l, t);
	        }
	      }
      }
    }    
  }

  //TColor::InvertPalette();
  c1->cd();
  h_t->Draw("box2Z");
  c2->cd();
  h_llh->Draw("box2Z");

  TFile fout("outputNegX.root","RECREATE");
  h_t->Write("h_t");
  h_llh->Write("h_llh");
  
}
