#include <dqcsim>
#include <xpu.h>
#include <xpu/runtime>
#include <qx_representation.h>
#include "bimap.hpp"

// Alias the dqcsim::wrap namespace to something shorter.
namespace dqcs = dqcsim::wrap;

class QxPlugin {
public:

  // Number of qubits allocated/to allocate within QX. This can't be changed
  // anymore once QX is initialized.
  size_t num_qubits = 0;

  // Whether QX has been initialized yet or not. This happens when the first
  // measurement is received.
  bool qx_initialized = false;

  // Map from upstream qubits to QX qubits.
  QubitBiMap dqcs2qx;

  /**
   * Initialization callback.
   */
  void initialize(
    dqcs::PluginState &state,
    dqcs::ArbCmdQueue &&cmds
  ) {
    // TODO?
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
        if (dqcs2qx.reverse_lookup(qx_qubit) < 0) {
          qx_qubit = q;
          break;
        }
      }

      // If no qubits are free, see if we can still allocate more.
      if (!qx_initialized) {
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
   * Gate callback.
   */
  dqcs::MeasurementSet gate(
    dqcs::PluginState &state,
    dqcs::Gate &&gate
  ) {
    // TODO
    return dqcs::MeasurementSet();
  }

};

int main(int argc, char *argv[]) {
  QxPlugin qxPlugin;
  return dqcs::Plugin::Backend("qx", "JvS", "v0.0")
    .with_initialize(&qxPlugin, &QxPlugin::initialize)
    .with_allocate(&qxPlugin, &QxPlugin::allocate)
    .with_free(&qxPlugin, &QxPlugin::free)
    .with_gate(&qxPlugin, &QxPlugin::gate)
    .run(argc, argv);
}
