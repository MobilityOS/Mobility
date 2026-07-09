# Mobility Design Documents

Work In Progress

## Architecture Overview of Mobility

Mobility is a unikernel-style operating system project created by Kenji Mouri to
realize his childhood operating system dream:

- He does not want to build yet another POSIX-based toy OS.
- He wants to see an ecosystem with binary-level portability.
- He wants to push the industry forward by solving real problems.

Therefore, Mobility generally consists of both a Platform part and a Unikernel
part, also known as Mobility itself. The Platform part is the environment in
which Mobility runs, including local or cloud virtual machines and bare-metal
platforms such as SoCs and even MCUs.

The Platform part of Mobility can target the following possible platforms:

- Hyper-V Generation 2 Virtual Machines, including Windows, Azure, OpenVMM, etc.
- VirtIO-compatible virtualization platforms, such as KVM and Xen.
- UEFI without calling ExitBootServices(), as a fallback, especially for
  bare-metal platforms with UEFI support.
- Windows desktop application, for development use only.

The Reference Platform is Hyper-V Generation 2 Virtual Machines for the
following reasons:

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

The Windows desktop application platform target is intended for development use
only for the following reasons:

- Users already have much better choices for conventional application scenarios
  if they can accept running software on top of existing operating systems.
- Mobility is a complex long-term project, so having a Windows desktop
  application platform target can be very helpful for developing parts that are
  not platform-specific.

Kenji Mouri is currently working only on the following possible platform
targets:

- Hyper-V Generation 2 Virtual Machines
- Windows desktop application

Other platform targets are not Kenji Mouri's responsibility until the platform
targets mentioned above are finished and the first stable release is released.
Those platform targets also require help from the community.

## Reference Platform Specification for Mobility

The Reference Platform is Hyper-V Generation 2 Virtual Machines. The following
requirements apply to this platform target:

- Require x86 (64-Bit) guests with a minimum of 48 MiB of memory.
- Use a 1:1 identity page table, with the largest page-table entry available
  without any extra settings. For example, on x86 (64-Bit), use 2 MiB pages
  instead of 1 GiB pages.

The current page table layout is chosen to reduce memory usage, and it is also
natural for unikernel-style operating system projects.

Although Hyper-V Generation 2 Virtual Machines support ARM (64-Bit) guests, and
Kenji Mouri has created the https://github.com/ProjectMile/Mile.HyperV project
and collected a rich set of Hyper-V guest interface definitions for both x86
(64-Bit) and ARM (64-Bit), given the practical limits of Kenji Mouri's time,
resources, and technical capacity, Kenji Mouri is currently working only on x86
(64-Bit) guests on Hyper-V Generation 2 Virtual Machines until this target is
finished and the first stable release is released.

The following are the reasons for requiring a minimum of 48 MiB of memory for
Hyper-V Generation 2 Virtual Machines:

- Although 32 MiB is the minimum memory size supported by Hyper-V Generation 2
  Virtual Machines, it may not be sufficient in some scenarios:
  - On Windows 11 or later hosts, the Hyper-V UEFI firmware requires more than
    32 MiB of memory to load UEFI binaries larger than 1 MiB.
  - On x86 (64-Bit) CPUs that report more than 39 physical address bits, the
    UEFI firmware consumes more memory due to its use of a 1:1 identity page
    table.
- However, requiring 64 MiB would be excessive for the project design, and
  40 MiB may not be enough for future Windows versions. Here are some notes
  based on running a customized 900 KiB UEFI shell:
  - Intel Core i7-7700K with Windows 10, version 2004 host: CPU reports 39
    physical address bits, and 8 MiB free memory for 32 MiB virtual machines.
  - Intel Core i7-11800H with Windows 11, version 25H2 host: CPU reports 39
    physical address bits, and 2 MiB free memory for 32 MiB virtual machines.
  - AMD Ryzen 9 9950X with Windows 11, version 25H2 host: CPU reports 48
    physical address bits, and 2 MiB free memory for 34 MiB virtual machines,
    because 32 MiB virtual machines will crash silently.
  - AMD Ryzen 9 9950X with Windows 11, version 26H1 host: CPU reports 48
    physical address bits, and 2 MiB free memory for 38 MiB virtual machines,
    because 36 MiB virtual machines will hang during firmware initialization.
- The 48 MiB baseline also provides a good low-memory starting point for
  developing Mobility parts that are not platform-specific, which may help
  future ports to bare-metal platforms such as SoCs and even MCUs.

## Architecture Overview of Mobility Retrovisor (Retro-V)

Currently, Mobility Retrovisor (Retro-V) is the main practical form of Mobility.
It is intended to provide an immersive experience for legacy x86 (32-bit)
operating systems on modern hardware with UEFI Class 3 firmware, to provide a
portable DOS platform, and to push the limits of ultra-lightweight system
emulators. Mobility Retrovisor (Retro-V) is also intended to provide a fallback
solution that can make it easier and more reasonable for hardware vendors to
reduce legacy components, which Kenji Mouri believes can help the industry.

Given the practical limits of Kenji Mouri's time, resources, and technical
capacity, he concluded that the most feasible way to realize this vision is
through a specialized Type-1 Emulator, a term coined in homage to the Type-1
Hypervisor.

Mobility Retrovisor (Retro-V) is an emulator that runs directly on the platform
layer while emulating a pure legacy x86 ISA-bus platform, effectively a
486-class ISA machine paired with a 586-class Pentium OverDrive CPU. This pure
ISA baseline is intentional: rather than making newer standards such as PCI,
AGP, or PCIe part of the canonical emulated platform, Retro-V leaves open the
possibility of direct passthrough for users who want to experiment with modern,
real hardware.

In the Unikernel part, Mobility Retrovisor (Retro-V) consists of the Retrovisor
part, namely Mobility Retrovisor (Retro-V) itself, and the Guest part, which is
the emulated environment provided by Mobility Retrovisor (Retro-V).

Mobility Retrovisor (Retro-V) is designed to support the following scenarios:

- DOS
- DOS applications, if Mobility DOS (MO-DOS) mode is enabled
- Windows XP or earlier
  - Windows 98 and Windows 2000 should have an out-of-the-box experience.
  - Windows XP requires an upgrade installation from Windows 2000 if users want
    properly configured graphics and network drivers.
- Other legacy x86 (32-bit) operating systems that support a pure legacy x86
  ISA-bus platform.

Windows XP and operating systems outside the primary Mobility Retrovisor
(Retro-V) scenarios may be better served by other solutions when users need a
full general-purpose virtual machine experience.

## Reference Platform Specification for Mobility Retrovisor (Retro-V)

The following extra requirements apply to Mobility Retrovisor (Retro-V) on this
platform target:

- The maximum page-table-defined address is 4 GiB.
- Mobility Retrovisor (Retro-V) will keep its own memory usage within 8 MiB,
  including:
  - Reserved memory for UEFI Runtime Services, ACPI, etc.
  - The Mobility Retrovisor (Retro-V) executable image.
  - Platform contexts, including:
    - Memory-management overhead, including the page table.
    - Contexts for using Hyper-V Guest Interfaces.
  - Emulator contexts, including virtual CPU register contexts.
- The 8 MiB budget does not include guest RAM, guest MMIO like video memory and
  user payloads.
- The 8 MiB budget does not include platform MMIO.

## Emulated Hardware Specification for Mobility Retrovisor (Retro-V)

### Hardware List

- Processor: Emulates an Intel Pentium OverDrive processor.
- Platform: Implements a pure 486 ISA platform with the following components:
  - MC146818-compatible CMOS and Real-Time Clock (RTC)
  - Two 8237A-compatible DMA controllers
  - Two cascaded 8259A-compatible Programmable Interrupt Controllers (PICs)
  - 8253/8254-compatible Programmable Interval Timer (PIT)
  - Two 16550A-compatible UART controllers that support up to two serial ports
  - 8042-compatible Keyboard Controller Device and A20 Gate
  - ISA Bus
  - Floppy disk controller
  - PC Speaker
- Floppy Disk Drive: Supports a single 3.5-inch 1.44 MB floppy disk drive and
  mapping raw floppy drive images.
- Serial (COM) Port: Emulates up to two serial ports: COM1 and COM2.
- Mouse: Emulates a standard PS/2 mouse.
- Keyboard: Emulates a standard PS/2 keyboard.
- Network Adapter: Emulates a Novell NE2000 Compatible network adapter.
- Memory: Supports nearly 4 GiB of guest RAM.
- Sound Card: Emulates a Sound Blaster 16 Compatible sound card.
- Video Card: Emulates a Cirrus Logic 54xx compatible graphics adapter with 4
  MiB of Video RAM (VRAM), VGA, and SVGA support.
- IDE/ATAPI Storage: Emulates up to four IDE/ATAPI devices on the Primary and
  Secondary IDE Channels, including hard drives and CD-ROM drives or ISO images.

### IRQ

- IRQ0: Programmable Interval Timer (PIT)
- IRQ1: Keyboard Controller
- IRQ2: Programmable Interrupt Controllers (PIC) cascade
- IRQ3: COM2
- IRQ4: COM1
- IRQ5: Sound Card
- IRQ6: Floppy Disk Controller
- IRQ7: Unused
- IRQ8: Real-Time Clock (RTC)
- IRQ9: Unused
- IRQ10: Network Adapter
- IRQ11: Unused
- IRQ12: PS/2 Mouse
- IRQ13: x87 FPU
- IRQ14: Primary IDE Channel
- IRQ15: Secondary IDE Channel

### DMA

- DMA0: Unused
- DMA1: Sound Card (8-bit DMA)
- DMA2: Floppy Disk Controller
- DMA3: Unused
- DMA4: DMA controller cascade
- DMA5: Sound Card (16-bit DMA)
- DMA6: Unused
- DMA7: Unused

### I/O Port

- 0x0000 - 0x000F: DMA Controller
- 0x0020 - 0x0021: Programmable Interrupt Controllers (PIC)
- 0x0040 - 0x0043: Programmable Interval Timer (PIT)
- 0x0060 - 0x0064: Keyboard Controller, PC Speaker
- 0x0070 - 0x0071: CMOS and Real-Time Clock (RTC)
- 0x0080 - 0x008F: DMA Page Registers
- 0x0092: System Control Port A
- 0x0094: POS / Video Card
- 0x00A0 - 0x00A1: Programmable Interrupt Controllers (PIC)
- 0x00C0 - 0x00DF: DMA Controller
- 0x0102: POS / Video Card
- 0x0170 - 0x0177: Secondary IDE Channel
- 0x01F0 - 0x01F7: Primary IDE Channel
- 0x0220 - 0x022F: Sound Card
- 0x02F8 - 0x02FF: COM2
- 0x0300 - 0x031F: Network Adapter
- 0x0376: Secondary IDE Channel
- 0x0388 - 0x038B: Sound Card
- 0x03B0 - 0x03DF: Video Card
- 0x03F0 - 0x03F5: Floppy Disk Controller
- 0x03F6: Primary IDE Channel
- 0x03F7: Floppy Disk Controller
- 0x03F8 - 0x03FF: COM1
- 0x46E8: Video Card

### Reasons

Here are the real minimum hardware requirements for Windows XP x86 (32-bit):

- An x86 processor compatible with the original Pentium, also known as i586 or
  P5, because CPUID and CMPXCHG8B are required.
- Legacy BIOS firmware, which is supported by all x86 (32-bit) Windows versions.
- ACPI-compliant firmware is not required for Windows XP. Beginning with Windows
  Vista and Windows Server 2008, ACPI-compliant firmware dated January 1, 1999
  or later is required.
- The memory and storage requirements can be much more flexible.

Therefore, the actual oldest practical platform for running Windows XP x86
(32-bit) can be:

- Intel Pentium OverDrive, a processor with the original Pentium, also known as
  i586 or P5, ISA level without MMX.
- A pure 486 ISA platform, because Intel Pentium OverDrive was designed as an
  upgrade processor for 486 platform users, and 486 platforms can have pure ISA
  boards.

This combination is much simpler than later PC platforms, but is still enough
for most old software. For example, the baseline is pure ISA without PCI and
ACPI. This also keeps future bare-metal scenarios simpler if users want to use
passthrough PCI and ACPI in future releases. In that scenario, some emulated
devices can be disabled if bare-metal alternatives are available.

Mobility Retrovisor (Retro-V) also chooses a Cirrus Logic 54xx compatible
graphics adapter with 4 MiB of Video RAM (VRAM) because:

- Windows 9x and Windows 2000 have built-in drivers for this device.
- Windows XP also has the built-in driver binaries, but they are only usable
  through an upgrade installation from Windows 2000. Upgrade installation from
  Windows 9x has not been tested.
- Windows 2000 and Windows XP can identify 4 MiB of video memory, but only
  support 1024x768 16bpp, the same as with 2 MiB of video memory.
- Windows 98 can use 1024x768 24bpp with 4 MiB of video memory.

## User Experience Specification for Mobility Retrovisor (Retro-V)

- Runtime Menu
  - Entry Points
    - Hyper-V Generation 2 Virtual Machines: Ctrl + Alt + Del
    - Windows desktop application: top-bar button in the emulator window
    - Other Platform: T.B.D.
  - Menu Items
    - Send Ctrl + Alt + Del to the Guest.
    - Change Guest runtime settings.
      - Change the Floppy images.
      - Change the CD-ROM images.
    - Force Power Off the Guest.
    - Force Restart the Guest.
    - Return to the Guest, a.k.a. Cancel.
- Floppy Support
  - Hyper-V Generation 2 Virtual Machines
    - Only read-only mode is supported.
    - Runtime Floppy image changes are supported only among the Floppy images
      preloaded during Mobility Retrovisor (Retro-V) initialization.
    - The first preloaded Floppy image will be selected at Power On.
  - Windows desktop application
    - Read and write are supported.
    - Changing to any suitable Floppy image is supported.
  - Other Platform: T.B.D.
- CD-ROM Support
  - Hyper-V Generation 2 Virtual Machines
    - Only raw block devices exposed by Virtual Machines are supported.
    - CD-ROM images are not supported.
    - Runtime changes are not supported.
  - Windows desktop application
    - Only CD-ROM images are supported.
    - Changing to any suitable CD-ROM image is supported.
  - Other Platform: T.B.D.
- Hard Drive Support
  - Hyper-V Generation 2 Virtual Machines
    - Only raw block devices exposed by Virtual Machines are supported.
    - Hard Drive images are not supported.
  - Windows desktop application
    - Only Hard Drive images are supported.
  - Other Platform: T.B.D.
- Interaction
  - Hyper-V Generation 2 Virtual Machines
    - Hyper-V Basic Session
    - Hyper-V Enhanced Session over VMBus
      - Based on Mobility Alice
  - Windows desktop application
    - Standard Emulator Window Mode
    - Microsoft Remote Desktop Protocol (RDP) over TCP
      - Based on Mobility Alice
  - Other Platform: T.B.D.
- Initialization
  - Parse and load the configuration file.
  - Display version information.
  - Display Runtime Menu entry information.
  - If the legacy OS partition uses a FAT-family file system, Retro-V can add
    `/EFI/Boot/bootx64.efi`, a.k.a. Retro-V itself, to make the legacy OS great
    again.

## Stage 1 of the First Stable Release for Mobility Retrovisor (Retro-V)

For easier implementation, the First Stable Release should include a Stage 1.
This stage is intended to help implement and test emulated devices.

- Emulated Hardware
  - Change the Processor to Intel 8088.
  - Change the Video Card to Standard VGA.
  - No Sound Card.
- Interaction
  - No Hyper-V Enhanced Session over VMBus.
  - No Microsoft Remote Desktop Protocol (RDP) over TCP.

## Open Design Items for Mobility Retrovisor (Retro-V)

- Emulation Implementation
  - Interpreter with limited just in time binary translation
  - No translation cache, which is good for self modified code and reduce attack
    surface and errors
- Firmware: High Level Emulation BIOS
  - Only with necessary stubs in guest
  - Reduce the maintenance cost
  - Improve the performance
  - Optional MO-DOS mode (Mobility DOS, High Level Emulation DOS)
