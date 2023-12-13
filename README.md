# Genetic Dual VDD Optimizer
This project is part of **Low Power Design** course.
A genetic algorithm to minimize the static power of a given combinational circuit netlist by assigning gates the proper VDD levels (Dual VDD approach) with the maximum delay as the constraint.

# Dual VDD Genetic Algorithm

## Overview

The Dual VDD genetic algorithm is implemented in the provided C++ program, located in the `cpp.DualVDD` file. This algorithm is designed to optimize the power consumption and critical delay of a circuit by assigning different voltage levels to the gates within the circuit. The program makes use of a genetic algorithm approach to evolve the assignment of low voltage (LVDD) and high voltage (HVDD) to the gates, simulating the behavior of DNA in biological systems.

## Features

- **Critical Delay Calculation:** The program traverses the circuit from output to input gates to calculate the critical delay. Whenever a connection from LVDD to HVDD is encountered, the delay of an SDCVSL (Static-Dynamic Cascode Voltage Switch Logic) buffer is added.
- **Power Consumption Calculation:** The program statically calculates power consumption, summing up the power of all gates based on their LVDD or HVDD classification. If a gate with an LVDD output is connected to HVDD gates, the power consumption of one SDCVSL buffer is added to that gate's power consumption.
- **Genetic Algorithm:** This component determines the voltage levels (LVDD or HVDD) assigned to the gates. The different sets of voltage assignments form the population.
- **Fitness Function:** The algorithm evaluates the performance of a circuit configuration by multiplying the 'goodness' of the delay with the 'goodness' of the power consumption. The goodness of power consumption is calculated as the maximum power consumption of the circuit divided by the current power consumption. The goodness of delay uses a sigmoid-like function to provide a smooth transition from high fitness to low fitness, allowing states with delays slightly above 5 to not be immediately discarded.

## Usage

1. **Input Netlist:** To run the algorithm, the user must first input the `netlist`, which details the circuit's topology. An example netlist can be found in the `netlist.txt` file, representing the digital circuit below (Image 1)
2. 
![image](https://github.com/SamanMohseni/GeneticDualVDDOptimizer/assets/51726090/18bf518a-3a4c-4fea-ae59-1daf9c370927)
*Image 1. Sample circuit from [here](https://www.researchgate.net/publication/314197060_Critical_Gates_Identification_for_Fault-Tolerant_Design_in_Math_Circuits)*

4. **Execution:** After executing the genetic algorithm across 1000 generations with a population size of 1000 individuals, the program will output the following results:
   - Critical Delay (T): 4.8
   - Power Consumption (P): 31.9
   - Assigned Voltages: The list of assigned voltages to each gate is also printed. LVDD gates are highlighted in gray in Image 1 to illustrate algorithm output.

Note: SDCVSL buffers are not shown in the figure but are considered in the algorithm.
