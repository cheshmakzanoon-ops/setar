#!/usr/bin/env python3
"""Manual mutation check of the loss-law tests (not a CTest: it recompiles per mutant).

Each mutant injects one plausible implementation defect into a scratch COPY of the
sources; the repository is never modified. A mutant is killed when at least one of the
loss-related tests fails. A surviving mutant means the tests cannot detect that defect
and must be strengthened before the change is trusted. Takes about a minute.

    python3 tools/mutation_check_loss.py [--compiler g++]
"""
from __future__ import annotations

import argparse
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]

NUMERICAL_TESTS = ["loss_law_passivity", "loss_decay_estimator", "loss_decay_oracle",
                   "loss_modal_reference_crosscheck", "invalid_parameters",
                   "discrete_energy_ledger", "stability_envelope"]
CORE_TESTS = ["wavenumber_loss"]

NP = "src/numerical_prototype.cpp"
# (label, file, exact text to replace once, replacement)
MUTANTS = [
    ("loss sign flipped in the right-hand side (energy gain)", NP,
     "- alpha * kq + 0.5 * dt * dq +", "- alpha * kq - 0.5 * dt * dq +"),
    ("sigma_1 term dropped from the damping matrix", NP,
     "g.loss.decay_wavenumber_squared_m2_per_s * b[i * n + j] +\n"
     "                       g.loss.decay_frequency_squared_s * value",
     "0.0 * b[i * n + j] +\n                       g.loss.decay_frequency_squared_s * value"),
    ("sigma_2 term uses K instead of K/mu (unit error)", NP,
     "g.loss.decay_frequency_squared_s * value / g.linear_density_kg_per_m);",
     "g.loss.decay_frequency_squared_s * value);"),
    ("dissipation ledger off by a factor of two", NP,
     "const double dissipated = dt * mu * h * dissipation_form;",
     "const double dissipated = 0.5 * dt * mu * h * dissipation_form;"),
    ("system matrix omits dt*D/2", NP,
     "alpha * value +\n                                             0.5 * dt * damping;",
     "alpha * value;"),
    ("amplification oracle: missing 1/2 in the log radius", NP,
     "const double log_radius = 0.5 * std::log1p(-4.0 * a / den);",
     "const double log_radius = std::log1p(-4.0 * a / den);"),
    ("amplification oracle: slow overdamped root uses the fast one", NP,
     "const double slow = omega2 / (gamma + root);", "const double slow = gamma + root;"),
    ("estimator reports energy (2x) decay instead of amplitude", NP,
     "estimate.decay_rate_per_s = -0.5 * std::log(-b) * sample_rate_hz;",
     "estimate.decay_rate_per_s = -1.0 * std::log(-b) * sample_rate_hz;"),
    ("one-step lag: dissipation uses the old velocity only", NP,
     "dv += damping_[i * n + j] * 0.5 * (v_[j] + next_v_[j]);",
     "dv += damping_[i * n + j] * v_[j];"),
    ("explicit solver silently ignores a declared loss", NP,
     "    require(!has_loss(g.loss),\n            \"explicit prototype is lossless: use "
     "MidpointStiffString for a declared loss law\");\n", ""),
    ("negative constant loss admitted", NP,
     "g.loss.decay_constant_per_s >= 0.0,", "true,"),
    ("reference: sigma_1 multiplies omega^2 instead of k^2", "src/reference.cpp",
     "parameters.decay_wavenumber_squared_m2_per_s * k * k +",
     "parameters.decay_wavenumber_squared_m2_per_s * omega * omega +"),
    ("reference: sigma_1 term ignored", "src/reference.cpp",
     "parameters.decay_wavenumber_squared_m2_per_s * k * k +", "0.0 * k * k +"),
]


def run_mutant(compiler: str, scratch: Path, label: str, relative: str, old: str, new: str):
    if scratch.exists():
        shutil.rmtree(scratch)
    for name in ("include", "src", "tests"):
        shutil.copytree(ROOT / name, scratch / name)
    target = scratch / relative
    text = target.read_text(encoding="utf-8")
    if old not in text:
        return "NOT APPLIED", [f"pattern missing from {relative}"]
    target.write_text(text.replace(old, new, 1), encoding="utf-8")
    flags = ["-std=c++20", "-O1", "-I", str(scratch / "include"), "-I", str(scratch / "tests")]
    numerical = subprocess.run(
        [compiler, *flags, str(scratch / "tests/numerical_prototype_tests.cpp"),
         str(scratch / NP), str(scratch / "src/reference.cpp"), "-o", str(scratch / "numerical")],
        capture_output=True, text=True)
    core = subprocess.run(
        [compiler, *flags, str(scratch / "tests/reference_tests.cpp"),
         str(scratch / "src/reference.cpp"), "-o", str(scratch / "core")],
        capture_output=True, text=True)
    if numerical.returncode or core.returncode:
        return "BUILD FAILED", [(numerical.stderr or core.stderr)[:300]]
    killed = []
    for case in NUMERICAL_TESTS:
        result = subprocess.run([str(scratch / "numerical"), case], capture_output=True,
                                text=True, timeout=300)
        if result.returncode:
            killed.append(f"{case}: {result.stderr.strip()[:100]}")
    for case in CORE_TESTS:
        result = subprocess.run([str(scratch / "core"), case], capture_output=True, text=True,
                                timeout=300)
        if result.returncode:
            killed.append(f"core.{case}: {result.stderr.strip()[:100]}")
    return ("KILLED" if killed else "SURVIVED"), killed


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", default="g++")
    args = parser.parse_args()
    problems = 0
    with tempfile.TemporaryDirectory(prefix="setar-mutants-") as directory:
        for label, relative, old, new in MUTANTS:
            status, details = run_mutant(args.compiler, Path(directory) / "work", label,
                                         relative, old, new)
            print(f"{status:12s} {label}")
            for line in details[:2]:
                print(f"{'':14s}{line}")
            if status != "KILLED":
                problems += 1
    print(f"{len(MUTANTS) - problems}/{len(MUTANTS)} mutants killed")
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
