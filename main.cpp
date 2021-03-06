#include "stdafx.h"
#include "NetworkImporter.h"
#include "Circuit.h"
#include "Flow.h"
#include "Metaheuristic.h"
#include "BruteForce.h"
#include "ReconfigureSolutions.h"
#include "RemoveCap.h"
#include "AddCapBusHasCap.h"
#include "AddCapBusHasNoCap.h"
#include "AddCapNeighbour.h"
#include "const.h"
#include <chrono>

int main()
{
	auto start = std::chrono::high_resolution_clock::now();
	int numIter = 0;
	NetworkImporter pImporter;

	//Circuit used as reference to analyze gain at the end of the algorithm
	Circuit* pCircReference;
	pCircReference = pImporter.NetworkFromTxt("84BusGrid.txt");

	//Circuit that will changed by the the algorithm
	Circuit* pCirc;
	pCirc = pImporter.NetworkFromTxt("84BusGrid.txt");

	bool flowControl{ false };
	BruteForce pBruteForce;
	ReconfigureSolutions pReconfigureSolutions;
	RemoveCap pRemoveCap;
	AddCapBusHasCap pAddCapBusHasCap;
	AddCapBusWithNoCap pAddCapBusHasNoCap;
	AddCapNeighbour pAddCapNeighbour;

	//The initial Solution will be the solution from brute force. This is the start from which we'll improve.
	//Current solution is the solution constantly being improved
	std::vector<Solution*> initialSolution, currentSolution;

	//First we use brute force to find the best busses to place the capacitors
	//The initial solution is a sorted list with the gains for each bus with a capacitor placed
	//Github README
	initialSolution = pBruteForce.execute(pCirc);

	//After we get the best bars we place capacitors on them and get the initial solution
	//Github README
	currentSolution = pReconfigureSolutions.execute(initialSolution, pCirc);

	do {
		numIter++;
		//This boolean will control the continuity of the search
		//When all the Heuristics do not find a better solution the search is done
		flowControl = false;

		//First neighbourhood (N1): Removing capacitors
		//Github README
		flowControl |= pRemoveCap.execute(currentSolution, pCirc);
		if (flowControl) continue;

		//Second neighbourhood (N2): Adding capacitors
		//Github README
		flowControl |= pAddCapBusHasCap.execute(currentSolution, pCirc);
		if (flowControl) continue;

		//Third neighbourhood (N3): Adding capacitors to busses that do not have capacitors
		//Github README
		flowControl |= pAddCapBusHasNoCap.execute(currentSolution, initialSolution, pCirc);
		if (flowControl) continue;

		//Fourth neighbourhood (N4): Adding capacitors to a neighbour form a bus that has capacitor
		//Github README
		flowControl |= pAddCapNeighbour.execute(currentSolution, pCirc);
		if (flowControl) continue;

	} while (flowControl == true);
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time = finish - start;
	//Log solution
	Metaheuristic::log(currentSolution, pCircReference, pCirc);
	
	std::cout << Flow::numFlow << " flows" << std::endl;
	std::cout << numIter << " iterations" << std::endl;
	std::cout << time.count() << " seconds" << std::endl;
	return 0;
}