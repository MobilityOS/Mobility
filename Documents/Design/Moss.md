# Mobility Session Storage (MoSS) Design Documents

**Work In Progress**

## On-disk Structures

All multi-byte integer fields are stored in little-endian byte order.

All padding bytes must be set to zero.

The byte range `[0, Block Size)` of each block must be written one byte at a
time in strictly ascending byte-offset order.

### Superblock (Block 0)

The superblock is immutable.

| Offset           | Size              | Field                      |
|------------------|-------------------|----------------------------|
| `0`              | 1 byte            | Signature (`'M'`)          |
| `1`              | 1 byte            | Signature (`'o'`)          |
| `2`              | 1 byte            | Signature (`'S'`)          |
| `3`              | 1 byte            | Signature (`'S'`)          |
| `4`              | 4 bytes           | Format Version (`1`)       |
| `8`              | 4 bytes           | Block Size                 |
| `12`             | 4 bytes           | Total Block Count          |
| `16`             | `Block Size - 24` | Padding                    |
| `Block Size - 8` | 2 bytes           | Padding                    |
| `Block Size - 6` | 4 bytes           | CRC-32                     |
| `Block Size - 2` | 1 byte            | Completion Marker (`0x55`) |
| `Block Size - 1` | 1 byte            | Completion Marker (`0xAA`) |

### Block (Block N)

Every block after Block 0 uses the following layout:

| Offset           | Size              | Field                      |
|------------------|-------------------|----------------------------|
| `0`              | 4 bytes           | Identifier                 |
| `4`              | 4 bytes           | Length                     |
| `8`              | `Block Size - 16` | Payload (& Padding)        |
| `Block Size - 8` | 2 bytes           | Generation / Padding       |
| `Block Size - 6` | 4 bytes           | CRC-32                     |
| `Block Size - 2` | 1 byte            | Completion Marker (`0x55`) |
| `Block Size - 1` | 1 byte            | Completion Marker (`0xAA`) |

#### Identifier

```
|F|E|D|C|B|A|9|8|7|6|5|4|3|2|1|0|F|E|D|C|B|A|9|8|7|6|5|4|3|2|1|0|
|0|   Content Identifier                                        |
|1|0| Session Identifier                                        |
|1|1| Session List Identifier                                   |
```

#### Length

```
|F|E|D|C|B|A|9|8|7|6|5|4|3|2|1|0|F|E|D|C|B|A|9|8|7|6|5|4|3|2|1|0|
|0| Remaining Size                                              |
|1| Block Index                                                 |
```

A Content object consists of zero or more Block Index blocks followed by exactly
one Stop block. If any Block Index blocks are present, they are indexed
consecutively in object order, starting at `0`. Each Block Index block contains
a full Payload and uses the Block Index form of the Length field.

The Stop block uses the Remaining Size form of the Length field and stores the
remaining Content Data in the first `Remaining Size` bytes of its Payload. The
rest of the Payload must be filled with zero-valued padding bytes.

The Remaining Size must be less than `Block Size - 16`. If the Content Data is
empty or its size is an exact multiple of `Block Size - 16`, the Stop block has
a Remaining Size of `0` and its entire Payload is padding.

#### Generation / Padding

Content objects are immutable, so this field is used as padding.

Session and Session List objects use this field as their generation number.

#### Payload

Session and Session List objects use the following payload layout:

| Offset | Size     | Field                       |
|--------|----------|-----------------------------|
| `0`    | 4 bytes  | Metadata Content Identifier |
| `4`    | Variable | Identifier Array            |

For Session objects, the Identifier Array contains Content Identifiers.

For Session List objects, the Identifier Array contains Session Identifiers.

Metadata Content is encoded as UTF-8 JSON.

Content objects use the following payload layout:

| Offset | Size     | Field        |
|--------|----------|--------------|
| `0`    | Variable | Content Data |

#### CRC-32

The CRC-32 field uses the standard CRC-32 algorithm (CRC-32/ISO-HDLC).

This is the conventional CRC-32 algorithm used by Ethernet, ZIP, gzip, zlib,
and PNG. CRC-32C and other CRC-32 variants are not used.

For the Superblock, the CRC-32 is calculated over the byte range
`[4, Block Size - 6)`. This includes all bytes after the Signature and before
the CRC-32 field.

For every block after Block 0, the CRC-32 is calculated over the byte range
`[0, Block Size - 6)`. This includes all bytes before the CRC-32 field.

The CRC-32 field and the Completion Marker bytes are not included in the
CRC-32 calculation.

## Size Limitations

The block size must be a power of 2.

- Theoretical Limits
  - Block Size
    - Minimum: 4 KiB
    - Maximum: 2 GiB
  - Storage Size
    - Minimum: 24 KiB
      - 1 Superblock
      - 1 Session List object with 1 Metadata Content
      - 1 Session object with 1 Metadata Content and 1 Content object
    - Maximum: 8 EiB - 2 GiB
- Practical Limits
  - Block Size
    - Minimum: 4 KiB
    - Maximum: 16 MiB
  - Storage Size
    - Minimum: 256 KiB
    - Maximum: 64 GiB
