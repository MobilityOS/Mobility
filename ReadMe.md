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

- Hyper-V Generation 2 Virtual Machines as the prototype and reference platform
  - x86-64 guests with minimum 48 MiB of memory  

> [!NOTE]
> Why choose Hyper-V Generation 2 Virtual Machines as the prototype and
> reference platform?
>
> Hyper-V Generation 2 Virtual Machines are one of the most popular
> para-virtualization platforms, and arguably the most aggressively
> para-virtualized:
> - No CSM support. Only provides 64-bit UEFI Class 3 firmware.
> - Only COM ports, RTC, x2APIC, and VMBus devices are available.

> [!NOTE]
> Why is the minimum memory size requirement 48 MiB for Hyper-V Generation 2
> Virtual Machines?
>
> Although 32 MiB is the minimum memory size supported by Hyper-V Generation 2
> Virtual Machines, it may not be sufficient in some scenarios:
> - On Windows 11 or later hosts, the Hyper-V UEFI firmware requires more than
>   32 MiB of memory to load UEFI binaries larger than 1 MiB.
> - On x86-64 CPUs that report more than 39 physical address bits, the UEFI
>   firmware consumes more memory due to its use of a 1:1 identity page table.
> However, requiring 64 MiB would be excessive for the project design, and
> 40 MiB may not be enough for future Windows versions. Here are some notes 
> based on running a customized 900 KiB size UEFI shell.
>   - Intel Core i7-7700K with Windows 10, version 2004 host: CPU reports 39
>     physical address bits, and 8 MiB free memory for 32 MiB virtual machines.
>   - Intel Core i7-11800H with Windows 11, version 25H2 host: CPU reports 39
>     physical address bits, and 2 MiB free memory for 32 MiB virtual machines.
>   - AMD Ryzen 9 9950X with Windows 11, version 25H2 host: CPU reports 48
>     physical address bits, and 2 MiB free memory for 34 MiB virtual machines,
>     because 32 MiB virtual machines will crash silently.

## Documents

- [Documents](Documents/ReadMe.md)
- [License](License.md)
- [Release Notes](ReleaseNotes.md)
- [Versioning](Versioning.md)
