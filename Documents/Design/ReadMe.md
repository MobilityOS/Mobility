# Mobility Design Documents

**Work In Progress**

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
- Windows Desktop Application, for development use only.

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

The Windows Desktop Application platform target is intended for development use
only for the following reasons:

- Users already have much better choices for conventional application scenarios
  if they can accept running software on top of existing operating systems.
- Mobility is a complex long-term project, so having a Windows desktop
  application platform target can be very helpful for developing parts that are
  not platform-specific.

Kenji Mouri is currently working only on the following possible platform
targets:

- Hyper-V Generation 2 Virtual Machines
- Windows Desktop Application

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
