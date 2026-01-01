# MappedImagesDetector

A native C++20 Windows tool to detect suspicious or unexpected mapped images in process address spaces and report related anomalies. The project is implemented in modern C++ (C++20) and targets Visual Studio (MSVC) using Win32 APIs.

## Features

- Scan running processes for mapped modules and images
- Detect anomalies in memory mappings
- Lightweight, native implementation with minimal dependencies
- Test module for local validation

## Repository layout

- `AnnomaliesDetector/` — core detection library and executable sources
- `TestModule/` — small test harness / DLL used during development
- `MappedImagesDetector.sln` — Visual Studio solution (open in VS 2019 / 2022)

## Requirements

- Windows 10 or later
- Visual Studio 2019 or 2022 with C++ workload
- C++20 toolset (MSVC)

## Build

1. Open the solution file in Visual Studio: `MappedImagesDetector.sln`.
2. Select the desired configuration (`x64` recommended) and `Debug` or `Release`.
3. Build the solution (Build -> Build Solution).

Alternatively, use MSBuild from the Developer Command Prompt:

```powershell
msbuild MappedImagesDetector.sln /p:Configuration=Release /p:Platform=x64
```

Built binaries are placed in the project `bin` folders for the selected configuration (for example: `AnnomaliesDetector\x64\Release`).

## Usage

- The primary executable and test artifacts are produced in the build output directories.
- Use the test harness in `TestModule` to validate detection logic during development.
- Logs are produced by the built-in logger (`CLogger`) and can be configured in source or at runtime depending on the build.

## Development notes

- The codebase follows a modular structure: `CMemoryMapper`, `CScanTask`, and `CLogger` contain the main scanning and logging logic.
- The project targets C++20; use `/std:c++20` in project settings when editing or creating new projects.
- Keep platform-specific code isolated where possible and prefer RAII for resource management.

## Contributing

1. Fork the repository and create a feature branch.
2. Open a pull request with a clear description of changes and rationale.
3. Ensure builds succeed on Windows x64 and add tests where applicable.

## License

See `LICENSE` for details. If a license is not present, add one (MIT is a common choice for tooling projects).

## Contact

For issues or feature requests, open an issue on GitHub in this repository.
