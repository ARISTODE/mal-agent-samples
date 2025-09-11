# Control-Flow Obfuscated Calculator Sample

## Overview
Mathematical expression calculator with control-flow obfuscation
- **Base Program**: Custom expression parser and calculator
- **Architecture**: x64  
- **Compiler**: MinGW-w64 GCC 14.2.0

## Executable Details
- **Clean Version**: `compiled/calc_clean.exe` (~2.8 MB)
- **Obfuscated Version**: `compiled/calc_obfuscated.exe` (~2.9 MB)
- **Type**: Windows PE console executable
- **Static linking**: Yes

## Build Information
- **Build Date**: 2025-09-10
- **Build System**: MinGW-w64 with g++
- **Source**: Custom C++ implementation
- **Configuration**: Release build with optimizations (-O2)

## Obfuscation Techniques
### 1. Control Flow Flattening
- Linear code converted to switch-based state machines
- Natural execution flow replaced with artificial states
- Jump tables and state variables control program flow

### 2. Opaque Predicates
- `OPAQUE_TRUE(x)`: Always evaluates to true but appears complex
- `OPAQUE_FALSE(x)`: Always evaluates to false but appears complex
- Used to obscure branch conditions and confuse static analysis

### 3. Dead Code Insertion
- Unreachable code blocks inserted throughout functions
- Junk variables and calculations that don't affect results
- Misleading labels and goto statements

### 4. State Machine Obfuscation
- Parser functions converted to complex state machines
- Multiple entry/exit points with goto statements
- Artificial control variables and flow redirection

### 5. Junk Code Generation
- `JUNK_VAR` macro inserts volatile random variables
- Fake computations that appear meaningful but are optimized away
- Random number generation for state obfuscation

## Key Features
- Basic arithmetic operations (+, -, *, /)
- Parentheses support for precedence
- Interactive mode and command-line mode
- Expression parsing with proper operator precedence

## Usage Example
```cmd
# Command line mode
calc_obfuscated.exe "2 + 3 * 4"
calc_obfuscated.exe "(10 - 2) / 4"

# Interactive mode (clean version only - obfuscated has input issues)
calc_clean.exe
> 2 + 3 * 4
Result: 14
> quit
```

## Analysis Characteristics
### Clean Version
- Straightforward recursive descent parser
- Clear function call hierarchy
- Linear control flow
- Easy to analyze and understand

### Obfuscated Version
- **Control Flow Flattening**: Natural call structure replaced with state machines
- **Complex Branching**: Multiple switch statements with artificial states
- **Dead Code**: Unreachable paths and junk calculations
- **Opaque Predicates**: Conditions that always evaluate the same way
- **Anti-Disassembly**: Confusing control flow for reverse engineers
- **CFG Complexity**: Dramatically increased cyclomatic complexity

## Code Structure Comparison
```
Clean Version:
- Simple recursive functions
- Natural if/else branching
- Clear variable names
- Linear execution flow

Obfuscated Version:
- State machine implementations
- goto statements and labels
- Volatile variables
- Complex switch statements
- Junk code insertions
- Artificial state variables
```

## Obfuscation Effectiveness
- **Static Analysis**: Significantly more complex control flow graph
- **Decompilation**: Produces much less readable output
- **Manual Analysis**: Requires more time to understand program logic
- **Debugging**: Control flow is harder to follow
- **Performance**: Minimal overhead due to compiler optimizations

## Security Analysis Notes
- Demonstrates intermediate-level control flow obfuscation
- Opaque predicates can be detected with symbolic execution
- State machines make manual analysis significantly harder
- Dead code increases binary size and analysis complexity
- Effective against basic static analysis tools