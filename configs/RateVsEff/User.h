
// *** Default user settings *** //
TString IN_DIR_NAME   = ".";          // Directory for input ROOT files
TString OUT_DIR_NAME  = "/home/ws13/TMVA/TMVA/EMTFPtAssign2018";      // Directory for output ROOT file
TString OUT_FILE_NAME = "RateVsEff";  // Name base for output ROOT file
std::vector<PtAlgo> ALGOS    = {};    // Vector of factory-MVA-mode sets for evaluation
std::vector<int>    EFF_CUTS = {};    // Vector of efficiency thresholds (%)
std::vector<int>    TURN_ONS = {};    // Vector of pT cuts for turn-on curves

namespace RateVsEff_cfg {

  inline void ConfigureUser( const TString USER ) {

    std::cout << "\nConfiguring RateVsEff code for user " << USER << std::endl;

    if (USER == "WEI") {
      const int MODE = 15;  // Specify one mode in particular to look at

      IN_DIR_NAME   = "/home/ws13/TMVA/TMVA/EMTFPtAssign2018";
      TString out_str;
      out_str.Form("RateVsEff_mode_%d", MODE);
      OUT_FILE_NAME = out_str;

      EFF_CUTS    = {1};
      TURN_ONS    = {8, 16, 24};

      TString in_str;
      TString fact_str;
      TString ID_str;
      TString alias_str;

      PtAlgo EMTF15;  // 2017 EMTF pT algorithm, mode 15
      EMTF15.in_file_name = "PtRegression2018_MODE_15_noBitCompr_noRPC.root";
      EMTF15.fact_name    = "f_MODE_15_logPtTarg_invPtWgt_noBitCompr_noRPC";
      EMTF15.MVA_name     = "EMTF_pt";
      EMTF15.unique_ID    = "EMTF15";
      EMTF15.alias        = "EMTF mode 15";
      EMTF15.modes        = {15};
      EMTF15.modes_CSC    = {15};
      EMTF15.modes_RPC    = {0};  // No RPC hits allowed
      //The scale below is used in firmware to acheive ~90% eff at each threshold in 2017
      //EMTF15.trg_pt_scale = 1.2 / (1 - 0.015*fmin(20., pt) );  // To compare new BDT with 2017
      EMTF15.color        = 1;  // kBlack

      // Mode 15 BDT
      PtAlgo BDT15;
      BDT15.in_file_name = "PtRegression2018_MODE_15_noBitCompr_noRPC.root";
      BDT15.fact_name    = "f_MODE_15_logPtTarg_invPtWgt_noBitCompr_noRPC";
      BDT15.MVA_name     = "BDTG_AWB_Sq";
      BDT15.unique_ID    = "BDT_15_logPtTarg_invPtWgt";
      BDT15.alias        = "logPt target, Least Sq loss";
      BDT15.modes        = {15};
      BDT15.modes_CSC    = {15};
      BDT15.modes_RPC    = {0};
      //BDT15.trg_pt_scale = XXX;  // TBD
      BDT15.color        = 840;  // kTeal

      ALGOS.push_back(EMTF15);  // First algo is always the standard comparison algo
      ALGOS.push_back(BDT15);
    } // End conditional: if (USER == "WEI")

  } // End function: inline void ConfigureUser()

} // End namespace RateVsEff_cfg
