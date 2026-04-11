# Mobility

Mobility is a unikernel-style operating system project created by Kenji Mouri
to realize his childhood operating system dream:

- He does not want to build yet another POSIX-based toy OS.
- He wants to see an ecosystem with binary-level portability.
- He wants to push the industry forward by solving real problems.

Kenji Mouri has pursued this idea since 2012. Given the practical limits of his
time, resources, and technical capacity, he concluded that the most feasible way
to realize this vision is through a specialized Type-1 Emulator, a term coined
in homage to the Type-1 Hypervisor. It is an emulator that runs directly on bare
metal while emulating a pure legacy x86 ISA base platform, effectively a
486-class ISA machine paired with a 586-class Pentium OverDrive CPU. This pure
ISA base is intentional: rather than making newer standards such as PCI, AGP,
or PCIe part of the canonical platform, it leaves open the possibility of direct
passthrough for those who wish to experiment with modern, real hardware.

**Work In Progress**

## Components

### Mobility Retrovisor (Retro-V)

Immersive bare-metal legacy x86-32 virtualization solution for modern x86-64
UEFI Class 3 platform.

[Mobility.Retrovisor](Mobility.Retrovisor)

### Mobility Hyper-V Guest Compatibility Shim

Simple compatibility shim for booting x86-64 Windows XP / Vista / 7 on Hyper-V
Generation 2 Virtual Machines without modifications in some scenarios.

[Mobility.HvGcs](Mobility.HvGcs)

### Mobility Hyper-V Lightweight Debugger for Guests

Lightweight bare-metal debugger for Hyper-V Generation 2 Virtual Machines guests.

Note: Lightweight Debugger for Guests a.k.a. LDG is the acronym of the name of a
professor ("Le De Guang" as the reading sequence) who gives me some inspiration
to create this project as the middle stage of the whole Mobility project.

[Mobility.HvLdg](Mobility.HvLdg)

## Supported Platforms

- Hyper-V Generation 2 Virtual Machines as the reference platform
  - x86-64 guests with minimum 48 MiB of memory

> [!NOTE]
> Read [Mobility Design Documents] for more
> information about the decisions.

[Mobility Design Documents]: Documents/Design/ReadMe.md

## Documents

- [Documents](Documents/ReadMe.md)
- [License](License.md)
- [Release Notes](ReleaseNotes.md)
- [Versioning](Versioning.md)
