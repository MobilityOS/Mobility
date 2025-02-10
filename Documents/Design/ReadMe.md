# Mobility Design Documents

Work In Progress

## Why choose Hyper-V Generation 2 Virtual Machines as the reference platform?

- Hyper-V is a validated popular para-virtualization platform via history, and
  its Generation 2 Virtual Machines design should be the only one with an
  aggressive, lightweight para-virtualization design in the current stage, which
  help Kenji Mouri implement smaller bootable bare-metal instances that may
  exceed his expectations:
  - No CSM support. Only provides 64-Bit UEFI Class 3 firmware.
  - No emulated devices. Only devices based on VMBus are available.
  - No legacy x86 devices like the floppy controller, the DMA controller, the
    PCI Bus, the legacy Programmable Interrupt Controller (PIC), the legacy
    Programmable Interval Timer (PIT), and the Super I/O device. Needs guest
    operating systems to adapt explicitly.
- NanaBox, created by Kenji Mouri, and already in highly daily use by him, is a
  third-party lightweight XAML-based out-of-box-experience oriented Hyper-V
  virtualization software based on Host Compute System API, Remote Desktop
  ActiveX control, and XAML Islands.
- Kenji Mouri plans to write a technical book about the Hyper-V guest
  interfaces in the recent years, which needs a comprehensive, practical
  bare-metal example.
- Kenji Mouri has already made an out-of-box UEFI SDK for Visual Studio.
- Kenji Mouri is also a proud Microsoft MVP, he needs some Microsoft-related
  community contributions to help him continue to be in this role in the next
  years.

## Why choose WebAssembly as the bytecode ecosystem for Mobility?

- Kenji Mouri wants to use the hardware-neutral virtual bytecode ISA solution.
  He had tried to choose RISC-V as the bytecode solution, but he found many
  people hoped to do passthrough or AOT. He thinks binary portability are
  important, and he doesn't want to introduce any risk of breaking the binary
  portability.
- Kenji Mouri doesn't want to introduce something like RISC-V C extension, ELF
  binary format, etc. Although he is not satisfied with WASI, it's reasonable to
  not use WASI in WebAssembly, especially the embedded and IoT scenarios.
- Kenji Mouri hopes to have a more friendly interoperability mechanism between
  privileged environments and unprivileged environments. WebAssembly's export
  and import is much more friendly than the RISC-V's environment call.
- Kenji Mouri thinks the RISC-V ISA design is not friendly for implementing a
  portability, lightweight, software-based virtual machine environment.
- Kenji Mouri thinks we should hug to the Web-based ecosystem because most
  developers do the Web-based development scenarios. Also, WebAssembly is
  broadly used in Cloud (FaaS) and IoT (Embedded), which matches the designed
  scenario for him.
- Kenji Mouri also wants to use this project as the external extension
  infrastructure in NanaZip and AptxZip (NanaZip for POSIX).

## Why Mobility also has a bare-metal legacy x86-32 emulator flavor?

- Kenji Mouri hopes to have an immersive experience for legacy x86-32 operating
  systems on modern hardware with the UEFI Class 3 firmware.
- Kenji Mouri hopes to push the limits for ultra-lightweight system emulators.
- Kenji Mouri hopes to provide a fallback solution that can make hardware
  vendors reduce legacy things easier and more reasonably, which he thinks can
  help the industry.
