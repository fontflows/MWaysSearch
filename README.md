# AED II - Trabalho 1 - Parte 2

B-Tree (order m) implementation with disk-resident index, search (mSearch), and insertion (insertB).

## Authors
- Francisco Eduardo Fontenele - 15452569
- Vinicius Botte - 15522900

## Description

Disk-based B-Tree that indexes an integer key field. The tree order `m` is chosen by the user at runtime. The binary on-disk node layout is fixed by `MAX_M` to keep records aligned, while the effective order is `m` (3 ≤ m ≤ MAX_M). The program supports interactive search and insertion, plus loading the initial index from a text file.

## Features

- Parametric B-Tree order `m` selected at startup
- Fixed on-disk node layout (`MAX_M`) for stable binary format
- Create binary index from text file (`createFromText`)
- Disk-based search (`mSearch`) returning `(node, position, found)`
    - `position` is 1-based only when `found = true` (hit), otherwise 0..n as insertion slot
- Disk-based insertion (`insertB`) with node split and median promotion
- Root creation when the old root splits
- Tree display in the required tabular format
- Interactive menu: display, search, insert, exit

## Requirements

- C\+\+17 or newer
- CMake
- Tested on Windows (CLion 2024.2.3)

## Build and Run

```bash
mkdir build
cd build
cmake ..
cmake --build .
./MWaySearch
```

Alternatively, open the project in CLion and run the `MWaySearch` target.

## Usage

1. Select the tree input file to load.
2. Enter the B-Tree order `m` according to the selected file:
    - For `mvias.txt`, `mvias2.txt`, `mvias3.txt`, `mvias4.txt` use `m = 3`.
    - For `mvias5.txt` use `m = 5`.
3. The program creates `mvias.bin` from the chosen text file, opens it, and shows the index.
4. Use the menu to:
    - Search a key: prints `key (node,position,found)`
    - Insert a key: updates the on-disk index and shows the new structure on next display
    - Exit

## File Structure

```
MWaySearch/
├── CMakeLists.txt
├── main.cpp
├── MWayTree.h
├── MWayTree.cpp
├── mvias.txt
├── mvias2.txt
├── mvias3.txt
├── mvias4.txt
└── mvias5.txt
```

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

The binary file `mvias.bin` stores a dummy header at position 0 and nodes from position 1 onward (1-based node numbering in displays).

## Algorithm Notes

- `mSearch` traverses from root, comparing keys and following child pointers until it finds the key or the leaf where it should be inserted.
- `insertB` inserts bottom-up, splitting full nodes, promoting the median, and creating a new root if required.
- On root creation, nodes are shifted on disk to keep the root at position 1; child pointers are adjusted accordingly.

## Testing Guide

Important:
- Use `m = 3` for `mvias.txt`, `mvias2.txt`, `mvias3.txt`, `mvias4.txt`.
- Use `m = 5` for `mvias5.txt`.
- Compare the exact output line `key (node,position,found)` for search tests.
- For insertion tests, re-display the tree and compare the specified node lines.

### Test Files Description

1. `mvias.txt`: Tree 1 with standard numbering (a=1, b=2, c=3, d=4, e=5)
2. `mvias2.txt`: Tree 2 with standard numbering (a=1, b=2, c=3, d=4, e=5, f=6, g=7)
3. `mvias3.txt`: Tree 1 with alternative numbering (a=5, b=7, c=4, d=3, e=2)
4. `mvias4.txt`: Tree 2 with alternative numbering (alternative mapping)
5. `mvias5.txt`: Tree 3 for `m = 5` (broader node capacity)

### Tree 1 (Standard Numbering) — `mvias.txt` (m = 3)

Structure:
```
        [20, 40]
       /    |    \
   [10,15] [25,30] [45,50]
              |
            [35]
```

Search suite (expected):
- `45` -> `45 (4,1,true)`
- `36` -> `36 (5,1,false)`
- `34` -> `34 (5,0,false)`
- `20` -> `20 (1,1,true)`
- `30` -> `30 (3,2,true)`

Insertion suite:
- Insert `36`:
    - Node 5 after insertion:
        - `5 2, 0,( 35, 0)( 36, 0)`
- Insert existing `45`: no structural change.

### Tree 2 (Standard Numbering) — `mvias2.txt` (m = 3)

Structure:
```
      [30]
     /    \
  [20]    [40]
  /  \    /   \
[10,15][25][35][45,50]
```

Search suite (expected):
- `30` -> `30 (1,1,true)`
- `55` -> `55 (7,2,false)`
- `32` -> `32 (6,0,false)`
- `25` -> `25 (5,1,true)`
- `40` -> `40 (3,1,true)`

Insertion suite:
- Insert `32`:
    - Node 6 after insertion:
        - `6 2, 0,( 32, 0)( 35, 0)`
- Insert `12` (leaf split, parent absorbs without overflow):
    - After:
        - `4 1, 0,( 10, 0)`
        - `8 1, 0,( 15, 0)`
        - `2 2, 4,( 12, 8)( 20, 5)`

### Tree 1 (Alternative Numbering) — `mvias3.txt` (m = 3)

Search suite (expected):
- `45` -> `45 (3,1,true)`
- `36` -> `36 (2,1,false)`
- `34` -> `34 (2,0,false)`
- `20` -> `20 (1,1,true)`
- `30` -> `30 (4,2,true)`

Insertion suite:
- Insert `36`:
    - `2 2, 0,( 35, 0)( 36, 0)`

### Tree 2 (Alternative Numbering) — `mvias4.txt` (m = 3)

Search suite (expected):
- `30` -> `30 (1,1,true)`
- `55` -> `55 (7,2,false)`
- `32` -> `32 (6,0,false)`
- `25` -> `25 (5,1,true)`
- `40` -> `40 (3,1,true)`

Insertion suite:
- Insert `32`:
    - `6 2, 0,( 32, 0)( 35, 0)`
- Insert `12`:
    - `4 1, 0,( 10, 0)`
    - `8 1, 0,( 15, 0)`
    - `2 2, 4,( 12, 8)( 20, 5)`

### Tree 3 (Order 5) — `mvias5.txt` (m = 5)

Structure:
```
              [20, 40, 60]
             /     |     |     \
        [10,15] [25,30,35] [45,50,55] [65,70,80,90]
```

Search suite (expected):
- `55` -> `55 (4,3,true)`
- `75` -> `75 (5,2,false)`
- `20` -> `20 (1,1,true)`
- `33` -> `33 (3,2,false)`
- `100` -> `100 (5,4,false)`

Insertion suite:
- Insert `75` (splits leaf, root does not overflow):
    - Expected display lines after insertion:
        - ` 1 4, 2,( 20, 3)( 40, 4)( 60, 5)( 75, 6)`
        - ` 2 2, 0,( 10, 0)( 15, 0)`
        - ` 3 3, 0,( 25, 0)( 30, 0)( 35, 0)`
        - ` 4 3, 0,( 45, 0)( 50, 0)( 55, 0)`
        - ` 5 2, 0,( 65, 0)( 70, 0)`
        - ` 6 2, 0,( 80, 0)( 90, 0)`
- Insert `22` (no split; fills leaf to capacity):
    - Node 3 after insertion:
        - `3 4, 0,( 22, 0)( 25, 0)( 30, 0)( 35, 0)`

## Notes

- Use the order `m` that matches the selected input file.
- The on-disk node layout is fixed by `MAX_M` to keep record size constant; do not change `MAX_M` unless you understand the binary compatibility impact.

## Validation Checklist

- [ ] Tree displays correctly with 1-based node numbering
- [ ] Search results strictly match `key (node,position,found)`
- [ ] Insertion of a new key updates only the expected nodes
- [ ] Insertion of an existing key does not change the tree
- [ ] All five test suites match the expected outputs above using their respective `m`
```