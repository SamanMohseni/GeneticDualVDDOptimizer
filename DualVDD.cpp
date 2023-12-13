/*
*   Author: Seyed Saman Mohseni Sangtabi
*   Student number: 99210067
*   Midterm exam, question 3
*/

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <map>

using namespace std;

#define NOTSET -1

struct CircuitStats{
	double MaxDelay;
	double Power;
};

class Circuit {
public:
	Circuit(){

	}

	void readNetList(){
		cout << "Enter the net list using the following format, without < and >. Finish gate IO with ; seperated with space." << endl;
		cout << "<gate name> <input 1 wire label> <input 2 wire label> ... <input n wire label> <output 1 wire label> ;" << endl;
		cout << "Finish the list by entering # on a new line." << endl;
		cout << "Enter:" << endl;

		int gateCount = 0;
		int wireLabelCount = 0;
		while (true){
			string gate;
			cin >> gate;
			if (gate == "#"){
				break;
			}
			if (gateNumber.count(gate)){
				cerr << "ERROR [invalid input]: gate already entered." << endl;
				exit(1);
			}
			gateNumber.insert(make_pair(gate, gateCount++));

			string wire, preWire;
			vector<int> adjacencyList;
			cin >> wire;
			while (true){
				preWire = wire;
				cin >> wire;
				if (!wireLabelNumber.count(preWire)){
					wireLabelNumber.insert(make_pair(preWire, wireLabelCount++));
				}
				if (wire == ";"){
					if (parents.count(wireLabelNumber[preWire])){
						cerr << "ERROR [invalid input]: 2 output wires overlap." << endl;
						exit(1);
					}
					parents.insert(make_pair(wireLabelNumber[preWire], make_pair(gateNumber[gate], adjacencyList)));
					break;
				}
				adjacencyList.push_back(wireLabelNumber[preWire]);
			}
		}

		wireDelay.resize(wireLabelNumber.size());
		gateHasBufferInOutput.resize(gateNumber.size());
	}

	CircuitStats getStats(vector<bool>* _gateType){
		CircuitStats stats;

		gateType = _gateType;

		fill(wireDelay.begin(), wireDelay.end(), NOTSET);
		fill(gateHasBufferInOutput.begin(), gateHasBufferInOutput.end(), false);

		double MaxDelay = 0;
		for (map<string, int>::iterator it = wireLabelNumber.begin(); it != wireLabelNumber.end(); ++it){
			double delay = getWireDelay(it->second);
			if (delay > MaxDelay){
				MaxDelay = delay;
			}
		}
		stats.MaxDelay = MaxDelay;

		stats.Power = getPower();

		return stats;
	}

	int getNumberOfGates(){
		return gateNumber.size();
	}

private:
	double getWireDelay(int wireNumber){
		if (wireDelay[wireNumber] != NOTSET){
			return wireDelay[wireNumber];
		}
		if (!parents.count(wireNumber)){
			wireDelay[wireNumber] = 0;
			return 0;
		}
		double maxDelay = 0;
		for (int i = 0; i < parents[wireNumber].second.size(); i++){
			int inputWire = parents[wireNumber].second[i];
			double delay = getWireDelay(inputWire);

			if (parents.count(inputWire)){
				if ((*gateType)[parents[inputWire].first] == LVDD && (*gateType)[parents[wireNumber].first] == HVDD){
					delay += SDCVSL_T;
					gateHasBufferInOutput[parents[inputWire].first] = true;
				}
			}// assuming all input wires to the combinational circuit are outputs of HVDD FlipFlops.

			if (delay > maxDelay){
				maxDelay = delay;
			}
		}
		wireDelay[wireNumber] = maxDelay + ((*gateType)[parents[wireNumber].first] ? HVDD_T : LVDD_T);
		return wireDelay[wireNumber];
	}

	double getPower(){
		double power = 0;
		for (int i = 0; i < (*gateType).size(); i++){
			power += (*gateType)[i] ? HVDD_P : LVDD_P;
			power += gateHasBufferInOutput[i] ? SDCVSL_P : 0;
		}
		return power;
	}

public:

	static const bool LVDD = 0;
	static const bool HVDD = 1;

private:
	map<int, pair<int, vector<int>>> parents; // map<output_label_number, pair<gate_number, vector<input_label_number>>>
	map<string, int> gateNumber;
	map<string, int> wireLabelNumber;
	vector<double> wireDelay;
	vector<bool>* gateType;
	vector<bool> gateHasBufferInOutput;

	const double LVDD_T = 1.5;
	const double LVDD_P = 0.5;

	const double HVDD_T = 1;
	const double HVDD_P = 1;

	const double SDCVSL_T = 0.3;
	const double SDCVSL_P = 0.2;
};

class GeneticAlgorithm{
public:
	GeneticAlgorithm(int populationSize, int DNALength, double(*_fitnessFunction)(vector<bool>)){
		fitnessFunction = _fitnessFunction;
		generateRandomPopulation(populationSize, DNALength);
		fitness.resize(populationSize);
	}

	vector<bool> Run(int generations){
		for (int i = 0; i < generations; i++){
			calculateFitnessesRatio(i % 2);
			generateNextGeneration(i % 2);
		}
		calculateFitnessesRatio(generations % 2);
		double max = 0;
		int maxIndex = 0;
		for (int i = 0; i < fitness.size(); i++){
			if (fitness[i] > max){
				max = fitness[i];
				maxIndex = i;
			}
		}
		return population[generations % 2][maxIndex];
	}

private:
	void generateRandomPopulation(int populationSize, int DNALength){
		population[0].resize(populationSize);
		population[1].resize(populationSize);

		for (int i = 0; i < populationSize; i++){
			population[0][i] = generateRandomDNA(DNALength, 0.5);
			population[1][i] = population[0][i];
		}
	}

	vector<bool> generateRandomDNA(int DNALength, double HVDDProbablity){
		vector<bool> DNA;
		DNA.resize(DNALength);
		for (int i = 0; i < DNALength; i++){
			double r = (double)rand() / RAND_MAX;
			DNA[i] = (r <= HVDDProbablity);
		}
		return DNA;
	}

	void calculateFitnessesRatio(int sourceIndex){
		double sum = 0;
		for (int i = 0; i < fitness.size(); i++){
			double f = fitnessFunction(population[sourceIndex][i]);
			sum += f;
			fitness[i] = f;
		}
		for (int i = 0; i < fitness.size(); i++){
			fitness[i] /= sum;
		}
	}

	int randomSelect(){
		double r = (double)rand() / RAND_MAX;
		double sum = 0;
		for (int i = 0; i < fitness.size(); i++){
			sum += fitness[i];
			if (sum >= r){
				return i;
			}
		}
		return fitness.size() - 1;
	}

	void makeChild(vector<bool> &mother, vector<bool> &father, vector<bool> &child){
		for (int i = 0; i < child.size(); i++){
			child[i] = (rand() & 1) ? mother[i] : father[i];
		}
	}

	void mutate(vector<bool> &child){
		int index = rand() % child.size();
		child[index] = !child[index];
	}

	void generateNextGeneration(int sourceIndex){
		for (int i = 0; i < fitness.size(); i++){
			int mother = randomSelect();
			int father = randomSelect();
			makeChild(population[sourceIndex][mother], population[sourceIndex][father], population[1 - sourceIndex][i]);
			if ((double)rand() / RAND_MAX < 0.05){
				mutate(population[1 - sourceIndex][i]);
			}
		}
	}

private:
	vector<vector<bool>> population[2]; // to act as ping-pong arrays
	vector<double> fitness;
	double(*fitnessFunction)(vector<bool>);
};

Circuit circuit;
double minDelay, maxDelay;
double minPower, maxPower;
double maximumAcceptableDelay;

void calculatePowerDelayMargine(){
	vector<bool> gateType(circuit.getNumberOfGates(), Circuit::LVDD);
	CircuitStats stats = circuit.getStats(&gateType);
	maxDelay = stats.MaxDelay;
	minPower = stats.Power;

	fill(gateType.begin(), gateType.end(), Circuit::HVDD);
	stats = circuit.getStats(&gateType);
	minDelay = stats.MaxDelay;
	maxPower = stats.Power;

	cout << "Delay: [lower bound: " << minDelay << "], [upper bound: " << maxDelay << "]" << endl;
	cout << "Power: [lower bound: " << minPower << "], [upper bound: " << maxPower << "]" << endl;
}

double fitnessFunction(vector<bool> gateType){
	CircuitStats stats = circuit.getStats(&gateType);

	double powerFitness = maxPower / stats.Power;
	double normalizedDelay = (stats.MaxDelay - maximumAcceptableDelay - 0.05) / (maxDelay - minDelay);
	double delayFitness = 1 / (1 + exp(8 * normalizedDelay));
	return powerFitness * delayFitness;
}

int main(){

	circuit.readNetList();

	calculatePowerDelayMargine();

	maximumAcceptableDelay = 5;

	GeneticAlgorithm dualVDDSolver(1000, circuit.getNumberOfGates(), fitnessFunction);

	vector<bool> gateType = dualVDDSolver.Run(1000);

	CircuitStats stats = circuit.getStats(&gateType);

	cout << "delay: " << stats.MaxDelay << endl;
	cout << "power: " << stats.Power << endl;

	cout << "gate type assignments:" << endl;
	for (int i = 0; i < gateType.size(); i++){
		cout << i << ": " << (gateType[i] ? "H" : "L") << "\t";
		if (i % 8 == 7){
			cout << endl;
		}
	}

	return 0;
}