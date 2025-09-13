# AED II - Trabalho 1 - Parte 1

M-way Search Tree Implementation for Disk-Based Operations

## Authors
- [Author 1 Name] - [USP Number]
- [Author 2 Name] - [USP Number]

## Description

Implementation of m-way search trees (order 3) stored on disk with search functionality using the mSearch algorithm.

## Features

- Read tree structure from text file and store in binary format
- Disk-based m-way search tree operations
- mSearch algorithm implementation
- Interactive search interface

## Build and Run

```bash
mkdir build
cd build
cmake ..
make
./MWaySearch
```

## Usage

1. Prepare your `mvias.txt` file with the tree structure
2. Run the program - it will create `mvias.bin` automatically
3. Enter search keys when prompted
4. The program will show if the key was found and its location

## Testing Guide

### Test 1: Default Tree Structure
The default `mvias.txt` contains the tree from the assignment example:

```
Tree Structure:
        [20, 40]
       /    |    \
   [10,15] [25,30] [45,50]
              |
            [35]
```

**Expected Results:**
- Search `45`: Should return `(4,1,true)` - found in node 4, position 1
- Search `36`: Should return `(5,1,false)` - not found, can be inserted in node 5, position 1
- Search `34`: Should return `(5,0,false)` - not found, can be inserted in node 5, position 0
- Search `20`: Should return `(1,1,true)` - found in root node, position 1
- Search `30`: Should return `(3,2,true)` - found in node 3, position 2

### Test 2: Alternative Tree Structure
To test with the second tree structure, replace `mvias.txt` with `test_mvias.txt`:

```
Tree Structure:
      [20, 30, 40]
     /    |   |    \
  [10,15] [25] [35] [45,50]
```

**Expected Results:**
- Search `30`: Should return `(1,2,true)` - found in root, position 2
- Search `25`: Should return `(2,1,true)` - found in node 2, position 1
- Search `32`: Should return `(3,1,false)` - not found, can be inserted in node 3, position 1

### Validation Checklist
- [ ] Tree displays correctly with proper node numbering (1-based)
- [ ] Search results follow format: `key (node,position,found)`
- [ ] Found keys return `true` with correct node and 1-based position
- [ ] Missing keys return `false` with insertion location
- [ ] Program handles multiple searches without reloading tree

## File Format

The `mvias.txt` should contain nodes in the format:
```
n A0 K1 A1 K2 A2 ... Kn An
```

Where:
- n: number of keys in the node
- A0: left child pointer
- Ki: key value
- Ai: right child pointer for key Ki