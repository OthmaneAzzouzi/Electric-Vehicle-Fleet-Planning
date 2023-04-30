# Electric Vehicle Fleet Planning

This project aims to plan the routes for an electric vehicle fleet while taking into account the constraints of battery recharging. The program Uses advanced algorithms and graph-based techniques in C++ and allows users to generate efficient route plans based on the provided inputs.

# Prerequisites

To build and run the program, you need a C++ compiler (C++11 or higher)

# Usage

1. Prepare an input file with the following format:

```
<number of vehicles>
<number of charging stations>
<number of destinations>
<vehicle 1 initial battery level>
<vehicle 2 initial battery level>
...
<charging station 1 x-coordinate> <charging station 1 y-coordinate> <charging time (minutes)>
<charging station 2 x-coordinate> <charging station 2 y-coordinate> <charging time (minutes)>
...
<destination 1 x-coordinate> <destination 1 y-coordinate>
<destination 2 x-coordinate> <destination 2 y-coordinate>
...
```

2. Run the program and provide the path to the input file when prompted:

```
./ElectricVehicleFleetPlanning input.txt
```

3. The program will output the route plans for each vehicle as follows:

```
Vehicle 1: route -> [route nodes] | charging stations -> [charging station nodes]
Vehicle 2: route -> [route nodes] | charging stations -> [charging station nodes]
...

```

# Important Note on Code Conventions

Please be aware that in the source code for this program, variable names, function names, and comments are primarily written in French. We understand that this might pose some challenges for users who are not familiar with the French language. However, We have ensured that the user documentation, such as this README, is written in English for broader accessibility.

# Authors

The project was developed by Othmane Azzouzi and Besma Jabri.
