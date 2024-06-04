# HashTable

This C program implements a hash table using two different collision resolution methods: Quadratic Probing and Double Hashing. The program allows users to insert, search, delete, and compare the performance of the two collision resolution methods.

## Overview

The program includes the following components:

- Definitions of constant values and data structures.
- Initialization functions to create and initialize the hash tables.
- Hashing functions for calculating the hash index of a key.
- Collision resolution methods (Quadratic Probing and Double Hashing).
- Functions for inserting, searching, and deleting elements from the hash tables.
- Input/output functions for reading from files and printing to the console.
- Functions for handling collisions and resizing the hash tables.

## Usage

1. Compile the program using a C compiler.
2. Run the executable.
3. Choose from the menu options to perform various operations on the hash tables.

## Functionality

1. **Print Hashed Tables**: Display the contents of both Quadratic Probing and Double Hashing tables.
2. **Print Table Size and Load Factor**: Show the size and load factor of the selected hash table.
3. **Print Hash Function**: Display the hash function used by each hashing method.
4. **Insert New Record**: Add a new word and its meaning to both hash tables.
5. **Search for a Word**: Search for a specific word in either hash table.
6. **Delete a Word**: Remove a specific word from both hash tables.
7. **Compare Collisions**: Compare the number of collisions between Quadratic Probing and Double Hashing.
8. **Save Hash Table to File**: Save the contents of the Double Hashing table to a file.
9. **Exit**: Terminate the program.

## Files

- `main.c`: Contains the main C code for the hash table implementation.

## Notes

- Words and meanings are read from a file named `words.txt`.
- The program dynamically resizes the hash table when the load factor exceeds a threshold.
- Collision resolution methods are used to handle collisions during insertion.
- Both Quadratic Probing and Double Hashing methods are implemented for comparison purposes.
