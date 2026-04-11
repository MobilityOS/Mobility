# Mobility OneCent Design Documents

Mobility OneCent is a lightweight bare-metal WebAssembly Virtual Machine
Manager, which based on homemade WebAssembly Virtual Machine implementation
called Otokonoko.

> [!NOTE]
> This subproject is paused because of the following reasons:
> - It's not a suitable timing for introducing the external extension
>   infrastructure in NanaZip and AptxZip (NanaZip for POSIX).
> - Kenji Mouri think the scenarios for that is mainly for RTOS. Maybe we should
>   have a Linux kernel module called wasm.ko to do the similar thing.
> - Kenji Mouri think he should put his precious efforts to Mobility Retrovisor.
> This design document will be kept for historic purpose.

Originally, the whole Mobility project was the rebirth of the original MobOS
created by Kenji Mouri around  2012. MobOS is an abandoned toy operating system
project because he doesn't want to make a duplicate POSIX-based toy OS
implementation.

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
