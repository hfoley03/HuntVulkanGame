#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"
#include "generateScope.hpp"
#include "gameParameters.h"
#include "gameStructs.h"
#include "utils.hpp"
#include "objectInstances.hpp"
#include <cstdlib> 
#include <ctime>
#include <string>
#include <unistd.h>







bool isDebugMode = false;	// Debug mode
float dayCyclePhase;		// Starting moment of the day

// TEXTS
std::vector<SingleText> outText;		// Menu screen
std::vector<SingleText> outTimeText;	// Time counter
std::vector<SingleText> outAnimalText;	// Animal counter


std::vector<Instance> balls;
std::vector<Instance> vegRocks; 
std::vector<Instance> structures;
std::vector<Instance> animals;