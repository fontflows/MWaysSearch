# AED II - Trabalho 1 - Parte 2

B-Tree (order `m`) implementation with disk-resident index, search (`mSearch`), and insertion (`insertB`).

## Authors
- Francisco Eduardo Fontenele - 15452569
- Vinicius Botte - 15522900

## Description

Disk-based B-Tree that indexes an integer key field. The tree order `m` is chosen by the user at runtime and persisted in the index header to prevent mismatches. The on-disk node layout is fixed by `MAX_M` to keep records aligned, while the effective order is `m` (`3 ≤ m ≤ MAX_M`). The program supports interactive search and insertion, creates the initial index from a text file, integrates a main data file, and can export the current index back to text.

## Features

- Parametric B-Tree order `m` selected at startup
- Fixed on-disk node layout (`MAX_M`) for a stable binary format
- Index header at position 0 (`n = -1`) storing `m` and `root`; runtime validates `m`
- Create binary index from text (`createFromText`)
- Export binary index back to text (`exportToText`)
- Disk-based search (`mSearch`) returning `(node, position, found)`
- Disk-based insertion (`insertB`) with node split and median promotion
- Root creation when the old root splits (keeps root at logical position 1)
- Tree display in the required tabular format
- Main data file (`data.bin`) with fixed-size records `{ int key; char payload[64]; }`
- I/O counters for index and data operations (reads/writes per operation)
- Interactive menu: display, search, insert, print main file, exit
- Optional export of the updated index back to the selected text file after insertion

## Requirements

- C++20
- CMake 3.29+
- Tested on Windows (CLion 2024.2.3)

## Build and Run

```bash
mkdir build
cd build
cmake ..
cmake --build .
./MWaysSearch
```

Alternatively, open the project in CLion and run the `MWaysSearch` target.

## Usage

1. Select the input text file to load (`mvias*.txt`).
2. Enter the B-Tree order `m` matching the selected file:
    - `mvias.txt`, `mvias2.txt`, `mvias3.txt`, `mvias4.txt` → use `m = 3`
    - `mvias5.txt` → use `m = 5`
3. The program recreates `mvias.bin` (index) and `data.bin` (main file) from the chosen text file, validates `m` via the header, and shows the index.
4. Menu:
    - 1. Search key: prints `key (node,position,found)`, index I/O, and the record from `data.bin` if found
    - 2. Insert key: inserts into the index and `data.bin`, shows I/O, and optionally exports the new index back to the selected `.txt`
    - 3. Print main file: lists all records from `data.bin`
    - 4. Exit

Note: As `mvias.bin` and `data.bin` are recreated on startup from the selected `.txt`, insertions do not persist across runs unless you export the updated index back to text.

## File Structure

```
MWaysSearch/
├── CMakeLists.txt
├── main.cpp
├── MWayTree.h
├── MWayTree.cpp
├── DataFile.h
├── DataFile.cpp
├── mvias.txt
├── mvias2.txt
├── mvias3.txt
├── mvias4.txt
└── mvias5.txt
```

Generated at runtime in the build directory:
- `mvias.bin` — binary index with header at position 0
- `data.bin` — main data file

## File Format

Text files (`mvias*.txt`) list nodes, one per line, in the format:
```
n A0 K1 A1 K2 A2 ... Kn An
```
Where:
- `n`: number of keys in the node
- `A0`: leftmost child
- `Ki`: key `i`
- `Ai`: child right of key `Ki`

Binary index `mvias.bin`:
- Position 0: header node with `n = -1`, `keys[0] = m`, `children[0] = root`
- Positions 1..N: nodes with fixed arrays sized by `MAX_M`

Main data file `data.bin`:
- Fixed-size records: `{ int key; char payload[64]; }` (appended on insertion)

## Algorithm Notes

- `mSearch` traverses from root, comparing keys and following child pointers until it finds the key or the leaf position where it should be inserted.
- `insertB` inserts bottom-up, splitting full nodes, promoting the median, and creating a new root if required.
- On root creation, existing nodes at positions `≥ 1` are shifted to keep the root at position 1; child references are adjusted accordingly.
- The index header persists the chosen `m` and `root`; on open, `m` is validated to avoid misinterpreting indices built with a different order.
- I/O counters report per-operation reads/writes for both index and data files.

## Testing Guide

Important:
- Use `m = 3` for `mvias.txt`, `mvias2.txt`, `mvias3.txt`, `mvias4.txt`.
- Use `m = 5` for `mvias5.txt`.
- Compare the exact output line `key (node,position,found)` for search tests.
- For insertion tests, re-display the tree and compare the specified node lines.

The functional tests remain the same. Additional informational lines (I/O counters and optional main file prints) do not affect the expected structural outputs.

### Test Files Description

1. `mvias.txt`: Tree 1 with standard numbering
2. `mvias2.txt`: Tree 2 with standard numbering
3. `mvias3.txt`: Tree 1 with alternative numbering
4. `mvias4.txt`: Tree 2 with alternative numbering
5. `mvias5.txt`: Tree 3 for `m = 5` (broader node capacity)

### Tree 1 (Standard Numbering) — `mvias.txt` (m = 3)

Structure (displayTree):
```
 1 2, 2,( 20, 3),( 40, 4)
 2 2, 0,( 10, 0),( 15, 0)
 3 2, 0,( 25, 0),( 30, 5)
 4 2, 0,( 45, 0),( 50, 0)
 5 1, 0,( 35, 0)
```

Search suite (expected):
- `45` -> `45 (4,1,true)`
- `36` -> `36 (5,1,false)`
- `34` -> `34 (5,0,false)`
- `20` -> `20 (1,1,true)`
- `30` -> `30 (3,2,true)`

Insertion suite:
- Insert `36` (no split; leaf gains second key):
```
 1 2, 2,( 20, 3),( 40, 4)
 2 2, 0,( 10, 0),( 15, 0)
 3 2, 0,( 25, 0),( 30, 5)
 4 2, 0,( 45, 0),( 50, 0)
 5 2, 0,( 35, 0),( 36, 0)
```
- Insert existing `45`: no structural change.

### Tree 2 (Standard Numbering) — `mvias2.txt` (m = 3)

Structure (displayTree):
```
 1 1, 2,( 30, 3)
 2 1, 4,( 20, 5)
 3 1, 6,( 40, 7)
 4 2, 0,( 10, 0),( 15, 0)
 5 1, 0,( 25, 0)
 6 1, 0,( 35, 0)
 7 2, 0,( 45, 0),( 50, 0)
```

Search suite (expected):
- `30` -> `30 (1,1,true)`
- `55` -> `55 (7,2,false)`
- `32` -> `32 (6,0,false)`
- `25` -> `25 (5,1,true)`
- `40` -> `40 (3,1,true)`

Insertion suite:
- Insert `32` (no split; updates node 6):
```
 1 1, 2,( 30, 3)
 2 1, 4,( 20, 5)
 3 1, 6,( 40, 7)
 4 2, 0,( 10, 0),( 15, 0)
 5 1, 0,( 25, 0)
 6 2, 0,( 32, 0),( 35, 0)
 7 2, 0,( 45, 0),( 50, 0)
```
- Insert `12` (leaf split; parent absorbs without overflow):
```
 1 1, 2,( 30, 3)
 2 2, 4,( 12, 8),( 20, 5)
 3 1, 6,( 40, 7)
 4 1, 0,( 10, 0)
 5 1, 0,( 25, 0)
 6 2, 0,( 32, 0),( 35, 0)
 7 2, 0,( 45, 0),( 50, 0)
 8 1, 0,( 15, 0)
```

### Tree 1 (Alternative Numbering) — `mvias3.txt` (m = 3)

Structure (displayTree):
```
 1 2, 7,( 20, 4),( 40, 3)
 2 1, 0,( 35, 0)
 3 2, 0,( 45, 0),( 50, 0)
 4 2, 0,( 25, 0),( 30, 2)
 5 2, 0,( 10, 0),( 15, 0)
```

Search suite (expected):
- `45` -> `45 (3,1,true)`
- `36` -> `36 (2,1,false)`
- `34` -> `34 (2,0,false)`
- `20` -> `20 (1,1,true)`
- `30` -> `30 (4,2,true)`

Insertion suite:
- Insert `36` (no split; updates node 2):
```
 1 2, 7,( 20, 4),( 40, 3)
 2 2, 0,( 35, 0),( 36, 0)
 3 2, 0,( 45, 0),( 50, 0)
 4 2, 0,( 25, 0),( 30, 2)
 5 2, 0,( 10, 0),( 15, 0)
```

### Tree 2 (Alternative Numbering) — `mvias4.txt` (m = 3)

Structure (displayTree):
```
 1 1, 2,( 30, 3)
 2 1, 4,( 20, 5)
 3 1, 6,( 40, 7)
 4 2, 0,( 10, 0),( 15, 0)
 5 1, 0,( 25, 0)
 6 1, 0,( 35, 0)
 7 2, 0,( 45, 0),( 50, 0)
```

Search suite (expected):
- `30` -> `30 (1,1,true)`
- `55` -> `55 (7,2,false)`
- `32` -> `32 (6,0,false)`
- `25` -> `25 (5,1,true)`
- `40` -> `40 (3,1,true)`

Insertion suite:
- Insert `32`:
```
 1 1, 2,( 30, 3)
 2 1, 4,( 20, 5)
 3 1, 6,( 40, 7)
 4 2, 0,( 10, 0),( 15, 0)
 5 1, 0,( 25, 0)
 6 2, 0,( 32, 0),( 35, 0)
 7 2, 0,( 45, 0),( 50, 0)
```
- Insert `12`:
```
 1 1, 2,( 30, 3)
 2 2, 4,( 12, 8),( 20, 5)
 3 1, 6,( 40, 7)
 4 1, 0,( 10, 0)
 5 1, 0,( 25, 0)
 6 2, 0,( 32, 0),( 35, 0)
 7 2, 0,( 45, 0),( 50, 0)
 8 1, 0,( 15, 0)
```

### Tree 3 (Order 5) — `mvias5.txt` (m = 5)

Structure (displayTree):
```
 1 3, 2,( 20, 3),( 40, 4),( 60, 5)
 2 2, 0,( 10, 0),( 15, 0)
 3 3, 0,( 25, 0),( 30, 0),( 35, 0)
 4 3, 0,( 45, 0),( 50, 0),( 55, 0)
 5 4, 0,( 65, 0),( 70, 0),( 80, 0),( 90, 0)
```

Search suite (expected):
- `55` -> `55 (4,3,true)`
- `75` -> `75 (5,2,false)`
- `20` -> `20 (1,1,true)`
- `33` -> `33 (3,2,false)`
- `100` -> `100 (5,4,false)`

Insertion suite:
- Insert `75` (splits leaf, root does not overflow):
```
 1 4, 2,( 20, 3),( 40, 4),( 60, 5),( 75, 6)
 2 2, 0,( 10, 0),( 15, 0)
 3 3, 0,( 25, 0),( 30, 0),( 35, 0)
 4 3, 0,( 45, 0),( 50, 0),( 55, 0)
 5 2, 0,( 65, 0),( 70, 0)
 6 2, 0,( 80, 0),( 90, 0)
```
- Insert `22` (no split; fills leaf to capacity) — node 3 after insertion:
```
 3 4, 0,( 22, 0),( 25, 0),( 30, 0),( 35, 0)
```

## Notes

- Use the B-Tree order `m` that matches the selected input file. The index header persists `m`; opening with a different `m` is rejected.
- The on-disk node layout is fixed by `MAX_M` to keep record size constant; do not change `MAX_M` unless you understand the binary compatibility impact.

## Validation Checklist

- [ ] Tree displays correctly with 1-based node numbering
- [ ] Search results strictly match `key (node,position,found)`
- [ ] Insertion of a new key updates only the expected nodes
- [ ] Insertion of an existing key does not change the tree
- [ ] All five test suites match the expected outputs above using their respective `m`
- [ ] `data.bin` prints consistently and contains a record for each inserted key
- [ ] I/O counters are shown for search and insertion