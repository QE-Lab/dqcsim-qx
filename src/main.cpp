#include <dqcsim>
#include <memory>
#include <vector>
#include <xpu.h>
#include <xpu/runtime>
#include <qx_representation.h>
#include "bimap.hpp"

// Alias the dqcsim::wrap namespace to something shorter.
namespace dqcs = dqcsim::wrap;

/**
 * Function pointer type for constructing/binding QX gates.
 */
using GateConstructor = void (*)(std::vector<qx::gate*>&, std::vector<size_t>&&, dqcs::ArbData&&);

/**
 * Hadamard gate constructor.
 */
static void construct_h(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  gates.push_back(new qx::hadamard(qubits[0]));
}

/**
 * CNOT gate constructor.
 */
static void construct_cnot(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  gates.push_back(new qx::cnot(qubits[0], qubits[1]));
}

/**
 * Toffoli gate constructor.
 */
static void construct_toffoli(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  gates.push_back(new qx::toffoli(qubits[0], qubits[1], qubits[2]));
}

/**
 * Identity gate constructor.
 */
static void construct_identity(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  gates.push_back(new qx::identity(qubits[0]));
}

/**
 * X gate constructor.
 */
static void construct_pauli_x(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  gates.push_back(new qx::pauli_x(qubits[0]));
}

/**
 * Y gate constructor.
 */
static void construct_pauli_y(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  gates.push_back(new qx::pauli_y(qubits[0]));
}

/**
 * Z gate constructor.
 */
static void construct_pauli_z(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  gates.push_back(new qx::pauli_z(qubits[0]));
}

/**
 * S gate constructor.
 */
static void construct_s(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  gates.push_back(new qx::phase_shift(qubits[0]));
}

/**
 * S dagger gate constructor.
 */
static void construct_s_dag(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  gates.push_back(new qx::s_dag_gate(qubits[0]));
}

/**
 * T gate constructor.
 */
static void construct_t(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  gates.push_back(new qx::t_gate(qubits[0]));
}

/**
 * T dagger gate constructor.
 */
static void construct_t_dag(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  gates.push_back(new qx::t_dag_gate(qubits[0]));
}

/**
 * Controlled phase gate constructor.
 */
static void construct_ctrl_phase_shift(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  double angle = data.pop_arb_arg_as<double>();
  gates.push_back(new qx::ctrl_phase_shift(qubits[0], qubits[1], angle));
}

/**
 * Swap gate constructor.
 */
static void construct_swap(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  gates.push_back(new qx::swap(qubits[0], qubits[1]));
}

/**
 * RX gate constructor.
 */
static void construct_rx(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  gates.push_back(new qx::rx(qubits[0], data.get_arb_arg_as<double>(0)));
}

/**
 * RY gate constructor.
 */
static void construct_ry(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  gates.push_back(new qx::ry(qubits[0], data.get_arb_arg_as<double>(0)));
}

/**
 * RZ gate constructor.
 */
static void construct_rz(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  gates.push_back(new qx::rz(qubits[0], data.get_arb_arg_as<double>(0)));
}

/**
 * Measure Z constructor.
 */
static void construct_measure_z(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  for (size_t qubit : qubits) {
    gates.push_back(new qx::measure(qubit));
  }
}

/**
 * Measure X constructor.
 */
static void construct_measure_x(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  for (size_t qubit : qubits) {
    gates.push_back(new qx::measure_x(qubit));
  }
}

/**
 * Measure Y constructor.
 */
static void construct_measure_y(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  for (size_t qubit : qubits) {
    gates.push_back(new qx::measure_y(qubit));
  }
}

/**
 * Prep Z constructor.
 */
static void construct_prep_z(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  for (size_t qubit : qubits) {
    gates.push_back(new qx::prepz(qubit));
  }
}

/**
 * Prep X constructor.
 */
static void construct_prep_x(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  for (size_t qubit : qubits) {
    gates.push_back(new qx::prepx(qubit));
  }
}

/**
 * Prep Y constructor.
 */
static void construct_prep_y(std::vector<qx::gate*> &gates, std::vector<size_t> &&qubits, dqcs::ArbData &&data) {
  for (size_t qubit : qubits) {
    gates.push_back(new qx::prepy(qubit));
  }
}

/**
 * Main plugin class for QX.
 */
class QxPlugin {
public:

  // List of gates that have been received but have not been simulated yet.
  // The integer before the gate specifies the number of simulation cycles
  // advanced between the previous gate and this gate.
  std::vector<qx::gate*> pending;

  // Number of qubits allocated/to allocate within QX. This can't be changed
  // anymore once QX is initialized.
  size_t num_qubits = 0;

  // Counter for the circuit name generator.
  size_t circuit_counter = 0;

  // The current quantum register. Initialized only when the first measurement
  // is received.
  std::shared_ptr<qx::qu_register> qreg;

  // Map from upstream qubits to QX qubits.
  QubitBiMap dqcs2qx;

  // Map from DQCsim gates to QX gates.
  dqcs::GateMap<GateConstructor> gatemap;

  // Values for the depolarizing channel error model.
  bool depolarizing_channel = false;
  double error_probability = 0.0;

  /**
   * Initialization callback.
   */
  void initialize(
    dqcs::PluginState &state,
    dqcs::ArbCmdQueue &&cmds
  ) {
    // TODO: parameter parsing
    // epsilon, depolarizing_channel
    double epsilon = 1.0e-6;

    // Construct the gatemap.
    gatemap.with_unitary(construct_h, dqcs::PredefinedGate::H, 0, epsilon);
    gatemap.with_unitary(construct_cnot, dqcs::PredefinedGate::X, 1, epsilon);
    gatemap.with_unitary(construct_toffoli, dqcs::PredefinedGate::X, 2, epsilon);
    gatemap.with_unitary(construct_identity, dqcs::PredefinedGate::I, 0, epsilon);
    gatemap.with_unitary(construct_pauli_x, dqcs::PredefinedGate::X, 0, epsilon);
    gatemap.with_unitary(construct_pauli_y, dqcs::PredefinedGate::Y, 0, epsilon);
    gatemap.with_unitary(construct_pauli_z, dqcs::PredefinedGate::Z, 0, epsilon);
    gatemap.with_unitary(construct_s, dqcs::PredefinedGate::S, 0, epsilon);
    gatemap.with_unitary(construct_s_dag, dqcs::PredefinedGate::S_DAG, 0, epsilon);
    gatemap.with_unitary(construct_t, dqcs::PredefinedGate::T, 0, epsilon);
    gatemap.with_unitary(construct_t_dag, dqcs::PredefinedGate::T_DAG, 0, epsilon);
    gatemap.with_unitary(construct_ctrl_phase_shift, dqcs::PredefinedGate::Phase, 1, epsilon);
    gatemap.with_unitary(construct_swap, dqcs::PredefinedGate::Swap, 0, epsilon);
    gatemap.with_unitary(construct_rx, dqcs::PredefinedGate::RX, 0, epsilon);
    gatemap.with_unitary(construct_ry, dqcs::PredefinedGate::RY, 0, epsilon);
    gatemap.with_unitary(construct_rz, dqcs::PredefinedGate::RZ, 0, epsilon);
    gatemap.with_measure(construct_measure_z, dqcs::PauliBasis::Z, epsilon);
    gatemap.with_measure(construct_measure_x, dqcs::PauliBasis::X, epsilon);
    gatemap.with_measure(construct_measure_y, dqcs::PauliBasis::Y, epsilon);
    gatemap.with_prep(construct_prep_z, dqcs::PauliBasis::Z, epsilon);
    gatemap.with_prep(construct_prep_x, dqcs::PauliBasis::X, epsilon);
    gatemap.with_prep(construct_prep_y, dqcs::PauliBasis::Y, epsilon);

  }

  /**
   * Qubit allocation callback.
   *
   * DQCsim supports allocating and freeing qubits at will, but QX doesn't.
   * The trivial solution would be to just error out on the N+1'th qubit
   * allocation, but we can do better than that when there are deallocations
   * as well by reusing qubits that were freed. Furthermore, we only initialize
   * QX when the first measurement is performed, with as many qubits as have
   * been allocated at that point. The dqcs2qx bimap is used to keep track of
   * which upstream qubit maps to which QX qubit.
   */
  void allocate(
    dqcs::PluginState &state,
    dqcs::QubitSet &&qubits,
    dqcs::ArbCmdQueue &&cmds
  ) {

    // Loop over the qubits that are to be allocated.
    while (qubits.size()) {

      // A new DQCsim upstream qubit index to allocate.
      size_t dqcsim_qubit = qubits.pop().get_index();

      // Look for the first free QX qubit index.
      ssize_t qx_qubit = -1;
      for (size_t q = 0; q < num_qubits; q++) {
        if (dqcs2qx.reverse_lookup(q) < 0) {
          qx_qubit = q;
          break;
        }
      }

      // If no qubits are free, see if we can still allocate more.
      if (!qreg) {
        qx_qubit = num_qubits;
        num_qubits++;
      }

      if (qx_qubit >= 0) {
          DQCSIM_DEBUG("Placed upstream qubit %d at QX index %d", dqcsim_qubit, qx_qubit);
          dqcs2qx.map(dqcsim_qubit, qx_qubit);
      } else {
        throw std::runtime_error("Upstream plugin requires too many live qubits!");
      }

    }

  }

  /**
   * Qubit deallocation callback.
   *
   * Inverse of `allocate()`.
   */
  void free(
    dqcs::PluginState &state,
    dqcs::QubitSet &&qubits
  ) {

    // Loop over the qubits that are to be freed.
    while (qubits.size()) {

      // The DQCsim upstream qubit index to free.
      size_t dqcsim_qubit = qubits.pop().get_index();

      // Unmap it in the bimap to do the free.
      DQCSIM_DEBUG("Freed upstream qubit %d", dqcsim_qubit);
      dqcs2qx.unmap_upstream(dqcsim_qubit);

    }

  }

  /**
   * Simulates all pending gates.
   *
   * When this is called for the first time, it also initializes the quantum
   * register.
   */
  void simulate_pending() {

    // Initialize the qubit state if we haven't already.
    if (!qreg) {
      DQCSIM_INFO("Creating quantum register of %d qubits...", num_qubits);
      try {
        qreg = std::make_shared<qx::qu_register>(num_qubits);
      } catch(std::bad_alloc& exception) {
        DQCSIM_FATAL("Not enough memory for quantum state");
        throw std::runtime_error("out of memory");
      }
    }

    // Construct a circuit from the pending gates.
    qx::circuit perfect_circuit = qx::circuit(
      num_qubits, "circuit_" + std::to_string(circuit_counter++));
    for (qx::gate *gate : pending) {
      perfect_circuit.add(gate);
    }
    pending.clear();

    // Add error using the depolarizing channel model if required.
    qx::circuit *circuit = &perfect_circuit;
    if (depolarizing_channel) {
      size_t total_errors = 0;
      circuit = qx::noisy_dep_ch(&perfect_circuit, error_probability, total_errors);
      DQCSIM_INFO("Depolarizing channel model inserted %d errors", total_errors);
    }

    // Simulate the circuit.
    circuit->execute(*qreg);

  }

  /**
   * Gate callback.
   */
  dqcs::MeasurementSet gate(
    dqcs::PluginState &state,
    dqcs::Gate &&gate
  ) {

    // Convert the DQCsim gate to a QX gate and queue it up.
    const GateConstructor *gate_constructor = nullptr;
    dqcs::QubitSet dqcsim_qubits = dqcs::QubitSet(0);
    dqcs::ArbData data = dqcs::ArbData(0);
    if (!gatemap.detect(gate, &gate_constructor, &dqcsim_qubits, &data)) {
      DQCSIM_DEBUG("Unsupported gate! Here's a dump: %s", gate.dump().c_str());
      throw std::runtime_error("unsupported gate");
    }
    std::vector<size_t> qx_qubits;
    while (dqcsim_qubits.size()) {
      size_t dqcsim_index = dqcsim_qubits.pop().get_index();
      ssize_t qx_index = dqcs2qx.forward_lookup(dqcsim_index);
      if (qx_index < 0) {
        throw std::runtime_error("failed to resolve DQCsim qubit to QX qubit");
      }
      qx_qubits.push_back(qx_index);
    }
    (*gate_constructor)(pending, std::move(qx_qubits), std::move(data));

    // Simulate once we receive a measurement, because then we have to return a
    // result.
    if (gate.has_measures()) {
      simulate_pending();
    }

    // Return the state of the measurement register for any measured qubits.
    dqcs::QubitSet measured_qubits = gate.get_measures();
    dqcs::MeasurementSet measurements = dqcs::MeasurementSet();
    while (measured_qubits.size()) {
      dqcs::QubitRef dqcsim_ref = measured_qubits.pop();
      size_t dqcsim_index = dqcsim_ref.get_index();
      ssize_t qx_index = dqcs2qx.forward_lookup(dqcsim_index);
      if (qx_index < 0) {
        throw std::runtime_error("failed to resolve DQCsim qubit to QX qubit");
      }
      dqcs::MeasurementValue value = qreg->get_measurement(qx_index)
        ? dqcs::MeasurementValue::One : dqcs::MeasurementValue::Zero;
      measurements.set(dqcs::Measurement(dqcsim_ref, value));
    }
    return measurements;
  }

  /**
   * Drop callback.
   *
   * We use this to flush out any pending operations occurring after the last
   * measurement.
   */
  void drop(
    dqcs::PluginState &state
  ) {
    simulate_pending();
  }

  // TODO: arb to dump current state

};

int main(int argc, char *argv[]) {
  QxPlugin qxPlugin;
  return dqcs::Plugin::Backend("qx", "JvS", "0.0.3")
    .with_initialize(&qxPlugin, &QxPlugin::initialize)
    .with_allocate(&qxPlugin, &QxPlugin::allocate)
    .with_free(&qxPlugin, &QxPlugin::free)
    .with_gate(&qxPlugin, &QxPlugin::gate)
    .with_drop(&qxPlugin, &QxPlugin::drop)
    .run(argc, argv);
}
