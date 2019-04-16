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

int main()
{
	NetworkImporter pImporter;

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

		//This boolean will control the continuity of the search
		//When all the Heuristics do not find a better solution the search is done
		flowControl = false;

		//First Metaheuristic: Removing capacitors
		//Github README
		flowControl |= pRemoveCap.execute(currentSolution, pCirc);

		//Second Metaheuristic: Adding capacitors
		//Github README
		flowControl |= pAddCapBusHasCap.execute(currentSolution, pCirc);

		//Third Metaheuristic: Adding capacitors to busses that do not have capacitors
		//Github README
		flowControl |= pAddCapBusHasNoCap.execute(currentSolution, initialSolution, pCirc);

		//Fourth Metaheuristic: Adding capacitors to a neighbour form a bus that has capacitor
		//Github README
		flowControl |= pAddCapNeighbour.execute(currentSolution, pCirc);

	} while (flowControl == true);

	//TODO check all Heuristics: removeCap already reviewed
	//TODO check if capacitors are really being added to busses
	//TODO refactor equipments classes
	//TODO change access specifiers from classes from public to private when possible
	//TODO make a report after the search is done
	//TODO understand if there is a relationship between the final result and the initial number of busses analyzed
	return 0;
}

