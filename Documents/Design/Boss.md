# Mobility Basic Operating System for Sessions (BOSS) Design Documents

**Work In Progress**

## Components

- Session Manager
- Session Storage
- Model Provider
- Hardware Abstraction Layer (HAL)
  - Compute
    - Processor
    - Interrupts and Events
    - Timers
    - Memory
  - Devices
    - Display (optional; required for the framebuffer console)
    - Keyboard (optional; required for local keyboard interaction)
    - Network (optional; may be provided through a separate UART)
    - Storage
    - UART
  - Infrastructure
    - Scheduler
    - Task Pool

## Target Resource Requirements

- Minimum MCU Configuration
  - Memory: 256 KiB
  - Storage: 1 MiB
- Recommended MCU Configuration
  - Memory: 512 KiB
  - Storage: 16 MiB
- Minimum Prototype Configuration
  - Memory: 48 MiB
  - Storage: 256 MiB
