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
    - AMD Ryzen 9 9950X with Windows 11, version 26H1 host: CPU reports 48
      physical address bits, and 2 MiB free memory for 38 MiB virtual machines,
      because 36 MiB virtual machines will crash with hanging at firmware
      initialization.

## The simple specification for Mobility Retrovisor (Retro-V)

- Goals
  - Kenji Mouri hopes to have an immersive experience for legacy x86-32 
    operating systems on modern hardware with the UEFI Class 3 firmware.
  - Kenji Mouri hopes to push the limits for ultra-lightweight system emulators.
  - Kenji Mouri hopes to provide a fallback solution that can make hardware
    vendors reduce legacy things easier and more reasonably, which he thinks can
    help the industry.
  - Kenji Mouri hopes to have a portable DOS platform.
- Style: Type-1 Emulator, a term coined in homage to the Type-1 Hypervisor
- Memory layout
  - 1:1 identity page table for running the emulator
  - Use the largest page table entry which is available without any extra
    settings, for example, x64 is 2 MiB page, not the 1 GiB page
  - Maximum page table defined address is 4 GiB
- Emulated Hardware
  - CPU: Intel Pentium Overdrive
    - Interpreter with limited just in time binary translation
    - No translation cache, which is good for self modified code and reduce
      attack surface and errors
    - ISA Level: 586 without MMX
  - Motherboard: 486 and pure ISA as the baseline (a.k.a. Without PCI and ACPI)
    - Intel Pentium Overdrive is designed for that
    - Much simplified but enough for most old software
    - Also good for passthrough PCI and ACPI if users want to use bare-metal
    - Some emulated devices can be disabled if bare-metal has alternatives
  - Input: PS/2 Keyboard & Mouse
  - Graphics: Cirrus Logic 54xx Compatible
    - Because Windows XP also supports that
    - Cirrus Logic 5434 4 MiB, for 1024x768 support
      - Windows 2000 and Windows XP can identify 4 MiB video memory, but only
        supports 1024x768 16bpp like the 2 MiB video memory.
      - Windows 98 can use 1024x768 24bpp with 4 MiB video memory.
  - Sound: Sound Blaster 16 Compatible
  - Network: Novell NE2000 Compatible
  - Firmware: High Level Emulation BIOS
    - Only with necessary stubs in guest
    - Reduce the maintenance cost
    - Improve the performance
    - Optional MO-DOS mode (Mobility DOS, High Level Emulation DOS)
  - Not mentioned devices:
    - Follow the mainstream legacy PC conventions
    - For example, storage will use IDE
- Guest OS Support
  - DOS
  - DOS Applications if MO-DOS mode enabled
  - Windows XP or earlier
    - Windows 98 and Windows 2000 should have out-of-the-box experience.
    - You need to use upgrade installation from Windows 2000 if you want to use
      Windows XP with proper configured graphics and network drivers.
- Possible Independent Backends
  - Hyper-V Generation 2 Virtual Machines
    - Maintained by Kenji Mouri
    - The reference and prototype platform
    - Retro-V will use 8 MiB memory for itself, including:
      - Unikernel a.k.a. runtime part including the page table
      - Emulator contexts including the video memory
      - Not including the guest memory and memory from MMIO
  - UEFI without calling ExitBootServices
    - Maintained by Kenji Mouri
    - Not the goal if the reference and prototype platform is not ready
  - VirtIO compatible platform like KVM and Xen
    - Maintained by community
    - Not the goal if the reference and prototype platform is not ready
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
