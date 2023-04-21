# Digital-circuit-simulator

The goal of the project is the simulation of a digital circuit. The project is divided into two directories: **include** and **source**. The include directory contains all the header files where the interface class is located, while the source directory contains the implementation files

## Elements

The elements that make up the circuit are:

- Logic combination circuits (**Gates**)
- **Generators**
- **Probes**

Gates can be: **And**, **Or** and **Not**.<br />
Generators can be: Clock signal generator (**Simple Generator**), Manually generator (**User Generator**).

## How the Simulator works

![Circuit](https://i.gifer.com/fetch/w300-preview/46/462c6f5f67c13830cd9fcdbfc7b55ded.gif)

The Generators generate a signal that can be 0 or 1. That signal is propagated through the circuit via Gates and at the output the Probe records the moment of the output change and the value of output.

In the Main.cpp, an object of the simulator class is created and its two main functions are called:

- `loadCircuit(filepath)` -> loads and connects the entire circuit
- `simulate(filepath)` -> calculates and prints the time when the change on the probes was registered.
