# Mobility Design Documents

Work In Progress

## Why choose Hyper-V Generation 2 Virtual Machines as the reference platform?

- Hyper-V Generation 2 Virtual Machines represent one of the most widely adopted
  para-virtualization platforms, and arguably the most aggressively
  para-virtualized. This makes them an ideal foundation for Kenji Mouri to
  implement compact, bootable bare-metal instances with potentially
  beyond-expectation results:
  - No CSM support. Only 64-bit UEFI Class 3 firmware is provided.
  - Only COM ports, RTC, x2APIC, and VMBus devices are available.
- NanaBox, created by Kenji Mouri and already part of his daily workflow, is a
  third-party, lightweight, XAML-based, out-of-the-box experience-oriented
  Hyper-V virtualization client built on the Host Compute System API, Remote
  Desktop ActiveX control, and XAML Islands.
- Kenji Mouri plans to author a technical book on Hyper-V guest interfaces in
  the coming years, which calls for a comprehensive and practical bare-metal
  reference implementation.
- Kenji Mouri has already developed a ready-to-use UEFI SDK for Visual Studio.
- As a proud Microsoft MVP, Kenji Mouri also seeks meaningful Microsoft-related
  community contributions to support his continued participation in the program
  in the years ahead.

## Why require minimum 48 MiB memory for Hyper-V Generation 2 Virtual Machines?

- Although 32 MiB is the minimum memory size supported by Hyper-V Generation 2
  Virtual Machines, it may not be sufficient in some scenarios:
  - On Windows 11 or later hosts, the Hyper-V UEFI firmware requires more than
    32 MiB of memory to load UEFI binaries larger than 1 MiB.
  - On x86-64 CPUs that report more than 39 physical address bits, the UEFI
    firmware consumes more memory due to its use of a 1:1 identity page table.
- However, requiring 64 MiB would be excessive for the project design, and
  40 MiB may not be enough for future Windows versions. Here are some notes 
  based on running a customized 900 KiB size UEFI shell.
    - Intel Core i7-7700K with Windows 10, version 2004 host: CPU reports 39
      physical address bits, and 8 MiB free memory for 32 MiB virtual machines.
    - Intel Core i7-11800H with Windows 11, version 25H2 host: CPU reports 39
      physical address bits, and 2 MiB free memory for 32 MiB virtual machines.
    - AMD Ryzen 9 9950X with Windows 11, version 25H2 host: CPU reports 48
      physical address bits, and 2 MiB free memory for 34 MiB virtual machines,
      because 32 MiB virtual machines will crash silently.

## The simple specification for Mobility Retrovisor (Retro-V)

- Goals
  - Kenji Mouri hopes to have an immersive experience for legacy x86-32 
    operating systems on modern hardware with the UEFI Class 3 firmware.
  - Kenji Mouri hopes to push the limits for ultra-lightweight system emulators.
  - Kenji Mouri hopes to provide a fallback solution that can make hardware
    vendors reduce legacy things easier and more reasonably, which he thinks can
    help the industry.
  - Kenji Mouri hopes to have a portable DOS platform.
- Style: Type 1 Emulator (the term salute to Type 1 Hypervisor)
- Memory layout
  - 1:1 identity page table for running the emulator
  - Maximum page table defined address is 4 GiB
  - Retro-V will use 8 MiB memory for itself and video memory
- Emulated Hardware
  - CPU: Intel Pentium Overdrive
    - Interpreter with limited just in time binary translation
    - No translation cache, which is good for self modified code and reduce
      attack surface and errors
    - ISA Level: 586 without MMX
  - Motherboard: 486 and pure ISA
    - Intel Pentium Overdrive is designed for that
    - Much simplified but enough for most old software
    - Also good for passthrough PCI and ACPI if users want to use bare-metal
    - Some emulated devices can be disabled if bare-metal has alternatives
  - Input: PS/2 Keyboard & Mouse
  - Graphics: Cirrus Logic Compatible 54xx Compatible
    - Because Windows XP also supports that
    - Cirrus Logic 5434 4 MiB, for 1024x768 support
      - Windows 2000 and Windows XP can identify 4 MiB video memory, but only
        supports 1024x768 16bpp like the 2 MiB video memory.
      - Windows 98 can use 1024x768 24bpp with 4 MiB video memory.
  - Sound: Sound Blaster 16 Compatible
  - Network: Novell NE2000 Compatible
  - Firmware: High Level Emulation BIOS, 
    - Only with necessary stubs in guest
    - Reduce the maintenance cost
    - Improve the performance
    - Optional MO-DOS mode (Mobility DOS, High Level Emulation DOS)
- Guest OS Support
  - DOS
  - DOS Applications if MO-DOS mode enabled
  - Windows XP or earlier
    - Windows 98 and Windows 2000 should have out-of-the-box experience.
    - You need to use upgrade installation from Windows 2000 if you want to use
      Windows XP with proper configured graphics and network drivers.
- Host Support
  - Hyper-V Generation 2 Virtual Machines (maintained by Kenji Mouri)
  - UEFI without calling ExitBootServices (maintained by Kenji Mouri)
  - VirtIO compatible platform like KVM and Xen (maintained by community)
- User Experience
  - Ctrl+Alt+Del for runtime menu
    - Send Ctrl+Alt+Del to guest
    - Cancel
    - Change guest resource settings like floppy and cdrom
    - Force Poweroff
    - Force Restart
  - Version notice and Ctrl+Alt+Del notice at initialization
  - Support Hyper-V Basic Session and Enhanced Session
  - If the legacy OS partition is FAT series, we can add /EFI/Boot/bootx64.efi
    a.k.a. Retro-V itself to make the legacy OS great again
