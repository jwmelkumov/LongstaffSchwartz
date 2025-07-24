#pragma once
#include "AssetSimulator.h"
#include "OptionPricer.h"
#include <string>

struct LsmResult {
    double price;             
    double expExerciseStep;   
    double expExercisePayoff; 
};

class LongstaffSchwartzEngine {
public:
    LongstaffSchwartzEngine(AssetSimulator&   sim,
                            OptionPricer&     pricer,
                            int               polyOrder,
                            const std::string& csvFile);   

    LsmResult price();       

private:
    AssetSimulator& sim_;
    OptionPricer&   pricer_;
    int             polyOrder_;
    std::string     csvFile_; 
};
