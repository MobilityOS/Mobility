# Mobility

Mobility is a unikernel-style operating system project created by Kenji Mouri
for achieving his childhood operating system dreams:

- He doesn't want to make a duplicate POSIX-based toy OS implementation.
- He wants to see an ecosystem with binary-level portability.
- He wants to enlighten the industry via solving the real issues.

**Work In Progress**

## Flavors

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
