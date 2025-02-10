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
