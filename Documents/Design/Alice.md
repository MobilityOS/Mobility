# Mobility Alice Design Documents

Mobility Alice is a lightweight implementation of a Microsoft Remote Desktop
Protocol (RDP) server for embedded and bare-metal environments.

Note: In the following document, RDP refers to Microsoft Remote Desktop Protocol
(RDP) in non-reference text for simplified descriptions.

**Work In Progress**

## Why named Alice?

Read https://touhou.fandom.com/wiki/Alice_Margatroid for more information.

## RDP References

Here are the RDP specifications referenced in this project:

- RDP 4.0
  - [MS-RDPBCGR] - Remote Desktop Protocol: Basic Connectivity and Graphics
    Remoting
  - [MS-RDPEGDI] - Remote Desktop Protocol: Graphics Device Interface (GDI)
    Acceleration Extensions
- RDP 5.0
  - [MS-RDPECLIP] - Remote Desktop Protocol: Clipboard Virtual Channel Extension
- RDP 5.1
  - [MS-RDPEA] - Remote Desktop Protocol: Audio Output Virtual Channel Extension
- RDP 6.0
  - [MS-RDPEDYC] - Remote Desktop Protocol: Dynamic Channel Virtual Channel
    Extension
- RDP 7.0
  - [MS-RDPEAI] - Remote Desktop Protocol: Audio Input Redirection Virtual
    Channel Extension
- RDP 8.1
  - [MS-RDPEDISP] - Remote Desktop Protocol: Display Update Virtual Channel
    Extension

[MS-RDPBCGR]:
  https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpbcgr
[MS-RDPEGDI]:
  https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpegdi
[MS-RDPECLIP]:
  https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpeclip
[MS-RDPEA]:
  https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpea
[MS-RDPEDYC]:
  https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpedyc
[MS-RDPEAI]:
  https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpeai
[MS-RDPEDISP]:
  https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpedisp

## Scenarios

- Hyper-V Enhanced Session
- RDP support integration for lightweight operating systems
- Headless management for MCU-based devices

## Role

- Alice only handles plaintext RDP packets. Transport and security layers are
  handled outside Alice.
- [MS-RDPBCGR] and [MS-RDPEGDI] are the minimum protocol specifications.
- Alice implements only the necessary parts.
- Alice supports both framebuffer-primary and GDI-drawing-order-primary output
  models.

## Rationale

This design is acceptable for the following reasons:

- Hyper-V Enhanced Session over VMBus can be understood as unencrypted RDP
  traffic transported over VMBus via the SynthRdp VMBus channels. The Hyper-V
  host side can handle the security layer for external clients, while the guest
  side receives plaintext RDP traffic.
- Because the Hyper-V host redirects plaintext RDP packets to the guest without
  modifying the RDP payload, an outer adapter may need to normalize the first
  RDP packet, also known as the [Client X.224 Connection Request PDU], before
  passing it to Alice.
  - Specifically, the [requestedProtocols field in RDP Negotiation Request
    (RDP_NEG_REQ)] may need to be changed from `0x0000000B` to `0x00000000`.
  - `0x0000000B` means `PROTOCOL_SSL | PROTOCOL_HYBRID | PROTOCOL_HYBRID_EX`.
  - `0x00000000` means `PROTOCOL_RDP`.
  - This allows a plaintext RDP server in the guest to handle the connection.
- Microsoft RDP ActiveX Control supports connections to plaintext RDP servers.

[Client X.224 Connection Request PDU]:
  https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpbcgr/18a27ef9-6f9a-4501-b000-94b1fe3c2c10
[requestedProtocols field in RDP Negotiation Request (RDP_NEG_REQ)]:
  https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpbcgr/902b090b-9cb3-4efc-92bf-ee13373371e3
