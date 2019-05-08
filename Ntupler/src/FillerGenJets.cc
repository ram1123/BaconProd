#include "BaconProd/Ntupler/interface/FillerGenJets.hh"
#include "BaconProd/Utils/interface/JetTools.hh"
#include "BaconAna/DataFormats/interface/TGenEventInfo.hh"
#include "BaconAna/DataFormats/interface/TGenParticle.hh"
#include "BaconAna/DataFormats/interface/TGenJet.hh"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "fastjet/contrib/Njettiness.hh"
#include "fastjet/contrib/SoftDrop.hh"

#include <TClonesArray.h>
#include <TMath.h>
#include <TLorentzVector.h>

using namespace baconhep;

//--------------------------------------------------------------------------------------------------
FillerGenJets::FillerGenJets(const edm::ParameterSet &iConfig,edm::ConsumesCollector && iC):
  fGenParName    (iConfig.getUntrackedParameter<std::string>("edmGenParticlesName","genParticles")),
  fGenJetName    (iConfig.getUntrackedParameter<std::string>("genJetName","AK4GenJets")),
  fGenFatJetName (iConfig.getUntrackedParameter<std::string>("genFatJetName","AK8GenJets"))
{
  fTokGenJet       = iC.consumes<reco::GenJetCollection>(fGenJetName); 
  fTokGenFatJet    = iC.consumes<reco::GenJetCollection>(fGenFatJetName); 
  fTokGenPar       = iC.consumes<reco::GenParticleCollection>(fGenParName); 
  fCAJetDef = new fastjet::JetDefinition(fastjet::cambridge_algorithm, 0.8);
  int activeAreaRepeats = 1;
  double ghostArea      = 0.01;
  double ghostEtaMax    = 7.0;
  fActiveArea           = new fastjet::ActiveAreaSpec (ghostEtaMax,activeAreaRepeats,ghostArea);  
  fAreaDefinition       = new fastjet::AreaDefinition (fastjet::active_area_explicit_ghosts, *fActiveArea );
  fTrimmer1  = new fastjet::Filter( fastjet::Filter(fastjet::JetDefinition(fastjet::kt_algorithm, 0.2),  fastjet::SelectorPtFractionMin(0.05)));
  fECF = new EnergyCorrelations();
}

//--------------------------------------------------------------------------------------------------
FillerGenJets::~FillerGenJets(){}

//--------------------------------------------------------------------------------------------------
void FillerGenJets::fill(TClonesArray *array,TClonesArray *fatJetArray,
                         const edm::Event &iEvent)
{
  assert(array);
  // Get generator jet collection
  edm::Handle<reco::GenJetCollection> hGenJetProduct;
  const reco::GenJetCollection *genJets = 0;
  iEvent.getByToken(fTokGenJet,hGenJetProduct);
  assert(hGenJetProduct.isValid());
  genJets = hGenJetProduct.product();

  edm::Handle<reco::GenJetCollection> hGenFatJetProduct;
  const reco::GenJetCollection *genFatJets = 0;
  if(fGenFatJetName.size() > 0) { 
    iEvent.getByToken(fTokGenFatJet,hGenFatJetProduct);
    assert(hGenFatJetProduct.isValid());
    genFatJets = hGenFatJetProduct.product();
  }
  // Get Jet Flavor Match
  //edm::Handle<reco::JetFlavourMatchingCollection> jetFlavourMatch;
  //iEvent.getByLabel(fJetFlavorName, jetFlavourMatch);
  //edm::Handle<reco::JetFlavourMatchingCollection> jetFlavourMatchPhys;
  //iEvent.getByLabel(fJetFlavorPhysName, jetFlavourMatchPhys);

  // Get generator particles collection
  edm::Handle<reco::GenParticleCollection> hGenParProduct;
  iEvent.getByToken(fTokGenPar,hGenParProduct);
  assert(hGenParProduct.isValid());  
  const reco::GenParticleCollection genParticles = *(hGenParProduct.product());  

  // loop over GEN particles
  TClonesArray &rArray = *array;
  for (reco::GenJetCollection::const_iterator itGenJ = genJets->begin(); itGenJ!=genJets->end(); ++itGenJ) {
    // construct object and place in array
    assert(rArray.GetEntries() < rArray.GetSize());
    const int index = rArray.GetEntries();
    new(rArray[index]) baconhep::TGenJet();
    baconhep::TGenJet *pGenJet = (baconhep::TGenJet*)rArray[index];
    pGenJet->pdgId   = flavor(&(*itGenJ),genParticles);
    pGenJet->pt      = itGenJ->pt();
    pGenJet->eta     = itGenJ->eta();
    pGenJet->phi     = itGenJ->phi();
    pGenJet->mass    = itGenJ->mass();
    /*
    double* lElectrons =  genCone(&(*itGenJ),genParticles,0,0.10,  -1);
    double* lMuons     =  genCone(&(*itGenJ),genParticles,0,0.10,  -2);
    double* lPhotons   =  genCone(&(*itGenJ),genParticles,0,0.10,   1);
    double* lTotal     =  genCone(&(*itGenJ),genParticles,0,0.10,   0);
    double* lIso03     =  genCone(&(*itGenJ),genParticles,0.1,0.3,  0);
    double* lIso04     =  genCone(&(*itGenJ),genParticles,0.1,0.4,  0);
    double* lIso05     =  genCone(&(*itGenJ),genParticles,0.1,0.5,  0);

    pGenJet->elept       =  lElectrons[1];
    pGenJet->eleeta      =  lElectrons[2];
    pGenJet->elephi      =  lElectrons[3];
    pGenJet->elem        =  lElectrons[4];

    pGenJet->mupt        =  lMuons[1];
    pGenJet->mueta       =  lMuons[2];
    pGenJet->muphi       =  lMuons[3];
    pGenJet->mum         =  lMuons[4];

    pGenJet->gapt        =  lPhotons[1];
    pGenJet->gaeta       =  lPhotons[2];
    pGenJet->gaphi       =  lPhotons[3];
    pGenJet->gam         =  lPhotons[4];

    pGenJet->totpt       =  lTotal[1];
    pGenJet->toteta      =  lTotal[2];
    pGenJet->totphi      =  lTotal[3];
    pGenJet->totm        =  lTotal[4];

    pGenJet->iso03       =  lIso03[0];
    pGenJet->iso04       =  lIso04[0];
    pGenJet->iso05       =  lIso05[0];

    delete lElectrons;
    delete lMuons    ; 
    delete lPhotons  ; 
    delete lTotal    ; 
    delete lIso03    ; 
    delete lIso04    ; 
    delete lIso05;
    */
  }

  if(genFatJets != 0) { 
    TClonesArray &rArray = *fatJetArray;
    for (reco::GenJetCollection::const_iterator itGenJ = genFatJets->begin(); itGenJ!= genFatJets->end(); ++itGenJ) {
      // construct object and place in array
      assert(rArray.GetEntries() < rArray.GetSize());
      const int index = rArray.GetEntries();
      new(rArray[index]) baconhep::TGenJet();
      baconhep::TGenJet *pGenJet = (baconhep::TGenJet*)rArray[index];
      pGenJet->pdgId   = flavor(&(*itGenJ),genParticles);
      pGenJet->pt      = itGenJ->pt();
      pGenJet->eta     = itGenJ->eta();
      pGenJet->phi     = itGenJ->phi();
      pGenJet->mass    = itGenJ->mass();

      float pMsd = 0; float pe2sdb1 =0; float pe3_v2_sdb1 =0;
      if(pGenJet->pt > 100) softdrop(&(*itGenJ),pMsd,pe2sdb1,pe3_v2_sdb1);                                                                       
      pGenJet->msd       = pMsd;                                                                                                                                 
      pGenJet->e2sdb1    = pe2sdb1;
      pGenJet->e3_v2_sdb1= pe3_v2_sdb1;     

      /*
      double* lElectrons =  genCone(&(*itGenJ),genParticles,0,0.10,  -1);
      double* lMuons     =  genCone(&(*itGenJ),genParticles,0,0.10,  -2);
      double* lPhotons   =  genCone(&(*itGenJ),genParticles,0,0.10,   1);
      double* lTotal     =  genCone(&(*itGenJ),genParticles,0,0.10,   0);
      double* lIso03     =  genCone(&(*itGenJ),genParticles,0.1,0.3,  0);
      double* lIso04     =  genCone(&(*itGenJ),genParticles,0.1,0.4,  0);
      double* lIso05     =  genCone(&(*itGenJ),genParticles,0.1,0.5,  0);
    
      pGenJet->elept       =  lElectrons[1];
      pGenJet->eleeta      =  lElectrons[2];
      pGenJet->elephi      =  lElectrons[3];
      pGenJet->elem        =  lElectrons[4];
      
      pGenJet->mupt        =  lMuons[1];
      pGenJet->mueta       =  lMuons[2];
      pGenJet->muphi       =  lMuons[3];
      pGenJet->mum         =  lMuons[4];
      
      pGenJet->gapt        =  lPhotons[1];
      pGenJet->gaeta       =  lPhotons[2];
      pGenJet->gaphi       =  lPhotons[3];
      pGenJet->gam         =  lPhotons[4];

      pGenJet->totpt       =  lTotal[1];
      pGenJet->toteta      =  lTotal[2];
      pGenJet->totphi      =  lTotal[3];
      pGenJet->totm        =  lTotal[4];
      
      pGenJet->iso03       =  lIso03[0];
      pGenJet->iso04       =  lIso04[0];
      pGenJet->iso05       =  lIso05[0];
      float pMTrim = 0; 
      float pTau1  = 0;
      float pTau2  = 0;
      if(pGenJet->pt > 100) trim(&(*itGenJ),pMTrim,pTau1,pTau2);
      pGenJet->mtrim       = pMTrim;
      pGenJet->tau2        = pTau2;
      pGenJet->tau1        = pTau1;
      
      delete lElectrons;
      delete lMuons    ; 
      delete lPhotons  ; 
      delete lTotal    ; 
      delete lIso03    ; 
      delete lIso04    ; 
      delete lIso05;
      */
    }
  }
}
double* FillerGenJets::genCone(const reco::GenJet *iJet,const reco::GenParticleCollection &iGenParticles,double iDRMin,double iDRMax,int iType) {
  double lIso = 0;
  TLorentzVector lVec; lVec.SetPtEtaPhiM(0,0,0,0);
  for (reco::GenParticleCollection::const_iterator itGenP = iGenParticles.begin(); itGenP!=iGenParticles.end(); ++itGenP) {
    if(itGenP->status() != 1) continue;
    double pDPhi1 = fabs(iJet->phi()-itGenP->phi()); if(pDPhi1 > 2.*TMath::Pi()-pDPhi1) pDPhi1 = 2.*TMath::Pi()-pDPhi1;
    double pDEta1 = fabs(iJet->eta()-itGenP->eta());
    double pDR    = sqrt(pDPhi1*pDPhi1 + pDEta1*pDEta1);
    if(pDR  < iDRMin  || pDR > iDRMax) continue;
    int pPdgId = itGenP->pdgId();
    if(fabs(pPdgId) < 17  && fabs(pPdgId) > 10 && pPdgId % 2 == 0 )  continue; //skip neutrinos
    if(itGenP->charge()   == 0  && iType <   0) continue;
    if(itGenP->charge()   != 0  && iType >   0) continue;
    if(fabs(pPdgId)       != 22 && iType ==  1) continue;
    if(fabs(pPdgId)       != 11 && iType == -1) continue;
    if(fabs(pPdgId)       != 13 && iType == -2) continue;
    TLorentzVector pVec; pVec.SetPtEtaPhiM(itGenP->pt(),itGenP->eta(),itGenP->phi(),itGenP->mass());
    lVec += pVec;
    lIso += itGenP->pt();
  }
  double *lReturn = new double[5];
  lReturn[0] = lIso;
  if(lVec.Px() > 0) {
    lReturn[1] = lVec.Pt();
    lReturn[2] = lVec.Eta();
    lReturn[3] = lVec.Phi();
    lReturn[4] = lVec.M();
  }
  return lReturn;
}
int FillerGenJets::flavor(const reco::GenJet *iJet,const reco::GenParticleCollection &iGenParticles) { 
  int    lId    = -1;
  double lPtMax = -1; 
  for (reco::GenParticleCollection::const_iterator itGenP = iGenParticles.begin(); itGenP!=iGenParticles.end(); ++itGenP) {
    double pDPhi1 = fabs(iJet->phi()-itGenP->phi()); if(pDPhi1 > 2.*TMath::Pi()-pDPhi1) pDPhi1 = 2.*TMath::Pi()-pDPhi1;
    double pDEta1 = fabs(iJet->eta()-itGenP->eta());
    double pDR    = sqrt(pDPhi1*pDPhi1 + pDEta1*pDEta1);
    if(pDR  > 0.25) continue;
    if(itGenP->pt() > lPtMax) lId    = itGenP->pdgId();
    if(itGenP->pt() > lPtMax) lPtMax = itGenP->pt(); 
  }  
  return lId;
}
void FillerGenJets::trim(const reco::GenJet *iJet,float &iMTrim,float &iTau1,float &iTau2) { 
  std::vector<const reco::GenParticle*> genConstituents = iJet->getGenConstituents(); 
  std::vector<fastjet::PseudoJet>  lClusterParticles;
  for(unsigned int ic=0; ic<genConstituents.size(); ic++) {
    const reco::GenParticle* gencand = genConstituents[ic];
    fastjet::PseudoJet   pPart(gencand->px(),gencand->py(),gencand->pz(),gencand->energy());
    lClusterParticles.emplace_back(pPart);
  }
  fClustering = new fastjet::ClusterSequenceArea(lClusterParticles, *fCAJetDef, *fAreaDefinition);
  std::vector<fastjet::PseudoJet>  lOutJets = fClustering->inclusive_jets(20.0);
  if(lOutJets.size() == 0) {
    delete fClustering;
    return;
  }
  fastjet::PseudoJet pT1Jet = (*fTrimmer1)( lOutJets[0]);
  iMTrim = pT1Jet.m();
 
  fastjet::contrib::NormalizedMeasure          normalizedMeasure        (1.0,0.8);
  fastjet::contrib::Njettiness routine(fastjet::contrib::Njettiness::onepass_kt_axes,normalizedMeasure);
  iTau1 = routine.getTau(1.,lClusterParticles);
  iTau2 = routine.getTau(2.,lClusterParticles);
  delete fClustering;
}
void FillerGenJets::softdrop(const reco::GenJet *iJet,float &iMsd,float &ie2,float &ie3) {
  std::vector<fastjet::PseudoJet>  lClusterParticles; 
  for (unsigned i = 0;  i < iJet->numberOfDaughters (); i++) {
    const reco::Candidate * daughter = iJet->daughter( i );
    fastjet::PseudoJet   pPart(daughter->px(),daughter->py(),daughter->pz(),daughter->energy());                                                                       
    lClusterParticles.emplace_back(pPart);    
  }
  fastjet::JetDefinition lCJet_def(fastjet::cambridge_algorithm, 0.8);
  fastjet::ClusterSequence lCClust_seq(lClusterParticles, lCJet_def);
  std::vector<fastjet::PseudoJet>  lOutJets = lCClust_seq.inclusive_jets(0.0);
  if(lOutJets.size() == 0) {
    std::cout << "no jets " << std::endl;
    return;
  }
  double beta=1;
  fastjet::contrib::SoftDrop SD(0.,0.1,0.8);
  fastjet::PseudoJet SD_jet = SD(lOutJets[0]);
  iMsd = SD_jet.m();
  //std::cout << iMsd << std::endl;
  std::vector<fastjet::PseudoJet> lSDClusterParticles = SD_jet.constituents();
  std::sort(lSDClusterParticles.begin(),lSDClusterParticles.end(),JetTools::orderPseudoJet);
  int nFilter = TMath::Min(100,(int)lSDClusterParticles.size());
  std::vector<fastjet::PseudoJet> lSDFilter(lSDClusterParticles.begin(),lSDClusterParticles.begin()+nFilter);
  fECF->calcECFN(beta,lSDFilter,true);
  ie2 = float(fECF->manager->ecfns["2_2"]);
  ie3 = float(fECF->manager->ecfns["3_2"]);
}
