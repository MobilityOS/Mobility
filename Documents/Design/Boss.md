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
  - Devices (all devices are optional)
    - Display: required for the framebuffer console
    - Keyboard: required for local keyboard interaction
    - Network: required unless a separate UART is available as an alternative
    - Storage: required unless a remote instance is available
    - UART: required unless other input and/or output devices are available
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

## Philosophies (boss-like)

> A session may be presented as a chat.

- Everything is a Session
- Sessions as Applications
- Sessions as Tasks
- Tool Calling as System Services
- Software as a Session (SaaS)
- Session-defined Hardware (SDH)
