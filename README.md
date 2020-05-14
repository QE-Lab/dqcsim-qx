# DQCsim QX backend

[![PyPi](https://badgen.net/pypi/v/dqcsim-qx)](https://pypi.org/project/dqcsim-qx/)

See also: [DQCsim](https://github.com/mbrobbel/dqcsim) and
[QX](https://github.com/QE-Lab/qx-simulator/).

This repository contains some glue code to use the QX simulator as a DQCsim
backend.

## Status

The backend should support:

 - The following gates: H, cnot, Toffoli, I, X, Y, Z, S, S', T, T', CZ, swap,
   RX(angle), RY(angle), RZ(angle), measure X/Y/Z, prep X/Y/Z.
 - Depolarizing channel error model.
 - Dynamic qubit allocation upstream, as long as the number of qubits ever live
   at a time does not increase after the first measurement (prefixing the
   algorithm with an allocation of the desired number of qubits followed by a
   free of those qubits is enough to tell the backend you want to allocate that
   many).

## Install

You can install using `pip` using `pip install dqcsim-qx` or equivalent.
If you're installing with `--user`, make sure that the path Python installs
the executables into is in your system path, otherwise DQCsim will not be
able to find the plugin. A simple way to see where the files are installed
is to run `pip uninstall dqcsim-qx`; it shows which files it's about to
delete before querying for confirmation.

## Building/installing from source

 - Make sure all git submodules are checked out:
   `git submodule update --init --recursive`

 - Build the wheel file locally:
   `python3 setup.py build bdist_wheel`

 - Install the wheel file you just built:
   `pip install target/python/dist/*` (or equivalent)

Don't push a build like this to PyPI or attempt to distribute this wheel; it
will likely only work locally due to hardcoded paths and the likes. Refer to
`release.md` for more info.

## Usage

Once the plugin is installed, you can use it by selecting the `qx` backend in
a DQCsim command.

The following ArbCmds are supported for initialization and at runtime from
host or upstream unless otherwise specified:

 - `qx.error`: selects the error model using the JSON argument, which can be:

    - `{"model": "none"}` to disable error;
    - `{"model": "depolarizing_channel", "error_probability": <float>}` to
      select the depolarizing channel error model. Note that like QX itself,
      this model does not take gate timing into consideration; it simply
      inserts an X, Y, or Z gate with the given probability before any gate
      in the circuit.

   If the error model is modified during execution, the model is applied
   retroactively back to the previous measurement, because the backend queues
   up gates in a circuit until a measurement gate is seen.

 - `qx.dump_state`: dumps the state of the quantum register, flushing the
   aforementioned gate queue to synchronize the state of the quantum register.

 - `qx.epsilon` (only for `init()`): specifies the gate detection RMS
   tolerance using a JSON argument of the form `{"epsilon": <float>}`. Epsilon
   defaults to 1e-6.
