# MappedImagesDetector

**Native Windows memory scanner for detecting malicious DLL injections and in-memory malware**

MappedImagesDetector is a **low-level C++20 Windows tool** designed to detect **malicious DLL injection techniques** used by modern malware, including **manual-mapped DLLs, erased PE headers, and suspicious memory-mapped images** that traditional antivirus solutions often miss.

This project focuses on **memory-based detection**, not disk signatures. It is built for **antivirus engines, EDR systems, and malware research**, where visibility into a process address space is critical.

---

## What problem does this solve?

Modern malware increasingly operates **entirely in memory**:

* DLLs manually mapped without using `LoadLibrary`
* PE headers erased after mapping to evade scanners
* Fake or incomplete Import Address Tables (IAT)
* Executable memory regions that do not belong to any legitimate module

Disk-based AV sees nothing. **MappedImagesDetector inspects process memory directly** and flags these anomalies.

---

## Core capabilities

* Detect **manual-mapped DLLs** not present in the PEB/LDR module lists
* Identify **erased or corrupted PE headers** in memory
* Flag **suspicious mapped images** and orphan executable regions
* Detect **IAT / CRT thunk anomalies** common in manual mapping
* Scan executable memory regions with abnormal protections (RWX / EXEC-only)
* Lightweight, native **C++20 / Win32** implementation
* Designed to be embedded into **AV / EDR pipelines**

No gimmicks. No signature databases. Just memory inspection.

---

## Who this is for

* **Antivirus & EDR developers** adding memory-based detection
* **Blue team & SOC engineers** doing incident response
* **Malware researchers** analyzing in-memory implants
* **Security engineers** building defensive tooling

This is **not** an offensive framework and **not** a loader.

---

## How detection works (high level)

The scanner applies multiple defensive heuristics:

* Enumerates process virtual memory regions
* Correlates mapped images with PEB/LDR module lists
* Validates in-memory PE structures when present
* Detects inconsistencies typical of manual mapping
* Flags executable regions that do not map to valid images

Each finding can be scored and consumed by higher-level logic (EDR, SIEM, automation).

---

## Repository layout

```
AnomaliesDetector/        # core scanning logic
TestModule/              # test DLL for local validation
MappedImagesDetector.sln # Visual Studio solution
```

---

## Requirements

* Windows 10+ (x64)
* Visual Studio 2019 or 2022
* MSVC with C++20 enabled

---

## Build

Open the solution in Visual Studio and build `x64 / Release`, or use MSBuild:

```powershell
msbuild MappedImagesDetector.sln /p:Configuration=Release /p:Platform=x64
```

Binaries are produced under the project `bin` directories.

---

## Usage

* Run the scanner with sufficient privileges to enumerate processes
* Use `TestModule` only for development and validation
* Integrate the core logic into AV or EDR components as a detection module

Logging is handled through the built-in logger and can be redirected or extended.

---

## False positives

Memory detection is powerful but noisy. This project is intended to be:

* A **signal**, not a verdict
* Combined with behavior, reputation, and telemetry
* Tuned with allowlists and context-aware logic

---

## Contributing

Contributions focused on **defensive detection quality** are welcome:

1. Fork the repository
2. Create a feature branch
3. Ensure clean Windows x64 builds
4. Submit a PR with clear rationale

Pull requests that add offensive or loader-style functionality will be rejected.

---

## Summary

**MappedImagesDetector exists for one reason:**

> To expose malicious code that hides in memory and bypasses traditional antivirus detection.

If you are building security software and still trust disk-only indicators, you are already behind.
