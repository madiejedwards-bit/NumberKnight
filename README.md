# NumberKnight
A math-based adventure game built natively in C++ utilizing **Dear ImGui** for the user interface and **DirectX 11** for hardware-accelerated rendering.
## Quick Start 
Want to try the game? Skip the compile step and grab the compiled desktop binary directly from my [Latest Stable Release](https://github.com/madiejedwards-bit/NumberKnight/releases/tag/v1.0.0)
## System Architecture & Separation of Concerns
To maintain a clean decoupled codebase, the project separates core gameplay simulation from the presentation layer:
* **The `Questions` Engine (100% Hand-Coded):** Built entirely from scratch without AI assistance. This isolated domain class handles the core math game loop, dynamic question generation, score/health state progression, and precision game-timer tracking.
* **The Grid UI Class:** Manages the visual rendering grid system, utilizing modern AI-assisted scaffolding to map spatial movements onto the underlying state.

## Development Workflow
* **Rapid Prototyping:** Utilized **Cursor AI** during the initial phase of development to scaffold the project structure and quickly generate the base ImGui wrapper layouts.
* **Engineering & Refactoring:** Took ownership of the codebase post-prototype to deeply refactor the main loop execution, optimize grid systems, resolve UI layout bugs, and manually tune player avatar scaling to precision.

## Technical Highlights & Performance Tuning
During the refactoring phase, several critical engineering optimizations were introduced to ensure memory safety, stability, and clean static analysis scores:

* **Memory Safety & Stack Optimization (C6262):** Refactored large texture layout definitions and internal memory blocks within image loading workflows (`stb_image`) to avoid dangerous stack-allocated boundaries, mitigating runtime stack overflow vulnerabilities.
* **DirectX 11 Parameter Integrity (C6387):** Hardened the resource pipeline by introducing strict pointer tracking and `HRESULT` return-status checks (`SUCCEEDED`) before invoking `CreateShaderResourceView`. This eliminates null-pointer exceptions if a texture fails to initialize dynamically.
* **Dynamic Grid UI Sizing:** Custom-tuned ImGui frame padding layouts (`ImGuiStyleVar_FramePadding`) and absolute bounds checking to dynamically scale player avatars cleanly within game loop grids without introducing stretching or rendering artifacts.

## Tech Stack
* **Language:** C++17 / C++20
* **Graphics API:** DirectX 11 (D3D11)
* **UI Library:** Dear ImGui
* **Utilities:** stb_image for efficient cross-platform texture decoding
* **AI Tooling:** Cursor AI (Scaffolding & Layout Prototyping)

