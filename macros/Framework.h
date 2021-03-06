#ifndef FRAMEWORK
#define FRAMEWORK
#ifndef DEBUG
#define DEBUG
bool debug = false;
#endif
#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TRandom.h"
#include "TGraphErrors.h"
#include "../CMSSW_10_3_2/src/RecoHI/HiEvtPlaneAlgos/interface/HiEvtPlaneList.h"
#ifndef EFFFILE
#define EFFFILE
string efffile = "eff.root";
#endif
using namespace hi;
using namespace std;
  static const int nqxorder = 1;
  int qxorders[nqxorder]={2};

class Framework{
public:
  Framework(string filelist="filelist.dat");
  bool AddFile();
  int GetN(){return maxevents;}
  void GetEntry(int i){tr->GetEntry(i);}
  int SetROIRange(int order, int minCent, int maxCent, double minEta, double maxEta, double minPt, double maxPt);
  void ShowAllROIRanges();
  void SetROIEP(int roi, int EPA, int EPB, int EPC, int EPA2=-1, int EPB2=-1, int EPC2 = -1);
  void AddEvent(int evt);
  double GetAvPt(int roi){return r[roi].pt/r[roi].wn;}
  double GetqnA(int roi){return r[roi].qn/r[roi].wnA;}
  double GetVn(int roi) {return GetqnA(roi)/GetqABC(roi);}
  double GetVnErrSubEvt(int roi);
  double GetVnErr(int roi) {return GetqnAError(roi)/GetqABC(roi);}
  double GetqABC(int roi);
  TH1D * GetSpectra(int roi, bool effCorrect = true);
  TH2D * Get2d(int roi){return r[roi].vn2d;}
  double GetVnxEvt(int roi){return vnxEvt[roi];}
  double GetVnyEvt(int roi){return vnyEvt[roi];}
  FILE * flist;
private:
  TRandom * ran;
  double vnxEvt[100];
  double vnyEvt[100];
  double GetVnSub(int roi,int i) {return r[roi].qnSub[i]/r[roi].wnASub[i]/GetqABC(roi);}
  double GetqnAError(int roi){return sqrt(r[roi].qne)/r[roi].wnA;}
  int maxevents;
  TFile * tf;
  TTree * tr;
  double centval;
  int ntrkval;
  int Noff;
  double vtx;
  double epang[NumEPNames];
  Double_t eporig[NumEPNames];
  Double_t qx[NumEPNames];
  Double_t qy[NumEPNames];
  Double_t q[NumEPNames];
  Double_t vn[NumEPNames];
  Double_t epmult[NumEPNames];
  Double_t sumw[NumEPNames];
  Double_t sumw2[NumEPNames];
  Double_t rescor[NumEPNames];
  Double_t rescorErr[NumEPNames];
  unsigned int runno_;
  TH2F * qxtrk[7]={nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
  TH2F * qytrk[7]={nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
  TH2F * qcnt=nullptr;
  TH2F * avpt=nullptr;
  struct range {
    int order;
    int orderIndx;
    int minCent;
    int maxCent;
    double minEta;
    double maxEta;
    double minPt;
    double maxPt;
    int minEtaBin;
    int maxEtaBin;
    int minPtBin;
    int maxPtBin;
    int A=-1;
    int B=-1;
    int C=-1;
    int A2=-1;
    int B2=-1;
    int C2=-1;
    double qn=0;
    double qne=0;
    double pt=0;
    double wn=0;
    double wne=0;
    double wnA = 0;
    double qAB = 0;
    double wAB = 0;
    double qAC = 0;
    double wAC = 0;
    double qBC = 0;
    double wBC = 0;
    double qnSub[10]={0,0,0,0,0,0,0,0,0,0};
    double qnSube[10]={0,0,0,0,0,0,0,0,0,0};
    double wnASub[10]={0,0,0,0,0,0,0,0,0,0};
    TH2D * vn2d;
  } r[20];
  int nrange = 0;
};
bool Framework::AddFile(){
  char buf[120];
  if(fgets(buf,120,flist)==NULL) return false;
  buf[strlen(buf)-1]=0;
  cout<<buf<<endl;
  string infile = buf;
  cout<<"infile:"<<infile<<":"<<endl;
  tf->Close();
  tf = new TFile(infile.data(),"read");
  tr = (TTree *) tf->Get("vnanalyzer/tree");
  maxevents = tr->GetEntries();
  tr->SetBranchAddress("Cent",&centval);
  tr->SetBranchAddress("NtrkOff",&Noff);
  tr->SetBranchAddress("ntrkflat",&ntrkval);
  tr->SetBranchAddress("Vtx",&vtx);
  tr->SetBranchAddress("epang",&epang);
  tr->SetBranchAddress("eporig",&eporig);
  tr->SetBranchAddress("qx",      &qx);
  tr->SetBranchAddress("qy",      &qy);
  tr->SetBranchAddress("q",       &q);
  tr->SetBranchAddress("vn", &vn );
  tr->SetBranchAddress("mult",    &epmult);
  tr->SetBranchAddress("sumw",    &sumw);
  tr->SetBranchAddress("sumw2",    &sumw2);
  tr->SetBranchAddress("Run",     &runno_);
  tr->SetBranchAddress("Rescor",  &rescor);
  tr->SetBranchAddress("RescorErr",&rescorErr);
  for(int iorder = 0; iorder<nqxorder; iorder++) {
    tr->SetBranchAddress(Form("qxtrk%d",qxorders[iorder]),  &qxtrk[iorder]);
    tr->SetBranchAddress(Form("qytrk%d",qxorders[iorder]),  &qytrk[iorder]);
  }
  tr->SetBranchAddress("qcnt",    &qcnt);
  tr->SetBranchAddress("avpt",    &avpt);
  return true;
}
Framework::Framework(string filelist){
  ran = new TRandom();
  cout<<"open: "<<filelist<<endl;
  system("cat filelist.dat");
  flist = fopen(filelist.data(),"r");
  char buf[120];
  fgets(buf,120,flist);
  buf[strlen(buf)-1]=0;
  cout<<buf<<endl;
  string infile = buf;
  cout<<"infile:"<<infile<<":"<<endl;
  tf = new TFile(infile.data(),"read");
  tr = (TTree *) tf->Get("vnanalyzer/tree");
  maxevents = tr->GetEntries();
  tr->SetBranchAddress("Cent",&centval);
  tr->SetBranchAddress("NtrkOff",&Noff);
  tr->SetBranchAddress("ntrkflat",&ntrkval);
  tr->SetBranchAddress("Vtx",&vtx);
  tr->SetBranchAddress("epang",&epang);
  tr->SetBranchAddress("eporig",&eporig);
  tr->SetBranchAddress("qx",      &qx);
  tr->SetBranchAddress("qy",      &qy);
  tr->SetBranchAddress("q",       &q);
  tr->SetBranchAddress("vn", &vn );
  tr->SetBranchAddress("mult",    &epmult);
  tr->SetBranchAddress("sumw",    &sumw);
  tr->SetBranchAddress("sumw2",    &sumw2);
  tr->SetBranchAddress("Run",     &runno_);
  tr->SetBranchAddress("Rescor",  &rescor);
  tr->SetBranchAddress("RescorErr",&rescorErr);
  for(int iorder = 0; iorder<nqxorder; iorder++) {
    tr->SetBranchAddress(Form("qxtrk%d",qxorders[iorder]),  &qxtrk[iorder]);
    tr->SetBranchAddress(Form("qytrk%d",qxorders[iorder]),  &qytrk[iorder]);
  }
  tr->SetBranchAddress("qcnt",    &qcnt);
  tr->SetBranchAddress("avpt",    &avpt);
  
}
int Framework::SetROIRange(int order, int minCent, int maxCent, double minEta, double maxEta, double minPt, double maxPt) {
  if(maxevents==0) return -1;
  int ifound = -1;
  for(int i = 0; i<nqxorder; i++) {
    if(order == qxorders[i]) ifound = i;
  }
  if(ifound<0) {
    cout<<"The requested vn order is not part of the replay. ABORT!"<<endl;
    return -1;
  }
  r[nrange].orderIndx = ifound;
  GetEntry(0);
  r[nrange].order = order;
  r[nrange].minCent = minCent;
  r[nrange].maxCent = maxCent;
  r[nrange].minEtaBin = qxtrk[0]->GetYaxis()->FindBin(minEta);
  r[nrange].maxEtaBin = qxtrk[0]->GetYaxis()->FindBin(maxEta-0.01);
  r[nrange].minPtBin = qxtrk[0]->GetXaxis()->FindBin(minPt);
  r[nrange].maxPtBin = qxtrk[0]->GetXaxis()->FindBin(maxPt-0.01);
  r[nrange].minEta = qxtrk[0]->GetYaxis()->GetBinLowEdge(r[nrange].minEtaBin);
  r[nrange].maxEta = qxtrk[0]->GetYaxis()->GetBinLowEdge(r[nrange].maxEtaBin)+qxtrk[0]->GetYaxis()->GetBinWidth(r[nrange].maxEtaBin);
  r[nrange].minPt = qxtrk[0]->GetXaxis()->GetBinLowEdge(r[nrange].minPtBin);
  r[nrange].maxPt = qxtrk[0]->GetXaxis()->GetBinLowEdge(r[nrange].maxPtBin)+qxtrk[0]->GetXaxis()->GetBinWidth(r[nrange].maxPtBin);
  r[nrange].vn2d = new TH2D(Form("vn2d_%d_%d_%d_%03.1f_%03.1f_%03.1f_%03.1f",order,minCent,maxCent,minEta,maxEta,minPt,maxPt),
			    Form("vn2d_%d_%d_%d_%03.1f_%03.1f_%03.1f_%03.1f",order,minCent,maxCent,minEta,maxEta,minPt,maxPt),100,-1.4,1.4,100,-1.4,1.4);
  r[nrange].vn2d->SetOption("colz");

  ++nrange;
  return nrange-1;
} 

void Framework::ShowAllROIRanges(){
  std::cout<<"indx\tminCent\tmaxCent\tminEta\tmaxEta\tminPt\tmaxPt\tminEtaBin\tmaxEtaBin\tminPtBin\tmaxPtBin"<<std::endl;
  for(int i = 0; i<nrange; i++) {
    std::cout<<i<<"\t"<<r[i].minCent<<"\t"<<r[i].maxCent<<"\t"<<r[i].minEta<<"\t"<<r[i].maxEta<<"\t"<<r[i].minPt<<"\t"<<r[i].maxPt<<"\t"<<r[i].minEtaBin<<"\t"<<r[i].maxEtaBin<<"\t"<<r[i].minPtBin<<"\t"<<r[i].maxPtBin<<std::endl;
  }
}

void Framework::SetROIEP(int roi, int EPA, int EPB, int EPC, int EPA2, int EPB2, int EPC2){
  if(roi>nrange) {
    cout<<"request roi index = "<<roi<< " > nrange = "<<nrange<<endl;
    return;
  }
  r[roi].A = EPA;
  r[roi].B = EPB;
  r[roi].C = EPC;
  r[roi].A2 = EPA2;
  r[roi].B2 = EPB2;
  r[roi].C2 = EPC2;
}

void Framework::AddEvent(int evt) {
  GetEntry(evt);
  for(int i = 0; i<nrange; i++) {
    if(r[i].A<0) {
      cout<<"Event planes have not yet been initialized for ROI # "<<i<<endl;
    }
    if(centval<(double) r[i].minCent || centval>=(double)r[i].maxCent) continue;
    double qAx = qx[r[i].A];
    double qAy = qy[r[i].A];
    double qnx = 0;
    double qny = 0;
    double qnxe = 0;
    double qnye = 0;
    double qncnt = 0;
    double qncnte = 0;
    double pt = 0;
    for(int j = r[i].minEtaBin; j<= r[i].maxEtaBin; j++) {
      for(int k = r[i].minPtBin; k<=r[i].maxPtBin; k++) {
	qnx+=qxtrk[r[i].orderIndx]->GetBinContent(k,j);
	qny+=qytrk[r[i].orderIndx]->GetBinContent(k,j);
	qncnt+=qcnt->GetBinContent(k,j);
	if(qcnt->GetBinContent(k,j)>0) {
	  qnxe+=pow(qxtrk[r[i].orderIndx]->GetBinError(k,j),2);
	  qnye+=pow(qytrk[r[i].orderIndx]->GetBinError(k,j),2);
	  qncnte+=pow(qcnt->GetBinError(k,j),2);
	}
	pt+=avpt->GetBinContent(k,j);
      }
    }
    int isub = ran->Uniform(0,9.9999);
    double val = qAx*qnx+qAy*qny;
    vnxEvt[i] = -2;
    vnyEvt[i] = -2;
    if(qncnt>0) {
      vnxEvt[i] = qnx/qncnt;
      vnyEvt[i] = qny/qncnt;
      r[i].vn2d->Fill(vnxEvt[i],vnyEvt[i]);
      r[i].qn+=val;
      r[i].qne+=pow(qAx,2)*qnxe/fabs(qnx) + pow(qAy,2)*qnye/fabs(qny);
      r[i].qnSub[isub]+=val;
      r[i].qnSube[isub]+=pow(qAx,2)*qnxe/qnx + pow(qAy,2)*qnye/qny;
      r[i].wn+=qncnt;
      r[i].wne+=qncnte;
      r[i].pt+=pt;
      double Ax = qx[r[i].A];
      double Bx = qx[r[i].B];
      double Cx = qx[r[i].C];
      double Ay = qy[r[i].A];
      double By = qy[r[i].B];
      double Cy = qy[r[i].C];
      double wA = sumw[r[i].A];
      double wB = sumw[r[i].B];
      double wC = sumw[r[i].C];
      r[i].qAB += Ax*Bx+Ay*By;
      r[i].qAC += Ax*Cx+Ay*Cy;
      r[i].qBC += Bx*Cx+By*Cy;
      r[i].wAB += wA*wB;
      r[i].wAC += wA*wC;
      r[i].wBC += wB*wC;
      r[i].wnA+=qncnt*wA;
      r[i].wnASub[isub]+=qncnt*wA;
    }
  }

}
double Framework::GetqABC(int roi) {
  double AB = r[roi].qAB/r[roi].wAB;
  double AC = r[roi].qAC/r[roi].wAC;
  double BC = r[roi].qBC/r[roi].wBC;
  return sqrt(AB*AC/BC);
}

TH1D * Framework::GetSpectra(int roi, bool effCorrect) {
  TH2D * spec;
  TH2D * heff=0;
  int minCent = r[roi].minCent;
  int maxCent = r[roi].maxCent;
  string crnge = Form("%d_%d",minCent,maxCent);
  string spname = "vnanalyzer/Spectra/"+crnge+"/ptspec";
  std::cout<<"spec 2d: "<<spname<<std::endl;
  TH2D * ptcnt = (TH2D *) tf->Get(spname.data());
  std::cout<<ptcnt<<std::endl;
  spec = (TH2D *) ptcnt->Clone(Form("spec_%s",crnge.data()));
  spec->SetDirectory(0);
  if(effCorrect){
    TFile * feff = new TFile(efffile.data());
    double avcent = (minCent+maxCent)/2.;
    if(avcent<5) {
      heff = (TH2D *) feff->Get("Eff_0_5");
    } else if (avcent<10) {
      heff = (TH2D *) feff->Get("Eff_5_10");
    } else if (avcent<30) {
      heff = (TH2D *) feff->Get("Eff_10_30");
    } else if (avcent<50) {
      heff = (TH2D *) feff->Get("Eff_30_50");
    } else if (avcent<100) {
      heff = (TH2D *) feff->Get("Eff_50_100");
    }
    for(int i = 1; i<=spec->GetNbinsX(); i++) {
      for(int j = 1; j<=spec->GetNbinsY(); j++) {
	double binpt = spec->GetXaxis()->GetBinCenter(i);
	double bineta = spec->GetYaxis()->GetBinCenter(j);
	double eff = heff->GetBinContent(heff->GetXaxis()->FindBin(bineta),heff->GetYaxis()->FindBin(binpt));
	if(eff == 0 && debug) cout<<i<<"\t"<<j<<"\t"<<eff<<"\t"<<binpt<<"\t"<<bineta<<endl;
	if(eff>0) spec->SetBinContent(i,j,spec->GetBinContent(i,j)/eff);
      }
    }
    feff->Close(); 
  }
  double etamin = r[roi].minEta;
  double etamax = r[roi].maxEta;
  int ietamin = spec->GetYaxis()->FindBin(etamin+0.01);
  int ietamax = spec->GetYaxis()->FindBin(etamax-0.01);
  TH1D * spec1d = (TH1D *) spec->ProjectionX(Form("spec1d_%d_%d_%4.1f_%4.1f",minCent,maxCent,etamin,etamax),ietamin,ietamax);
  for(int i = 0; i<spec1d->GetNbinsX(); i++) {
    double deta = etamax-etamin;
    double dpt = spec1d->GetBinWidth(i+1);
    spec1d->SetBinContent(i+1,spec1d->GetBinContent(i+1)/deta/dpt);
    spec1d->SetBinError(i+1,spec1d->GetBinError(i+1)/deta/dpt);

  }
  spec1d->SetXTitle("p_{T} (GeV/c)");
  spec1d->SetYTitle("dN/d#eta/dp_{T}");
  spec1d->SetMarkerStyle(20);
  spec1d->SetMarkerColor(kBlue);
  spec1d->SetLineColor(kBlue);
  spec->Delete();
  return spec1d;
}

double Framework::GetVnErrSubEvt(int roi){
  double vnav=0;
  for(int i = 0; i<10; i++) vnav+=GetVnSub(roi,i);
  vnav/=10.;
  double sig = 0;
  for(int i = 0; i<10; i++) sig+=pow(GetVnSub(roi,i)-vnav,2);
  sig = sqrt(sig/9.);
  cout<<"err: "<<vnav<<"\t"<<sig<<endl;
  return sig;
}
#endif
