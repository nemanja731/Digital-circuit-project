# Digital-circuit-simulator

The goal of the project is the simulation of a digital circuit.

## Elements

The elements that make up the circuit are:

- Logic combination circuits (Gates)
- Generators
- Probes

Gates can be: **And**, **Or** and **Not**<br />.
Generators can be: Clock signal generator (**Simple Generator**), Manually generator (**User Generator**).

## How the Simulator works

```diff
- text in red
+ text in green
! text in orange
# text in gray
@@ text in purple (and bold)@@
```

The Generators generate a signal that can be 0 or 1, that signal is propagated through the circuit via Gates and at the output the Probe records the moment of the output change and the value of output.

In the Main.cpp, an object of the simulator class is created and its two main functions are called:

- `loadCircuit(filepath)` -> loads and connects the entire circuit
- `simulate(filepath)` -> calculates and prints the time when the change on the probes was registered.
