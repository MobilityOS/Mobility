# Mobility Session Storage (MoSS) Design Documents

**Work In Progress**

## On-disk Structures

All multi-byte integer fields are stored in little-endian byte order.

### Superblock (Block 0)

| Offset           | Size              | Field                      |
|------------------|-------------------|----------------------------|
| `0`              | 1 byte            | Signature (`'M'`)          |
| `1`              | 1 byte            | Signature (`'o'`)          |
| `2`              | 1 byte            | Signature (`'S'`)          |
| `3`              | 1 byte            | Signature (`'S'`)          |
| `4`              | 4 bytes           | Format Version             |
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
