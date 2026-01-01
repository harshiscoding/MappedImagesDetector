# Contributing to MappedImagesDetector

Thank you for your interest in contributing. This project exists to improve **defensive Windows security**, specifically the detection of **malicious in-memory code and DLL injection techniques**. Contributions are welcome — but the bar is intentionally high.

If you are looking to add loader code, offensive tooling, or anything that meaningfully lowers the cost of writing malware, this is **not** the right repository.

---

## Project scope (read first)

**MappedImagesDetector is defensive-only.**

The scope includes:

* Memory-based malware detection
* Heuristics for manual-mapped DLLs
* PE structure validation in memory
* False-positive reduction
* Performance and stability improvements
* Code quality, safety, and maintainability

The scope explicitly excludes:

* Loaders, injectors, or reflective DLL implementations
* Code that demonstrates how to bypass detection
* Weaponized proof-of-concepts
* Obfuscation or evasion techniques framed as “research”

Pull requests that cross this line will be closed without discussion.

---

## What makes a good contribution

Strong contributions usually fall into one of these categories:

### Detection improvements

* New **defensive heuristics** for identifying suspicious mapped images
* Better correlation between memory regions and module metadata
* Improvements to PE validation logic
* Reductions in false positives without sacrificing coverage

### Engineering quality

* Performance optimizations (especially for large processes)
* Safer memory handling and error paths
* Cleaner abstractions without unnecessary complexity
* Improved logging or diagnostics

### Testing & reliability

* New test cases using **safe, non-malicious** samples
* Better test coverage for edge cases
* Reproducible validation scenarios

### Documentation

* Clear explanations of detection logic
* Architecture diagrams or threat models
* Comments that explain *why* something is done, not *what* it does

---

## Coding standards

* **Language:** C++20 (MSVC)
* **Platform:** Windows x64
* Prefer explicit types and clear ownership
* Use RAII for resource management
* Avoid hidden control flow and clever tricks
* Defensive checks > undefined behavior

This is security software. Correctness and clarity matter more than cleverness.

---

## Logging & diagnostics

All suspicious behavior, errors, and edge cases should be logged via the existing logging facilities.

When adding new detection logic:

* Log *why* something is flagged
* Include enough context to debug false positives
* Avoid noisy logs that trigger on benign behavior

---

## Tests

* Do **not** submit live malware samples
* Do **not** submit exploit payloads
* Use controlled, synthetic, or red-team-safe test modules only

The included `TestModule` exists specifically for validation without introducing real threats into the repository.

---

## Submitting a pull request

1. Fork the repository
2. Create a feature branch (`feature/<short-description>`)
3. Ensure the project builds cleanly on **Windows x64 (Release)**
4. Add or update tests if detection logic changes
5. Open a pull request with:

   * Clear problem statement
   * Explanation of the approach
   * Trade-offs and limitations

Low-effort or unclear PRs will be closed.

---

## Code review expectations

Reviews focus on:

* Defensive value
* Risk of misuse
* False-positive impact
* Performance cost
* Long-term maintainability

Expect feedback. This is intentional.

---

## Reporting security issues

If you discover a vulnerability **in this project itself**, do not open a public issue.

Instead, contact the maintainers privately using the channels listed in the repository so it can be addressed responsibly.

---

## Final note

MappedImagesDetector is built for people who care about **real-world detection quality**, not flashy demos.

If your contribution makes it harder for malware to hide in memory — it’s welcome.
If it makes malware easier to write — it will be rejected.

Be precise. Be defensive. Be professional.
