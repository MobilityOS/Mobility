# Mobility Session Storage (MoSS) Design Documents

**Work In Progress**

## Design Context

This section is non-normative.

Mobility Session Storage (MoSS) is a specialized filesystem for storing chat
sessions. It primarily targets MCU-class devices and virtual-machine guests
without a general-purpose operating system. A POSIX filesystem can be emulated
on top of Mobility Session Storage (MoSS), but doing so is strongly discouraged
because general-purpose filesystem workloads are not its intended use.

The design favors reliability and minimal erase/program activity over
throughput and initialization latency. Existing valid data is modified as
little as possible: the Superblock and Content objects are immutable, while
Session and Session List objects are replaced by newer generations.

When an FTL is present, physical wear leveling and bad-block management are
storage firmware responsibilities. On directly managed erase-before-write
media, the immutable and generational layout helps the backend minimize
erase/program operations. Medium-specific mechanisms such as ECC and bad-block
handling remain backend responsibilities.

Directly managed MCU storage is expected to be small enough for initialization
scanning. Larger virtual storage configurations are expected to provide memory
appropriate to their capacity for reconstructed indexes and caching. This
trade-off avoids mutable on-disk allocation maps, journals, indexes, and
checkpoints.

Zero-valued padding is intentional. It produces canonical block images and,
when the erased state is nonzero, helps offline binary analysis distinguish
serialized blocks from untouched storage.

## On-disk Structures

All multi-byte integer fields are stored in little-endian byte order.

All padding bytes must be set to zero.

Each block must be fully serialized into an in-memory image of exactly
`Block Size` bytes before being submitted to the underlying storage.
Serialization must write exactly one byte at each offset in
`[0, Block Size)`, proceeding in strictly ascending byte-offset order.

This requirement defines only the logical construction order of the block image.
It does not prescribe the write granularity or persistence order of the
underlying storage medium.

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

Every object consists of zero or more Block Index blocks followed by exactly one
Stop block. If any Block Index blocks are present, they are indexed
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

Generation numbers are compared using 16-bit serial number arithmetic. Given
generation numbers `A` and `B`, `A` is newer than `B` if and only if
`0 < ((A - B) mod (2 ^ 16)) < (2 ^ 15)`. A difference of exactly `2 ^ 15` is
ambiguous and must not occur between generations that may be compared.

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

## Operational Semantics

### Generation Selection

For each Session or Session List Identifier, the current generation is the
newest generation that forms a complete and valid object according to the
on-disk structure rules. Incomplete or invalid generations are ignored.

### Write Order

Objects written as part of an update must be completed in the following order:

1. Content objects referenced by Session generations, including their Metadata
   Content objects.
2. The Session generations that reference those Content objects, together with
   the Metadata Content objects referenced by Session List generations.
3. The Session List generations that reference those Sessions and Metadata
   Content objects.

Each stage must be completed before the next stage begins. Each object must be
written completely before any object that references it is written.

### Lazy Reclamation

Blocks are reclaimed only as part of a write operation.

The following blocks may be overwritten:

- Blocks belonging to superseded or invalid generations of Session or Session
  List objects.
- Blocks carrying a Content Identifier that is not referenced by any current
  Session or Session List generation.

Blocks reserved by an in-progress write must not be reclaimed. All other blocks
must be preserved.

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
