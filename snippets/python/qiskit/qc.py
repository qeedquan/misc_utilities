#!/usr/bin/env python3

# Simulate a quantum circuit and print the probabilities of the output at the end

import numpy as np
from qiskit import QuantumCircuit
from qiskit.quantum_info import Statevector

# 1. A quantum circuit for preparing the quantum state |000> + i |111>
qc = QuantumCircuit(3)
qc.h(1)
qc.h(2)
qc.p(0.3, 2)
qc.cx(1, 2)
qc.h(1)
qc.h(2)
qc.cx(0, 2)
print(qc)

state = Statevector.from_int(0, 2**3)
print(state.to_dict())
state = state.evolve(qc)
print(state.to_dict())
