# AED II - Trabalho 1 - Parte 1

M-way Search Tree Implementation for Disk-Based Operations

## Authors
- Francisco Eduardo Fontenele - 15452569
- Vinicius Botte - 15522900

## Description

Implementation of m-way search trees (order 3) stored on disk with search functionality using the mSearch algorithm. The program supports testing with multiple tree configurations as required by the assignment.

## Features

- Read tree structure from text file and store in binary format
- Disk-based m-way search tree operations
- mSearch algorithm implementation
- Interactive search interface
- Support for multiple tree configurations

## Build and Run

```bash
mkdir build
cd build
cmake ..
make
./MWaySearch
```

## File Structure

```
MWaySearch/
├── CMakeLists.txt
├── main.cpp
├── MWayTree.h
├── MWayTree.cpp
├── mvias.txt          # Tree 1 with standard numbering
├── mvias2.txt         # Tree 2 with standard numbering
├── mvias3.txt         # Tree 1 with alternative numbering
└── mvias4.txt         # Tree 2 with alternative numbering
```

## Testing Guide

### Test Files Description

1. **mvias.txt**: Tree 1 with standard numbering (a=1, b=2, c=3, d=4, e=5)
2. **mvias2.txt**: Tree 2 with standard numbering (a=1, b=2, c=3, d=4, e=5, f=6, g=7)
3. **mvias3.txt**: Tree 1 with alternative numbering (a=5, b=7, c=4, d=3, e=2)
4. **mvias4.txt**: Tree 2 with alternative numbering (a=5, b=7, c=4, d=3, e=2, f=6, g=1)

### Tree Structures

#### Tree 1 (Standard Numbering)
```
        [20, 40]
       /    |    \
   [10,15] [25,30] [45,50]
              |
            [35]
```

**Expected Results for Tree 1 Standard:**
- Search `45`: Should return `(4,1,true)` - found in node 4, position 1
- Search `36`: Should return `(5,1,false)` - not found, can be inserted in node 5, position 1
- Search `34`: Should return `(5,0,false)` - not found, can be inserted in node 5, position 0
- Search `20`: Should return `(1,1,true)` - found in root node, position 1
- Search `30`: Should return `(3,2,true)` - found in node 3, position 2

#### Tree 2 (Standard Numbering)
```
      30
    /    \
  20      40
 /  \    /  \
10,15 25 35  45,50
```

**Expected Results for Tree 2 Standard:**
- Search `30`: Should return `(1,1,true)` - found in root, position 1
- Search `55`: Should return `(7,2,false)` - not found, can be inserted in node 7, position 2
- Search `32`: Should return `(6,0,false)` - not found, can be inserted in node 6, position 0
- Search `25`: Should return `(5,1,true)` - found in node 5, position 1
- Search `40`: Should return `(3,1,true)` - found in node 3, position 1

#### Tree 1 (Alternative Numbering)
```
        [20,40] (node1)
       /    |    \
   [10,15] (node5) [25,30] (node4) [45,50] (node3)
                         |
                       [35] (node2)
```

**Expected Results for Tree 1 Alternative:**
- Search `45`: Should return `(3,1,true)` - found in node 3, position 1
- Search `36`: Should return `(2,1,false)` - not found, can be inserted in node 2, position 1
- Search `34`: Should return `(2,0,false)` - not found, can be inserted in node 2, position 0
- Search `20`: Should return `(1,1,true)` - found in root node, position 1
- Search `30`: Should return `(4,2,true)` - found in node 4, position 2

#### Tree 2 (Alternative Numbering)
```
      30 (node1)
    /    \
  20 (node2)      40 (node3)
 /  \    /  \
10,15 (node4) 25 (node5) 35 (node6)  45,50 (node7)
```

**Expected Results for Tree 2 Alternative:**
- Search `30`: Should return `(1,1,true)` - found in root, position 1
- Search `55`: Should return `(7,2,false)` - not found, can be inserted in node 7, position 2
- Search `32`: Should return `(6,0,false)` - not found, can be inserted in node 6, position 0
- Search `25`: Should return `(5,1,true)` - found in node 5, position 1
- Search `40`: Should return `(3,1,true)` - found in node 3, position 1

### Validation Checklist

- [ ] Tree displays correctly with proper node numbering (1-based)
- [ ] Search results follow format: `key (node,position,found)`
- [ ] Found keys return `true` with correct node and 1-based position
- [ ] Missing keys return `false` with correct insertion location
- [ ] Program handles multiple searches without reloading tree
- [ ] All four test cases work correctly with both standard and alternative numbering

## File Format

The text files should contain nodes in the format:
```
n A0 K1 A1 K2 A2 ... Kn An
```

Where:
- n: number of keys in the node
- A0: left child pointer
- Ki: key value
- Ai: right child pointer for key Ki

## Algorithm Explanation

The mSearch algorithm works by:
1. Starting at the root node
2. Comparing the search key with keys in the current node
3. Following the appropriate child pointer based on key comparisons
4. Returning the node and position where the key is found or should be inserted

The algorithm works correctly with both standard and alternative numbering because it only depends on the relative ordering of keys and the child pointers, not on the absolute values of node identifiers.

## Answer to Assignment Question (Item D)

The mSearch algorithm works in both cases (standard and alternative numbering) because it only uses the child pointers and keys stored in each node. The node numbering does not affect the search logic, as long as the child pointers are consistent with the numbering used in the file. This is expected because m-way tree search depends only on the order of keys and pointers, not on the absolute identifiers of the nodes.