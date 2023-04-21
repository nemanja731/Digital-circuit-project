# Digital-circuit-simulator

The goal of the project is the simulation of a digital circuit.

## Elements

The elements that make up the circuit are:

- Logic combination circuits (Gates)
- Generators
- Probes

Logic combination circuits can be: **And**, **Or** and **Not**
Generators can be: Clock signal generator (**Simple Generator**), Manually generator (**User Generator**)

## How the Simulator works

The Generators generate a signal that can be 0 or 1, that signal is propagated through the circuit via Gates and at the output the Probe records the moment of the output change and the value of output.

In the Main.cpp, an object of the simulator class is created and its two main functions are called:

- `loadCircuit` -> loads and connects the entire circuit
- `simulate` -> calculates and prints the time when the change on the probes was registered.
