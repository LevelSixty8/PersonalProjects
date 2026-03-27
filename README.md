# CSE340 Parser Project

##  Overview
This project implements a recursive-descent parser for a custom polynomial-based programming language. The system processes input programmes, performs lexical and syntactic analysis, and detects semantic errors.

The parser is designed as part of a compiler front-end, simulating how real programming languages are interpreted and validated.

---

##  Features

###  Lexical Analysis
- Tokenizes input using a custom lexer
- Identifies keywords, identifiers, operators, and literals

###  Parsing (Recursive Descent)
- Implements grammar rules using recursive functions
- Validates syntax structure of input programs
- Handles expressions, statements, and function-like constructs

###  Semantic Analysis
- Detects undeclared variable usage
- Tracks parameter scope within polynomial definitions
- Reports structured error codes

###  Testing Framework
- Includes extensive test cases:
  - Lexical analysis tests
  - Syntax error tests
  - Semantic error tests
- Expected outputs provided for validation

---

##  Concepts Demonstrated
- Recursive-descent parsing
- Context-free grammars
- Compiler front-end design
- Symbol tracking and scope handling
- Error detection and reporting systems

---

##  Technologies Used
- **Language:** C++
- **Environment:** Linux / Unix-based systems
- **Tools:** g++, shell scripts for testing

---

## 📂 Project Structure
