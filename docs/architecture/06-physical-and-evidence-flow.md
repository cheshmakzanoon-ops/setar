# Physical state, observation and evidence flow

These diagrams describe **proposed** component relationships, not currently
compiled and tested modules. They are kept small so a contributor can see
which state/energy boundaries require a verification fixture.

## Physical signal/energy model

```mermaid
flowchart TD
    A["Performer gesture trajectory"] --> B["Nail/finger/fret contact"]
    B <-->|"Forces and velocities"| C["Four persistent strings"]
    C <-->|"Bridge power ports"| D["Shared wooden structure"]
    D <-->|"Surface pressure/velocity"| E["Cavity and surrounding air"]
    E --> F["Calibrated microphone observation"]
    F --> G["Optional downstream gain/room and PCM"]
```

The contact element may contain a driven external work source (the performer's
motion) as well as passive losses. The body/cavity feedback may matter for
physical energy, depending on the adopted model. The downstream microphone
observation and playback gain do not become a new mechanical force just
because the same plugin renders them. The string-to-body path is bidirectional;
an outgoing rendered audio sample is not a substitute for a returned bridge
velocity. Proposed owner and type contracts are in
[C++ module contracts](04-cpp20-module-contracts.md) and
[coupling scheme selection](05-discrete-solver-selection.md).

## Evidence to runtime and product claims

```mermaid
flowchart TD
    A["Specimen measurements and source audit"] --> B["Canonical physical model"]
    B --> C["Independent offline reference"]
    C --> D["Verified reduced model and prepared asset"]
    D --> E["Realtime C++ instrument"]
    E --> F["JUCE/VST3 adapter and musician UI"]
    C --> G["Independent numerical and measured evaluation"]
    D --> G
    F --> H["Host and practitioner qualification"]
    G --> I["Bounded scientific claims"]
    H --> I
```

Only an accepted source can enter a calibrated asset; a purely synthetic input
can still enter an **explicitly synthetic** reference fixture. Numeric
verification and measured validation are separate. A review may reject the
proposed reduced model without invalidating the whole reference laboratory.
Similarly, a successful JUCE build without real host tests cannot satisfy the
product-qualification branch. No diagram represents a completed phase gate.

## Data ownership under concurrency

```mermaid
flowchart TD
    A["Worker: validate and prepare"] --> B["Immutable accepted asset"]
    B --> C["Single audio-thread state writer"]
    C --> D["Bounded telemetry snapshots"]
    D --> E["UI and offline reporter"]
    F["Host and performer events"] --> G["Bounded event adapter"]
    G --> C
```

Old asset destruction, expensive parsing, graphics and disk work happen outside
the realtime callback. A copied snapshot is safe only after its lifetime and
publication protocol are verified. An atomic pointer by itself does not solve
reclamation. Refer to [engine realtime requirements](../realtime/README.md),
[handoff work order ENG-P13-T02](../../roadmap/engineering-tasks.json), and
[the performance budget method](../engineering/performance-budget.md).
