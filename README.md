# Frequent-pattern-algorithm Implementation

Languge used: C++

Goal: Implement a frequent pattern mining approach that is efficient and scalable for finding frequent patterns of the transaction datasets. 

- ## Node Class
The Node class represents a node in the FP-Tree. It includes attributes for the item, count, parent node, child nodes, and a next node used for linking nodes with the same item.

- ## Main Functions
`init()`: Reads transaction data from the input file and initializes necessary variables.

`constructTree()`: Constructs the FP-Tree and header table from the transaction data.

`mineTree()`: Mines the FP-Tree for frequent patterns.

`conditionalPatternBase()` : Constructs the conditional pattern base for a given item.

`output()`: Writes the frequent patterns and their support to the output file.


- ## Compilation

To compile the project, navigate to the project directory and use the following command:

```bash
g++ -std=c++17 main.cpp
./a.out 0.2 sample.txt output.txt (./a.out <minSupport> <inputFile> <outputFile>)
