////////////////////////////////////////////////////////////////////////
// Class:       SingleCRTMatchingProducer
// Plugin Type: producer (art v2_10_03)
// File:        SingleCRTMatchingProducer_module.cc
//
// Generated at Wed Jun 27 04:09:39 2018 by Andrew Olivier using cetskelgen
// from cetlib version v3_02_00.
////////////////////////////////////////////////////////////////////////

//Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Persistency/Common/PtrMaker.h"
#include "canvas/Persistency/Common/Assns.h"
#include "lardata/Utilities/AssociationUtil.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "art_root_io/TFileService.h"

//LArSoft includes

#include "lardataobj/Simulation/AuxDetSimChannel.h"
#include "larcore/Geometry/Geometry.h"
#include "nusimdata/SimulationBase/MCParticle.h"
#include "nug4/ParticleNavigation/ParticleList.h"


#include "larsim/MCCheater/BackTrackerService.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/TrackTrajectory.h"
#include "lardataobj/RecoBase/Hit.h"
#include "larsim/MCCheater/ParticleInventoryService.h"
#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/AnalysisBase/T0.h"
#include "lardataobj/AnalysisBase/CosmicTag.h"



#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "lardata/DetectorInfoServices/DetectorClocksService.h"

#include "dune/Protodune/singlephase/CTB/data/pdspctb.h"
#include "lardataobj/RawData/RDTimeStamp.h"
#include "lardataobj/RecoBase/OpFlash.h"
#include "lardataobj/RecoBase/OpHit.h"

//Local includes
#include "dunetpc/dune/Protodune/singlephase/CRT/data/CRTTrigger.h"

//ROOT includes
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TImage.h"
#include "TTree.h"
#include "TH1D.h"
#include "TStyle.h"
#include "TString.h"

//c++ includes
#include <numeric> //std::accumulate was moved from <algorithm> to <numeric> in c++14
#include <iostream>
#include <cmath>
using namespace std;   // Namespaces established to make life easier
using namespace ROOT::Math;
namespace CRT {
  class SingleCRTMatchingProducer;
}


class CRT::SingleCRTMatchingProducer : public art::EDProducer {
public:
  explicit SingleCRTMatchingProducer(fhicl::ParameterSet const & p);

  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  SingleCRTMatchingProducer(SingleCRTMatchingProducer const &) = delete;
  SingleCRTMatchingProducer(SingleCRTMatchingProducer &&) = delete;
  SingleCRTMatchingProducer & operator = (SingleCRTMatchingProducer const &) = delete;
  SingleCRTMatchingProducer & operator = (SingleCRTMatchingProducer &&) = delete;
  
  bool moduleMatcherMCC(int module1, int module2);
  bool moduleMatcherData(int module1, int module2);
  // Required functions.
  double setAngle(double angle);
  void produce(art::Event & event) override;
  void beginJob() override;
  void endJob() override ;
    std::string fTrackModuleLabel = "pandoraTrack";
  std::string fopModuleLabel= "opflash";

int moduletoCTB(int module2, int module1);
  int nEvents = 0;
  int nHaloMuons = 0;
  private: ofstream logFile;
  const std::string reco="reco";
  //Parameters for reading in CRT::Triggers and associated AuxDetSimChannels.
  art::InputTag fCRTLabel; //Label for the module that produced 
  art::InputTag fCTBLabel;
  TTree * fCRTTree;
    bool fMCCSwitch;
    bool fModuleSwitch;
    int fADCThreshold;
    int fModuletoModuleTimingCut;
    int fFronttoBackTimingCut;
    int fOpCRTTDiffCut;


    double dotCos;
    int adcX, adcY;
    double X_CRT, Y_CRT, Z_CRT;
    double trackX1, trackX2, trackY1, trackY2, trackZ1, trackZ2;
    int moduleX, moduleY; 
    int stripX, stripY;
    double deltaX, deltaY;
    double CRTT0;
    double flashTime;
    double opCRTTDiff;
  typedef struct // Structures for arrays to move hits from raw to reco to validation
  {

    int channel;
    int module;
    int channelGeo;
    int adc;
    int triggerTime;
    int triggerNumber;
  }
  tempHits;

  typedef struct {
    int tempId;	
    int adcX;
    int adcY;

    double hitPositionX;
    double hitPositionY;
    double hitPositionZ;
    double timeAvg;
    int moduleX;
    int moduleY;
    int stripX;
    int stripY;
    int trigNumberX, trigNumberY;
  }
  recoHits;

  typedef struct // These are displacement metrics for track and hit reco
  {
    int tempId;
    int CRTTrackId;
    int recoId;
    int adcX1;
    int adcY1;
    double deltaX;
    double deltaY;
    double averageSignedDistanceXY;
    double averageSignedDistanceYZ;
    double averageSignedDistanceXZ;
    double averageSignedDistance;
    double dotProductCos;
    double X1;
    double Y1;
    double Z1;
    int trackID;
    TVector3 trackStartPosition;
    TVector3 trackEndPosition;
    int moduleX1, moduleY1;
    int stripX1, stripY1;
    double flashTDiff;
    double timeAvg;
    int trigNumberX, trigNumberY; 
  }
  tracksPair;

  struct removePairIndex // Struct to remove tracks after sorting
  {
    const tracksPair tracksPair1;
    removePairIndex(const tracksPair & tracksPair0): tracksPair1(tracksPair0) {}

    bool operator()(const tracksPair & tracksPair2) {
      return (tracksPair1.recoId == tracksPair2.recoId || tracksPair1.CRTTrackId == tracksPair2.CRTTrackId || (tracksPair1.stripX1==tracksPair2.stripX1 && tracksPair1.moduleX1==tracksPair2.moduleX1) || (tracksPair1.stripY1==tracksPair2.stripY1 && tracksPair1.moduleY1==tracksPair2.moduleY1));
    }
  };

  struct sortPair // Struct to sort to find best CRT track for TPC track
  {
    bool operator()(const tracksPair & pair1,
      const tracksPair & pair2) {
     
	//return (fabs(pair1.dotProductCos)>fabs(pair2.dotProductCos));
	return (fabs(pair1.deltaX)+fabs(pair1.deltaY)<fabs(pair2.deltaX)+fabs(pair2.deltaY));

  }
  };
  std::vector < recoHits > primaryHits_F;
  std::vector < recoHits > primaryHits_B;

  std::vector < tempHits > tempHits_F;
  std::vector < tempHits > tempHits_B;
  std::vector < tracksPair > tracksPair_F;
  std::vector < tracksPair > tracksPair_B;
};

CRT::SingleCRTMatchingProducer::SingleCRTMatchingProducer(fhicl::ParameterSet
    const & p):
  EDProducer{p}, fCRTLabel(p.get < art::InputTag > ("CRTLabel")), fCTBLabel(p.get<art::InputTag>("CTBLabel")) {
    consumes < std::vector < CRT::Trigger >> (fCRTLabel);
    consumes < std::vector < art::Assns < sim::AuxDetSimChannel, CRT::Trigger >>> (fCRTLabel); // CRT art consumables
  fMCCSwitch=(p.get<bool>("MCC"));
  produces< std::vector<anab::T0> >();
  produces< std::vector<anab::CosmicTag> >();
  produces< art::Assns<recob::Track, anab::T0> >();
  produces< art::Assns<recob::Track, anab::CosmicTag> >();
  produces< art::Assns<anab::CosmicTag, anab::T0> >();
  produces< art::Assns<CRT::Trigger, anab::CosmicTag> >();

  }



// v6 Geo Channel Map
bool CRT::SingleCRTMatchingProducer::moduleMatcherMCC(int module1, int module2) {
  // Function checking if two hits could reasonably be matched into a 2D hit
  if ((module1 == 6 && (module2 == 10 || module2 == 11)) || (module1 == 14 && (module2 == 10 || module2 == 11)) || (module1 == 19 && (module2 == 26 || module2 == 27)) || (module1 == 31 && (module2 == 26 || module2 == 27)) || (module1 == 7 && (module2 == 12 || module2 == 13)) || (module1 == 15 && (module2 == 12 || module2 == 13)) || (module1 == 18 && (module2 == 24 || module2 == 25)) || (module1 == 30 && (module2 == 24 || module2 == 25)) || (module1 == 1 && (module2 == 4 || module2 == 5)) || (module1 == 9 && (module2 == 4 || module2 == 5)) || (module1 == 16 && (module2 == 20 || module2 == 21)) || (module1 == 28 && (module2 == 20 || module2 == 21)) || (module1 == 0 && (module2 == 2 || module2 == 3)) || (module1 == 8 && (module2 == 2 || module2 == 3)) || (module1 == 17 && (module2 == 22 || module2 == 23)) || (module1 == 29 && (module2 == 22 || module2 == 23))) return 1;
  else return 0;

}

bool CRT::SingleCRTMatchingProducer::moduleMatcherData(int module1, int module2) {
  // Function checking if two hits could reasonably be matched into a 2D hit
  if ((module1 == 6 && (module2 == 10 || module2 == 11)) || (module1 == 14 && (module2 == 10 || module2 == 11)) || (module1 == 19 && (module2 == 26 || module2 == 27)) || (module1 == 31 && (module2 == 26 || module2 == 27)) || (module1 == 7 && (module2 == 12 || module2 == 13)) || (module1 == 15 && (module2 == 12 || module2 == 13)) || (module1 == 18 && (module2 == 24 || module2 == 25)) || (module1 == 30 && (module2 == 24 || module2 == 25)) || (module1 == 1 && (module2 == 4 || module2 == 5)) || (module1 == 9 && (module2 == 4 || module2 == 5)) || (module1 == 16 && (module2 == 20 || module2 == 21)) || (module1 == 28 && (module2 == 20 || module2 == 21)) || (module1 == 0 && (module2 == 2 || module2 == 3)) || (module1 == 8 && (module2 == 2 || module2 == 3)) || (module1 == 17 && (module2 == 22 || module2 == 23)) || (module1 == 29 && (module2 == 22 || module2 == 23))) return 1;
  else return 0;

}
/* v5 Geo
// Function to match CRT modules below is for MCC and the 2nd is for data
bool CRT::SingleCRTMatchingProducer::moduleMatcherMCC(int module1, int module2) {
  // Function checking if two hits could reasonably be matched into a 2D hit
  if ((module1 == 0 && (module2 == 5 || module2 == 4)) || (module1 == 12 && (module2 == 5 || module2 == 4)) || (module1 == 16 && (module2 == 20 || module2 == 21)) || (module1 == 28 && (module2 == 20 || module2 == 21)) || (module1 == 1 && (module2 == 6 || module2 == 7)) || (module1 == 13 && (module2 == 6 || module2 == 7)) || (module1 == 17 && (module2 == 22 || module2 == 23)) || (module1 == 29 && (module2 == 22 || module2 == 23)) || (module1 == 2 && (module2 == 10 || module2 == 11)) || (module1 == 14 && (module2 == 10 || module2 == 11)) || (module1 == 19 && (module2 == 24 || module2 == 25)) || (module1 == 31 && (module2 == 24 || module2 == 25)) || (module1 == 3 && (module2 == 8 || module2 == 9)) || (module1 == 15 && (module2 == 8 || module2 == 9)) || (module1 == 18 && (module2 == 26 || module2 == 27)) || (module1 == 30 && (module2 == 26 || module2 == 27))) return 1;
  else return 0;

}

// Function to match CRT modules below is for MCC and the 2nd is for data
bool CRT::SingleCRTMatchingProducer::moduleMatcherData(int module1, int module2) {
  // Function checking if two hits could reasonably be matched into a 2D hit
  if ((module1 == 0 && (module2 == 5 || module2 == 4)) || (module1 == 12 && (module2 == 5 || module2 == 4)) || (module1 == 16 && (module2 == 20 || module2 == 21)) || (module1 == 28 && (module2 == 20 || module2 == 21)) || (module1 == 1 && (module2 == 6 || module2 == 7)) || (module1 == 13 && (module2 == 6 || module2 == 7)) || (module1 == 17 && (module2 == 22 || module2 == 23)) || (module1 == 29 && (module2 == 22 || module2 == 23)) || (module1 == 2 && (module2 == 10 || module2 == 11)) || (module1 == 14 && (module2 == 10 || module2 == 11)) || (module1 == 19 && (module2 == 24 || module2 == 25)) || (module1 == 31 && (module2 == 24 || module2 == 25)) || (module1 == 3 && (module2 == 8 || module2 == 9)) || (module1 == 15 && (module2 == 8 || module2 == 9)) || (module1 == 18 && (module2 == 26 || module2 == 27)) || (module1 == 30 && (module2 == 26 || module2 == 27))) return 1;
  else return 0;

}
*/

int CRT::SingleCRTMatchingProducer::moduletoCTB(int module2, int module1){
  if (module1 == 14 && module2 == 11 ) return 15;
  else if (module1 == 14 &&  module2 == 10) return 10;
  else if (module1 == 6 &&  module2 == 11) return 8;
  else if (module1 == 6 &&  module2 == 10) return 9;
  else if (module1 == 18 &&  module2 == 25) return 4;
  else if (module1 == 18 &&  module2 == 24) return 13;
  else if (module1 == 30 &&  module2 == 25) return 3;
  else if (module1 == 30 &&  module2 == 24) return 2;
  else if (module1 == 31 &&  module2 == 27) return 1;
  else if (module1 == 31 &&  module2 == 26) return 0;
  else if (module1 == 19 &&  module2 == 27) return 12;
  else if (module1 == 19 &&  module2 == 26) return 11;
  else if (module1 == 7  &&  module2 == 12) return 7;
  else if (module1 == 7 &&  module2 == 13) return 6;
  else if (module1 == 15  &&  module2 == 12) return 14;
  else if (module1 == 15 &&  module2 == 13) return 5;
  else if (module1 == 1 &&  module2 == 4) return 25;
  else if (module1 == 1 &&  module2 == 5) return 24;
  else if (module1 == 9 &&  module2 == 4) return 26;
  else if (module1 == 9 &&  module2 == 5) return 31;
  else if (module1 == 16 &&  module2 == 20) return 27;
  else if (module1 == 16 &&  module2 == 21) return 28;
  else if (module1 == 28 &&  module2 == 20) return 16;
  else if (module1 == 28 &&  module2 == 21) return 17;
  else if (module1 == 29 &&  module2 == 22) return 18;
  else if (module1 == 29 &&  module2 == 23) return 19;
  else if (module1 == 17 &&  module2 == 22) return 29;
  else if (module1 == 17 &&  module2 == 23) return 20;
  else if (module1 == 8 &&  module2 == 2) return 30;
  else if (module1 == 8 &&  module2 == 3) return 21;
  else if (module1 == 0 &&  module2 == 2) return 23;
  else if (module1 == 0 &&  module2 == 3) return 22;
  else return -1;
}

double CRT::SingleCRTMatchingProducer::setAngle(double angle) {
  if (angle < 0) {
    angle += 3.14159265359;
  }
  angle *= 180.00 / 3.14159265359;
  return angle;
}

//Turn sim::AuxDetSimChannels into CRT::Hits. 
void CRT::SingleCRTMatchingProducer::produce(art::Event & event)
{

  std::unique_ptr< std::vector<anab::T0> > T0col( new std::vector<anab::T0>);
 
  auto CRTTrack=std::make_unique< std::vector< anab::CosmicTag > > ();

 std::unique_ptr< art::Assns<anab::CosmicTag, anab::T0> > CRTT0assn( new art::Assns<anab::CosmicTag, anab::T0>);

 std::unique_ptr< art::Assns<recob::Track, anab::CosmicTag> > TPCCRTassn( new art::Assns<recob::Track, anab::CosmicTag>);
 std::unique_ptr< art::Assns<recob::Track, anab::T0> > TPCT0assn( new art::Assns<recob::Track, anab::T0>);

 std::unique_ptr< art::Assns<CRT::Trigger, anab::CosmicTag>> CRTTriggerassn( new art::Assns<CRT::Trigger, anab::CosmicTag>);

  if (fMCCSwitch){
    fModuleSwitch=1;
    fADCThreshold=800;
    fModuletoModuleTimingCut=4;
    fFronttoBackTimingCut=100;
    fOpCRTTDiffCut=200;
    
}
  else {
    fModuleSwitch=0;
    fADCThreshold=10;
    fModuletoModuleTimingCut=5;
    fFronttoBackTimingCut=8;
    fOpCRTTDiffCut=1000000;


}

/*   if(!fMCCSwitch){
   art::ValidHandle<std::vector<raw::RDTimeStamp>> timingHandle = event.getValidHandle<std::vector<raw::RDTimeStamp>>("timingrawdecoder:daq");
   //const auto& pdspctbs = event.getValidHandle<std::vector<raw::ctb::pdspctb>>(fCTB_tag);

	//const raw::RDTimeStamp& timeStamp = timingHandle->at(0);
	//if(timeStamp.GetFlags()!= 13) {event.put(std::move(CRTTrack)); event.put(std::move(T0col)); event.put(std::move(TPCCRTassn)); event.put(std::move(CRTT0assn));  event.put(std::move(TPCT0assn)); return;}
 }*/

  int nHits = 0;
  art::ServiceHandle < cheat::BackTrackerService > backTracker;
  art::ServiceHandle < cheat::ParticleInventoryService > partInventory;

	auto const* detectorPropertiesService = lar::providerFrom<detinfo::DetectorPropertiesService>();


  primaryHits_F.clear();
  primaryHits_B.clear();
  tracksPair_F.clear();
  tracksPair_B.clear();
  tempHits_F.clear();
  tempHits_B.clear(); // Arrays to compile hits and move them through
  primaryHits_F.clear();
    //allTracksPair.clear();
  logFile.open("ProtoDUNE.log"); // Logfile I don't use right now

  //Get triggers
  cout << "Getting triggers" << endl;
  const auto & triggers = event.getValidHandle < std::vector < CRT::Trigger >> (fCRTLabel);

  art::FindManyP < sim::AuxDetSimChannel > trigToSim(triggers, event, fCRTLabel);

  art::Handle < vector < CRT::Trigger > > crtListHandle;
  vector < art::Ptr < CRT::Trigger > > crtList;
  if (event.getByLabel(fCRTLabel, crtListHandle)) {
    art::fill_ptr_vector(crtList, crtListHandle);
  }

  //Get a handle to the Geometry service to look up AuxDetGeos from module numbers
  art::ServiceHandle < geo::Geometry > geom;

  //Mapping from channel to trigger
  std::unordered_map < size_t, double > prevTimes;
  int hitID = 0;
  int trigID=0;
  for (const auto & trigger: * triggers) {
    const auto & hits = trigger.Hits();
    for (const auto & hit: hits) { // Collect hits on all modules
      if (hit.ADC() > fADCThreshold) { // Keep if they are above threshold

        tempHits tHits;
	if (!fMCCSwitch){
	art::ValidHandle<std::vector<raw::RDTimeStamp>> timingHandle = event.getValidHandle<std::vector<raw::RDTimeStamp>>("timingrawdecoder:daq");

        tHits.module = trigger.Channel(); // Values to add to array
        tHits.channelGeo = hit.Channel();
	tHits.channel=hit.Channel();
        tHits.adc = hit.ADC();
	tHits.triggerTime=trigger.Timestamp()-timingHandle->at(0).GetTimeStamp();
	}
	else{
        tHits.module = trigger.Channel(); // Values to add to array
        tHits.channelGeo = hit.Channel();
	tHits.channel=hit.Channel();
        tHits.adc = hit.ADC();
	tHits.triggerTime=trigger.Timestamp();
        tHits.triggerNumber=trigID;
	}
	 //cout<<trigger.Channel()<<','<<hit.Channel()<<','<<hit.ADC()<<endl;
        nHits++;

        const auto & trigGeo = geom -> AuxDet(trigger.Channel()); // Get geo  
        const auto & csens = trigGeo.SensitiveVolume(hit.Channel());
        const auto center = csens.GetCenter();
        if (center.Z() < 100) tempHits_F.push_back(tHits); // Sort F/B from Z
        else tempHits_B.push_back(tHits);
        hitID++;
      }
    }
    trigID++;
  }

  cout << "Hits compiled for event: " << nEvents << endl;
  cout << "Number of Hits above Threshold:  " << hitID << endl;

 for (unsigned int f = 0; f < tempHits_F.size(); f++) {
    for (unsigned int f_test = 0; f_test < tempHits_F.size(); f_test++) {
      const auto & trigGeo = geom -> AuxDet(tempHits_F[f].module);
      const auto & trigGeo2 = geom -> AuxDet(tempHits_F[f_test].module);
	int flipChannel=tempHits_F[f].channelGeo;
	int flipX=1;
	// v5 geo fixes
	//if (tempHits_F[f].module==21 && !fMCCSwitch){flipX=-1; flipChannel=flipChannel^63;}
	//if (!fMCCSwitch && (tempHits_F[f_test].module==13 || tempHits_F[f_test].module==1)) {flipY=-1; flipChannel=flipChannel^63;}
	//cout<<"Channel flip: "<<flipChannel<<','<<tempHits_F[f_test].channelGeo;
	//if (fabs(tempHits_F[f].triggerTime-tempHits_F[f_test].triggerTime)<1 && tempHits_F[f].module!=tempHits_F[f_test].module){
	//cout<<tempHits_F[f].module<<','<<tempHits_F[f_test].module<<endl;}
      const auto & hit1Geo = trigGeo.SensitiveVolume(flipChannel);
      const auto hit1Center = hit1Geo.GetCenter();
      // Create 2D hits from geo of the Y and X modules
	flipChannel=tempHits_F[f_test].channelGeo;
	int flipY=1;
       const auto & hit2Geo = trigGeo2.SensitiveVolume(flipChannel);
      const auto hit2Center = hit2Geo.GetCenter();
      bool moduleMatched;
      if(fModuleSwitch) moduleMatched=moduleMatcherMCC(tempHits_F[f_test].module, tempHits_F[f].module);
      else moduleMatched=moduleMatcherData(tempHits_F[f_test].module, tempHits_F[f].module);

      if (moduleMatched) {
        // Get the center of the hits (CRT_Res=2.5 cm)
        double hitX = hit1Center.X();
	for (unsigned int a = 0; a < tempHits_F.size(); a++)
	{
	if(tempHits_F[a].module==tempHits_F[f].module && (tempHits_F[a].channelGeo-flipX)==tempHits_F[f].channelGeo) hitX=hit1Center.X()+1.25;
	}
	double hitYPrelim=hit2Center.Y();
	for (unsigned int a = 0; a < tempHits_F.size(); a++)
	{
	if(tempHits_F[a].module==tempHits_F[f_test].module && (tempHits_F[a].channelGeo-flipY)==tempHits_F[f_test].channelGeo) hitYPrelim=hit2Center.Y()+1.25;
	}
	

	
	double hitY=hitYPrelim;
        double hitZ = (hit1Center.Z() + hit2Center.Z()) / 2.f;

        recoHits rHits;
        rHits.adcX=tempHits_F[f].adc;
	rHits.adcY=tempHits_F[f_test].adc;
        rHits.hitPositionX = hitX;
        rHits.hitPositionY = hitY;
        rHits.hitPositionZ = hitZ;
	rHits.moduleX=tempHits_F[f].module;
	rHits.moduleY=tempHits_F[f_test].module;
	rHits.trigNumberX=tempHits_F[f].triggerNumber;
	rHits.trigNumberY=tempHits_F[f_test].triggerNumber;
	rHits.stripX=tempHits_F[f].channel;
	rHits.stripY=tempHits_F[f_test].channel;
	rHits.timeAvg = (tempHits_F[f_test].triggerTime+tempHits_F[f].triggerTime)/2.0;
	if (fabs(tempHits_F[f_test].triggerTime-tempHits_F[f].triggerTime)<fModuletoModuleTimingCut) primaryHits_F.push_back(rHits); // Add array
    }
    }
  }
  for (unsigned int f = 0; f < tempHits_B.size(); f++) {
    for (unsigned int f_test = 0; f_test < tempHits_B.size(); f_test++) { // Same as above but for back CRT
	int channelFlipCheck=tempHits_B[f].module;
	/* Code to fix v5 geo issues in data
	if (!fMCCSwitch){   
	if (channelFlipCheck==8) channelFlipCheck=11;
	else if (channelFlipCheck==11) channelFlipCheck=8;
         else if (channelFlipCheck==10) channelFlipCheck=9;
	else if (channelFlipCheck==9) channelFlipCheck=10;

	else if (channelFlipCheck==26) channelFlipCheck=25;
        else if (channelFlipCheck==25) channelFlipCheck=26;
	else if (channelFlipCheck==24) channelFlipCheck=27;
	else if (channelFlipCheck==27) channelFlipCheck=24;
	}

     if (!fMCCSwitch && (tempHits_B[f].module==25 || tempHits_B[f].module==11 || tempHits_B[f].module==24 || tempHits_B[f].module==10)){flipX=-1; flipChannel=flipChannel^63;}

	//if (!fMCCSwitch && (tempHits_B[f_test].module==2 || tempHits_B[f_test].module==3  || tempHits_B[f_test].module==14 || tempHits_B[f_test].module==15)) {flipY=-1; flipChannel=flipChannel^63;}
	*/
     int flipX=1;
     int flipY=1;
     int flipChannel=tempHits_B[f].channelGeo;

 
      const auto & trigGeo = geom -> AuxDet(channelFlipCheck);
      const auto & trigGeo2 = geom -> AuxDet(tempHits_B[f_test].module);
      const auto & hit1Geo = trigGeo.SensitiveVolume(flipChannel);
      const auto hit1Center = hit1Geo.GetCenter();
      flipChannel=tempHits_B[f_test].channelGeo;

      const auto & hit2Geo = trigGeo2.SensitiveVolume(flipChannel);
      const auto hit2Center = hit2Geo.GetCenter();
      bool moduleMatched;
      if(fModuleSwitch) moduleMatched=moduleMatcherMCC(tempHits_B[f_test].module, tempHits_B[f].module);
      else moduleMatched=moduleMatcherData(tempHits_B[f_test].module, tempHits_B[f].module);

      if (moduleMatched) {
        double hitX = hit1Center.X();

	
	for (unsigned int a = 0; a < tempHits_B.size(); a++)
	{
	if(tempHits_B[a].module==tempHits_B[f].module && (tempHits_B[a].channelGeo-flipX)==tempHits_B[f].channelGeo) hitX=hit1Center.X()+1.25;
	}
	
        double hitYPrelim = hit2Center.Y();
	
	for (unsigned int a = 0; a < tempHits_B.size(); a++)
	{
	if(tempHits_B[a].module==tempHits_B[f_test].module && (tempHits_B[a].channel-flipY)==tempHits_B[f_test].channel) hitYPrelim=hit2Center.Y()+1.25;
	}
	double hitY=hitYPrelim;

	
        double hitZ = (hit1Center.Z() + hit2Center.Z()) / 2.f;

        recoHits rHits;
        rHits.adcX=tempHits_B[f].adc;
	rHits.adcY=tempHits_B[f_test].adc;
        rHits.hitPositionX = hitX;
        rHits.hitPositionY = hitY;
        rHits.hitPositionZ = hitZ;
	rHits.moduleX=tempHits_B[f].module;
	rHits.moduleY=tempHits_B[f_test].module;
	rHits.stripX=tempHits_B[f].channel;
	rHits.trigNumberX=tempHits_B[f].triggerNumber;
	rHits.trigNumberY=tempHits_B[f_test].triggerNumber;
	rHits.stripY=flipChannel;
	rHits.timeAvg = (tempHits_B[f_test].triggerTime+tempHits_B[f].triggerTime)/2.0;
       if (fabs(tempHits_B[f_test].triggerTime-tempHits_B[f].triggerTime)<fModuletoModuleTimingCut) primaryHits_B.push_back(rHits); 
     //primaryHits_B.push_back(rHits);
	 }
    }
  }
	/*
        int pixel0 = -1;
        int pixel1 = -1;
	if (!fMCCSwitch)
	{
       const auto& pdspctbs = *event.getValidHandle<std::vector<raw::ctb::pdspctb>>(fCTBLabel);
       std::vector<int> uS, dS;
	const size_t npdspctbs = pdspctbs.size();
	for(size_t j=0;j<npdspctbs;++j)
	  {
	    const std::vector<raw::ctb::Trigger> HLTriggers = pdspctbs[j].GetHLTriggers();
	    const std::vector<raw::ctb::ChStatus> chs = pdspctbs[j].GetChStatusAfterHLTs();
for (size_t k=0; k<HLTriggers.size(); ++k)
	      { 
		//cout<<chs[k].timestamp<<endl;
		int num = chs[k].crt;

	
        const std::string binary = std::bitset<32>(num).to_string();	
	const auto crtmask=chs[k].crt;
         pixel0 = -1;
         pixel1 = -1;
        for (int i = 0; i<32; ++i){
          if (crtmask & (1<<i)){
            if (i<16){
              pixel0 = i;
            }
            else {
              pixel1 = i;
            }
          }
   	}
        if (pixel0!=-1 && pixel1!=1) {
	}
	
	      }
	  }
	}
	*/


     auto const t0CandPtr = art::PtrMaker<anab::T0>(event);
     auto const crtPtr = art::PtrMaker<anab::CosmicTag>(event);	
  // Reconstruciton information
 art::Handle < vector < recob::Track > > trackListHandle;
  vector < art::Ptr < recob::Track > > trackList;
  art::Handle< std::vector<recob::PFParticle> > PFPListHandle; 
  vector<art::Ptr<recob::PFParticle> > pfplist;
  if (event.getByLabel(fTrackModuleLabel, trackListHandle)) {
    art::fill_ptr_vector(trackList, trackListHandle);
  }
  if(event.getByLabel("pandora",PFPListHandle)) art::fill_ptr_vector(pfplist, PFPListHandle);

  art::FindManyP<anab::T0> trk_t0_assn_v(PFPListHandle, event ,"pandora");
    art::FindManyP<recob::PFParticle> pfp_trk_assn(trackListHandle,event,"pandoraTrack");
	art::Handle< std::vector<recob::OpFlash> > opListHandle;
	std::vector<art::Ptr<recob::OpFlash> > opHitList;

	if (event.getByLabel(fopModuleLabel, opListHandle))
	    {
		art::fill_ptr_vector(opHitList, opListHandle);
	    }
  int nTracksReco = trackList.size();
  art::FindManyP < recob::Hit > hitsFromTrack(trackListHandle, event, fTrackModuleLabel);
  int tempId = 0;
  for (int iRecoTrack = 0; iRecoTrack < nTracksReco; ++iRecoTrack) {
    if (primaryHits_F.size()+primaryHits_B.size()<1) break;
   std::vector< art::Ptr<recob::Hit> > allHits =  hitsFromTrack.at(iRecoTrack);

      art::Ptr<recob::Track> ptrack(trackListHandle, iRecoTrack);
     
	std::vector<art::Ptr<recob::PFParticle>> pfps=pfp_trk_assn.at(iRecoTrack);
	if(!pfps.size()) continue;
	std::vector<art::Ptr<anab::T0>> t0s=trk_t0_assn_v.at(pfps[0].key());
	if(t0s.size()){ 
	  //auto t0=t0s.at(0);
	  //int t_zero=t0->Time();
	  //cout<<"Pandora T0: "<<t_zero<<endl;
   	}
    int firstHit=0;
    int lastHit=allHits.size()-2;


// Get track positions and find angles
 //  if (fMCCSwith==1){
    double trackStartPositionZ_noSCE = trackList[iRecoTrack]->Vertex().Z();
    double trackEndPositionZ_noSCE = trackList[iRecoTrack] -> End().Z();

    double trackStartPositionX_noSCE = trackList[iRecoTrack]->Vertex().X();
    double trackStartPositionY_noSCE = trackList[iRecoTrack]->Vertex().Y();


    double trackEndPositionX_noSCE = trackList[iRecoTrack] -> End().X();
    double trackEndPositionY_noSCE = trackList[iRecoTrack] -> End().Y();

   if (trackStartPositionZ_noSCE>trackEndPositionZ_noSCE){
    trackEndPositionZ_noSCE = trackList[iRecoTrack]->Vertex().Z();
    trackStartPositionZ_noSCE = trackList[iRecoTrack] -> End().Z();
    trackEndPositionX_noSCE = trackList[iRecoTrack]->Vertex().X();
    trackEndPositionY_noSCE = trackList[iRecoTrack]->Vertex().Y();


    trackStartPositionX_noSCE = trackList[iRecoTrack] -> End().X();
    trackStartPositionY_noSCE = trackList[iRecoTrack] -> End().Y();
    firstHit=lastHit;
    lastHit=0;
    }
 if ((trackEndPositionZ_noSCE>90 && trackEndPositionZ_noSCE < 660 && trackStartPositionZ_noSCE <50 && trackStartPositionZ_noSCE<660) || (trackStartPositionZ_noSCE>90 && trackStartPositionZ_noSCE < 660 && trackEndPositionZ_noSCE <50 && trackEndPositionZ_noSCE<660)) {
      for (unsigned int iHit_F = 0; iHit_F < primaryHits_F.size(); iHit_F++) {
   double xOffset=0;

		double trackStartPositionX_notCorrected=trackStartPositionX_noSCE;
		double trackEndPositionX_notCorrected=trackEndPositionX_noSCE;
		if (t0s.empty()){
		int RDOffset=0;
		if (!fMCCSwitch) RDOffset=111;
		double ticksOffset=0;
		//cout<<(primaryHits_F[iHit_F].timeAvg+RDOffset)<<endl;
		//cout<<detectorPropertiesService->GetXTicksOffset(allHits[firstHit]->WireID().Plane, allHits[firstHit]->WireID().TPC, allHits[firstHit]->WireID().Cryostat)<<endl;
		if (!fMCCSwitch) ticksOffset = (primaryHits_F[iHit_F].timeAvg+RDOffset)/25.f+detectorPropertiesService->GetXTicksOffset(allHits[firstHit]->WireID().Plane, allHits[firstHit]->WireID().TPC, allHits[firstHit]->WireID().Cryostat);

		else if (fMCCSwitch) ticksOffset = (primaryHits_F[iHit_F].timeAvg/500.f)+detectorPropertiesService->GetXTicksOffset(allHits[firstHit]->WireID().Plane, allHits[firstHit]->WireID().TPC, allHits[firstHit]->WireID().Cryostat);
		
	       xOffset=detectorPropertiesService->ConvertTicksToX(ticksOffset,allHits[firstHit]->WireID().Plane, allHits[firstHit]->WireID().TPC, allHits[firstHit]->WireID().Cryostat);
		//double xOffset=.08*ticksOffset
		
	 trackStartPositionX_noSCE=trackStartPositionX_notCorrected-xOffset;
         trackEndPositionX_noSCE=trackEndPositionX_notCorrected-xOffset;
	}

   double trackStartPositionX=trackStartPositionX_noSCE;
   double trackStartPositionY=trackStartPositionY_noSCE;
   double trackStartPositionZ=trackStartPositionZ_noSCE;

   double trackEndPositionX=trackEndPositionX_noSCE;
   double trackEndPositionY=trackEndPositionY_noSCE;
   double trackEndPositionZ=trackEndPositionZ_noSCE;
	//if (!fMCCSwitch && moduletoCTB(primaryHits_F[iHit_F].moduleX, primaryHits_F[iHit_F].moduleY)!=pixel0) continue;
        double X1 = primaryHits_F[iHit_F].hitPositionX;

        double Y1 = primaryHits_F[iHit_F].hitPositionY;

        double Z1 = primaryHits_F[iHit_F].hitPositionZ;

	// Make metrics for a CRT pair to compare later
	TVector3 trackStart(trackStartPositionX, trackStartPositionY, trackStartPositionZ);
	TVector3 trackEnd(trackEndPositionX, trackEndPositionY, trackEndPositionZ);
	TVector3 v1(X1,Y1,Z1);
	TVector3 v2(trackStartPositionX, trackStartPositionY, trackStartPositionZ);

            TVector3 v4(trackStartPositionX,
                        trackStartPositionY,
                        trackStartPositionZ);
            TVector3 v5(trackEndPositionX,
                        trackEndPositionY,
                        trackEndPositionZ);
	TVector3 trackVector = (v5-v4).Unit();
	TVector3 hitVector=(v2-v1).Unit();




              double predictedHitPositionY1 = (v1.Z()-v5.Z())/(v4.Z()-v5.Z())*(v4.Y()-v5.Y())+v5.Y();


              double predictedHitPositionX1 = (v1.Z()-v5.Z())/(v4.Z()-v5.Z())*(v4.X()-v5.X())+v5.X();

	double dotProductCos=trackVector*hitVector;

        double deltaX1 = (predictedHitPositionX1-X1);

	double deltaX=(deltaX1);

        double deltaY1 = (predictedHitPositionY1-Y1);

	double deltaY=deltaY1;
	double minTimeDifference=999999.99;
        for (unsigned int iFlash = 0; iFlash < opHitList.size(); iFlash++)
                {
		    if (opHitList[iFlash]->TotalPE()>100){

                    double flashTime = opHitList[iFlash]->Time();

                    double timeDifference = primaryHits_F[iHit_F].timeAvg - flashTime*1000;
		     //cout<<flashTime<<','<<primaryHits_F[iHit_F].timeAvg <<endl;
                    if(fabs(timeDifference) < fabs(minTimeDifference))
                        {
                            minTimeDifference = timeDifference;
			   //if (minTimeDifference<1000) cout<<"Min Time: "<<minTimeDifference<<endl;
                        }
		    }
                }

         tracksPair tPair;
        tPair.tempId = tempId;
        tPair.CRTTrackId = iHit_F;
        tPair.recoId = iRecoTrack;

	tPair.deltaX=deltaX;
        tPair.deltaY=deltaY;
        tPair.dotProductCos=dotProductCos;

        tPair.moduleX1 = primaryHits_F[iHit_F].moduleX;
        tPair.moduleY1 = primaryHits_F[iHit_F].moduleY;

        tPair.adcX1=primaryHits_F[iHit_F].adcX;
        tPair.adcY1=primaryHits_F[iHit_F].adcY;

        tPair.stripX1 = primaryHits_F[iHit_F].stripX;
        tPair.stripY1 = primaryHits_F[iHit_F].stripY;
        tPair.trigNumberX = primaryHits_F[iHit_F].trigNumberX;
        tPair.trigNumberY = primaryHits_F[iHit_F].trigNumberY;
        tPair.X1 = X1;
        tPair.Y1 = Y1;
        tPair.Z1 = Z1;
	tPair.timeAvg=primaryHits_F[iHit_F].timeAvg;
        tPair.trackStartPosition=trackStart;
	tPair.flashTDiff=minTimeDifference;
	tPair.trackEndPosition=trackEnd;
        tracksPair_B.push_back(tPair);

      }
	}
 if ( (trackStartPositionZ_noSCE<620 && trackEndPositionZ_noSCE > 660 && trackStartPositionZ_noSCE > 50 && trackEndPositionZ_noSCE > 50) || (trackStartPositionZ_noSCE>660 && trackEndPositionZ_noSCE < 620 && trackStartPositionZ_noSCE > 50 && trackEndPositionZ_noSCE > 50)) {
      for (unsigned int iHit_B = 0; iHit_B < primaryHits_B.size(); iHit_B++) {
double xOffset=0;
    

		double trackStartPositionX_notCorrected=trackStartPositionX_noSCE;
		double trackEndPositionX_notCorrected=trackEndPositionX_noSCE;
		if (t0s.empty()){
		int RDOffset=0;
		if (!fMCCSwitch) RDOffset=111;
		double ticksOffset=0;
		if (!fMCCSwitch) ticksOffset = (primaryHits_B[iHit_B].timeAvg+RDOffset)/25.f+detectorPropertiesService->GetXTicksOffset(allHits[firstHit]->WireID().Plane, allHits[firstHit]->WireID().TPC, allHits[firstHit]->WireID().Cryostat);

		else if (fMCCSwitch) ticksOffset = (primaryHits_B[iHit_B].timeAvg/500.f)+detectorPropertiesService->GetXTicksOffset(allHits[firstHit]->WireID().Plane, allHits[firstHit]->WireID().TPC, allHits[firstHit]->WireID().Cryostat);
		
	       xOffset=detectorPropertiesService->ConvertTicksToX(ticksOffset,allHits[firstHit]->WireID().Plane, allHits[firstHit]->WireID().TPC, allHits[firstHit]->WireID().Cryostat);
		//double xOffset=.08*ticksOffset
		
	 trackStartPositionX_noSCE=trackStartPositionX_notCorrected-xOffset;
         trackEndPositionX_noSCE=trackEndPositionX_notCorrected-xOffset;
	}

   double trackStartPositionX=trackStartPositionX_noSCE;
   double trackStartPositionY=trackStartPositionY_noSCE;
   double trackStartPositionZ=trackStartPositionZ_noSCE;

   double trackEndPositionX=trackEndPositionX_noSCE;
   double trackEndPositionY=trackEndPositionY_noSCE;
   double trackEndPositionZ=trackEndPositionZ_noSCE;

	//if (!fMCCSwitch && moduletoCTB(primaryHits_B[iHit_B].moduleX, primaryHits_B[iHit_B].moduleY)!=pixel1) continue;
        double X1 = primaryHits_B[iHit_B].hitPositionX;

        double Y1 = primaryHits_B[iHit_B].hitPositionY;

        double Z1 = primaryHits_B[iHit_B].hitPositionZ;


 
	// Make metrics for a CRT pair to compare later
	TVector3 trackStart(trackStartPositionX, trackStartPositionY, trackStartPositionZ);
	TVector3 trackEnd(trackEndPositionX, trackEndPositionY, trackEndPositionZ);
	TVector3 v1(X1,Y1,Z1);
	TVector3 v2(trackStartPositionX, trackStartPositionY, trackStartPositionZ);

            TVector3 v4(trackStartPositionX,
                        trackStartPositionY,
                        trackStartPositionZ);
            TVector3 v5(trackEndPositionX,
                        trackEndPositionY,
                        trackEndPositionZ);
	TVector3 trackVector = (v5-v4).Unit();
	TVector3 hitVector=(v2-v1).Unit();




              double predictedHitPositionY1 = (v1.Z()-v5.Z())/(v4.Z()-v5.Z())*(v4.Y()-v5.Y())+v5.Y();


              double predictedHitPositionX1 = (v1.Z()-v5.Z())/(v4.Z()-v5.Z())*(v4.X()-v5.X())+v5.X();

	double dotProductCos=trackVector*hitVector;

        double deltaX1 = (predictedHitPositionX1-X1);

	double deltaX=(deltaX1);

        double deltaY1 = (predictedHitPositionY1-Y1);

	double deltaY=(deltaY1);
	//cout<<"Delta B: "<<deltaX<<','<<deltaY<<endl;
	double minTimeDifference=999999.99;
       for (unsigned int iFlash = 0; iFlash < opHitList.size(); iFlash++)
                {
   		    if (opHitList[iFlash]->TotalPE()>100){
                    double hitTime = opHitList[iFlash]->Time();
                    double timeDifference = primaryHits_B[iHit_B].timeAvg - hitTime*1000;

                    if(fabs(timeDifference) < fabs(minTimeDifference))
                        {
                            minTimeDifference = timeDifference;
                        }
			}
                }

        tracksPair tPair;
        tPair.tempId = tempId;
        tPair.CRTTrackId = iHit_B;
        tPair.recoId = iRecoTrack;

	tPair.deltaX=deltaX;
        tPair.deltaY=deltaY;
        tPair.dotProductCos=dotProductCos;

        tPair.moduleX1 = primaryHits_B[iHit_B].moduleX;
        tPair.moduleY1 = primaryHits_B[iHit_B].moduleY;

        tPair.adcX1=primaryHits_B[iHit_B].adcX;
        tPair.adcY1=primaryHits_B[iHit_B].adcY;

        tPair.stripX1 = primaryHits_B[iHit_B].stripX;
        tPair.stripY1 = primaryHits_B[iHit_B].stripY;
       tPair.trigNumberX = primaryHits_B[iHit_B].trigNumberX;
        tPair.trigNumberY = primaryHits_B[iHit_B].trigNumberY;
        tPair.X1 = X1;
        tPair.Y1 = Y1;
        tPair.Z1 = Z1;
	tPair.timeAvg=primaryHits_B[iHit_B].timeAvg;
	tPair.flashTDiff=minTimeDifference;
        tPair.trackStartPosition=trackStart;
	tPair.trackEndPosition=trackEnd;
        tracksPair_B.push_back(tPair);

      }

      tempId++;
    } //iRecoTrack
    }

     //Sort pair by ascending order of absolute distance
    sort(tracksPair_F.begin(), tracksPair_F.end(), sortPair());
    sort(tracksPair_B.begin(), tracksPair_B.end(), sortPair());


    vector < tracksPair > allUniqueTracksPair;
    while (tracksPair_F.size()) {
      allUniqueTracksPair.push_back(tracksPair_F.front());
      tracksPair_F.erase(remove_if(tracksPair_F.begin(), tracksPair_F.end(), removePairIndex(tracksPair_F.front())),
        tracksPair_F.end());
    }

    while (tracksPair_B.size()) {
      allUniqueTracksPair.push_back(tracksPair_B.front());
      tracksPair_B.erase(remove_if(tracksPair_B.begin(), tracksPair_B.end(), removePairIndex(tracksPair_B.front())),
        tracksPair_B.end());
    }

	//cout<<"Number of reco and CRT pairs: "<<allUniqueTracksPair.size()<<endl;
    if (allUniqueTracksPair.size() > 0) {
      for (unsigned int u = 0; u < allUniqueTracksPair.size(); u++) {

	int CRTTrackId=allUniqueTracksPair[u].CRTTrackId;
	int TPCTrackId=allUniqueTracksPair[u].recoId;
	deltaX=allUniqueTracksPair[u].deltaX;

	deltaY=allUniqueTracksPair[u].deltaY;
	opCRTTDiff=allUniqueTracksPair[u].flashTDiff;

	dotCos=fabs(allUniqueTracksPair[u].dotProductCos);
	trackX1=allUniqueTracksPair[u].trackStartPosition.X();
	trackY1=allUniqueTracksPair[u].trackStartPosition.Y();
	trackZ1=allUniqueTracksPair[u].trackStartPosition.Z();

	trackX2=allUniqueTracksPair[u].trackEndPosition.X();
	trackY2=allUniqueTracksPair[u].trackEndPosition.Y();
	trackZ2=allUniqueTracksPair[u].trackEndPosition.Z();

	moduleX=allUniqueTracksPair[u].moduleX1;
	moduleY=allUniqueTracksPair[u].moduleY1;

	adcX=allUniqueTracksPair[u].adcX1;
	adcY=allUniqueTracksPair[u].adcY1;

	CRTT0=allUniqueTracksPair[u].timeAvg;
	stripX=allUniqueTracksPair[u].stripX1;
	stripY=allUniqueTracksPair[u].stripY1;

	X_CRT=allUniqueTracksPair[u].X1;
	Y_CRT=allUniqueTracksPair[u].Y1;
	Z_CRT=allUniqueTracksPair[u].Z1;

       	flashTime=-1*opCRTTDiff-CRTT0;
        if ( fabs(trackX1)<400 &&  fabs(trackX2)<400 && fabs(allUniqueTracksPair[u].dotProductCos)>0.9993 && fabs(deltaX)<40 &&  fabs(deltaY)<40) {
	cout<<"Found Matched Single CRT Tag with CRT*TPC: "<<fabs(allUniqueTracksPair[u].dotProductCos)<<endl;

	fCRTTree->Fill();
	std::vector<float> hitF;
	std::vector<float> hitB;
	hitF.push_back(X_CRT); hitF.push_back(Y_CRT); hitF.push_back(Z_CRT);
	hitB.push_back(trackX1); hitB.push_back(trackY1); hitB.push_back(trackZ1);
	CRTTrack->push_back(anab::CosmicTag(hitF,hitB, fabs(allUniqueTracksPair[u].dotProductCos),anab::CosmicTagID_t::kUnknown));
	if (Z_CRT<100) T0col->push_back(anab::T0(CRTT0, 1, 1,CRTTrackId,fabs(allUniqueTracksPair[u].dotProductCos)));
        else T0col->push_back(anab::T0(CRTT0, 2, 1,CRTTrackId,fabs(allUniqueTracksPair[u].dotProductCos) ));
	auto const crtTrackPtr = crtPtr(CRTTrack->size()-1);
	auto const t0CP = t0CandPtr(CRTTrackId);
	CRTT0assn->addSingle(crtTrackPtr,t0CP);
	
	
       
	util::CreateAssn(*this, event, *T0col, trackList[TPCTrackId], *TPCT0assn);
	util::CreateAssn(*this, event, *CRTTrack, trackList[TPCTrackId], *TPCCRTassn);
	util::CreateAssn(*this, event, *CRTTrack, crtList[allUniqueTracksPair[u].trigNumberX], *CRTTriggerassn);
	util::CreateAssn(*this, event, *CRTTrack, crtList[allUniqueTracksPair[u].trigNumberY], *CRTTriggerassn);

        }
      }
    }
  nEvents++;
event.put(std::move(T0col)); event.put(std::move(CRTTrack)); event.put(std::move(TPCCRTassn));  event.put(std::move(TPCT0assn)); event.put(std::move(CRTT0assn)); event.put(std::move(CRTTriggerassn));

}


void CRT::SingleCRTMatchingProducer::beginJob() {

	art::ServiceHandle<art::TFileService> fileServiceHandle;
       fCRTTree = fileServiceHandle->make<TTree>("Displacement", "event by event info");
	fCRTTree->Branch("nEvents", &nEvents, "fnEvents/I");


	fCRTTree->Branch("hdeltaX", &deltaX, "deltaX/D");
	fCRTTree->Branch("hdeltaY", &deltaY, "deltaY/D");
	fCRTTree->Branch("hdotProductCos", &dotCos, "dotCos/D");


	fCRTTree->Branch("hX_CRT", &X_CRT, "X_CRT/D");
	fCRTTree->Branch("hY_CRT", &Y_CRT, "Y_CRT/D");
	fCRTTree->Branch("hZ_CRT", &Z_CRT, "Z_CRT/D");

	fCRTTree->Branch("hCRTT0", &CRTT0, "CRTT0/D");


	fCRTTree->Branch("htrackStartX", &trackX1, "trackX1/D");
	fCRTTree->Branch("htrackStartY", &trackY1, "trackY1/D");
	fCRTTree->Branch("htrackStartZ", &trackZ1, "trackZ1/D");

	fCRTTree->Branch("htrackEndX", &trackX2, "trackX2/D");
	fCRTTree->Branch("htrackEndY", &trackY2, "trackY2/D");
	fCRTTree->Branch("htrackEndZ", &trackZ2, "trackZ2/D");

	fCRTTree->Branch("hmoduleX", &moduleX, "moduleX/I");
	fCRTTree->Branch("hmoduleY", &moduleY, "moduleY/I");

	fCRTTree->Branch("hstripX", &stripX, "stripX/I");
	fCRTTree->Branch("hstripY", &stripY, "stripY/I");

	fCRTTree->Branch("hadcX", &adcX, "adcX/I");
	fCRTTree->Branch("hadcY", &adcY, "adcY/I");

	fCRTTree->Branch("hopCRTTDiff", &opCRTTDiff, "opCRTTDiff/D");
	fCRTTree->Branch("hflashTime", &flashTime, "flashTime/D");


}
// Endjob actions
void CRT::SingleCRTMatchingProducer::endJob() 
{



}


DEFINE_ART_MODULE(CRT::SingleCRTMatchingProducer)
