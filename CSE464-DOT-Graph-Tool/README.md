# DOT Graph Tool

## Overview
This project is a Java-based graph utility for working with directed graphs written in DOT format. It can parse an input DOT file, inspect the graph, add nodes and edges, export the graph back to DOT, and generate a PNG visualization using Graphviz.

The project was built for CSE 464 and organized as a Maven project with JUnit tests and a sample DOT input.

## Features
- Parse a DOT graph file into an in-memory directed graph
- Print graph details including:
  - number of nodes
  - sorted node labels
  - number of edges
  - edge directions
- Add single nodes with duplicate checking
- Add multiple nodes while ignoring duplicates
- Add directed edges with duplicate checking
- Export the graph to a DOT file
- Export the graph to a PNG image using Graphviz

## Tech Stack
- Java 11
- Maven
- JGraphT
- JUnit 5
- Graphviz

## Project Structure

CSE464-DOT-Graph-Tool/
├── src/main/java/edu/asu/cse464/
│   ├── AppMain.java
│   └── DotGraphTool.java
├── src/test/java/edu/asu/cse464/
│   ├── DotGraphToolFeature2Test.java
│   ├── DotGraphToolFeature3Test.java
│   └── DotGraphToolFeature4Test.java
├── examples/
│   └── sample.dot
├── pom.xml
└── ReadME.pdf
