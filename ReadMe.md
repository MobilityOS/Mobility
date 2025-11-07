# Mobility

Mobility is the rebirth of the original MobOS created by Kenji Mouri around 
2012. MobOS is an abandoned toy operating system project because he doesn't 
want to make a duplicate POSIX-based toy OS implementation.

Work In Progress

## Flavors

### Mobility OneCent (Otokonoko-based Core)

Lightweight bare-metal WebAssembly Virtual Machine Manager.

The WebAssembly Virtual Machine used in the Mobility project will finally be an
independent implementation called Otokonoko.

[Mobility.OneCent](Mobility.OneCent)

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

- Hyper-V Generation 2 Virtual Machines
  - x86-64 guests with minimum 32 MiB of memory

## Documents

- [Documents](Documents/ReadMe.md)
- [License](License.md)
- [Release Notes](ReleaseNotes.md)
- [Versioning](Versioning.md)
