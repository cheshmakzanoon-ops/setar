# Real-time C++ engineering

The audio engine must be bounded in memory and work, maintain physical/numerical
validity, and remain independent of editor or worker progress. These chapters
define proposed contracts and required evidence; they do not assert that the
entire future engine is already real-time qualified.

| Chapter | Decision it supports |
| --- | --- |
| [Callback contract](01-callback-contract.md) | What may execute during processing and how faults are contained |
| [Events and overflow](02-events-capacity-and-overflow.md) | How timing, note identity, capacity, and release integrity are preserved |
| [Performance and numeric hygiene](03-performance-and-numerical-hygiene.md) | How cost, precision, denormals, and overload are measured |
| [Concurrency and telemetry](04-concurrency-state-and-telemetry.md) | How prepared data and diagnostics cross threads without unsafe lifetime |

Read the [validation evidence contract](../validation/00-evidence-contract.md)
first. Bring a verified subsystem into the callback harness before optimizing
it. Keep a separate offline oracle and measure every real-time approximation.
The [plugin lifecycle](../plugin/03-host-lifecycle-buses-latency.md) determines
how these contracts meet actual host behavior.

Required handoff artifacts include capacities, call graph, ownership, overflow
semantics, independent tests, allocation/deallocation observations, race/memory
checks, and timing context. Average CPU or a silence-only smoke test is not
enough. Use [plugin implementation and review prompts](../../prompts/plugin/README.md)
for bounded tasks that cross this boundary.
