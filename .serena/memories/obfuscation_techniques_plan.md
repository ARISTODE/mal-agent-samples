# Obfuscation and Encryption Techniques Plan

## Obfuscated Sample Planning

### Sample 4: String-Encrypted Utility
**Base Program**: Simple file utility (custom C++ program)
**Techniques**:
- XOR encryption for all string literals
- Runtime string decryption
- String stack construction
- Format string obfuscation

**Implementation**:
```cpp
// Example: String encryption with XOR
const char encrypted[] = {0x48^0x5A, 0x65^0x5A, 0x6C^0x5A, ...}; // "Hello"
char* decrypt(const char* enc, int len, char key);
```

### Sample 5: Control-Flow Obfuscated Calculator
**Base Program**: Mathematical expression evaluator
**Techniques**:
- Control flow flattening
- Opaque predicates
- Dead code insertion
- Function inlining/outlining
- Jump table obfuscation

**Implementation Ideas**:
- Use switch-based state machines
- Insert junk calculations that always evaluate to known values
- Split functions into multiple parts with indirect calls

### Sample 6: Packed Network Monitor
**Base Program**: Simple network statistics tool
**Techniques**:
- PE packing (UPX or custom packer)
- Import table obfuscation
- Anti-debugging tricks
- Code virtualization (simple VM)

**Implementation Ideas**:
- Dynamic API resolution
- Checksum verification
- Timing checks
- Debugger detection

## Obfuscation Tools and Libraries

### Manual Techniques
1. **String Encryption**
   - XOR cipher
   - RC4 encryption
   - Base64 encoding with custom alphabet

2. **Control Flow**
   - Goto-based spaghetti code
   - Function pointer arrays
   - Computed gotos

3. **Anti-Analysis**
   - IsDebuggerPresent checks
   - Timing attacks
   - Exception-based control flow

### Automated Tools (Optional)
- **Tigress**: C obfuscator (academic tool)
- **OLLVM**: Obfuscator-LLVM
- **ConfuserEx**: .NET obfuscator (for comparison)
- **UPX**: Executable packer

## Implementation Strategy

### For Each Obfuscated Sample:
1. Start with clean, simple C/C++ code
2. Apply obfuscation techniques incrementally
3. Maintain both obfuscated and clean versions
4. Document transformation process
5. Ensure functionality is preserved
6. Generate complexity metrics

### Code Complexity Targets
- **String Encrypted**: 2-3x string complexity
- **Control Flow**: 5-10x cyclomatic complexity increase
- **Packed**: 50-80% size reduction, then expansion at runtime

## Example Obfuscation Patterns

### Pattern 1: String Encryption
```cpp
// Original
printf("Hello World");

// Obfuscated
char* s = decrypt("\x1F\x2A\x3B\x3B\x38", 5, 0x5A);
printf("%s", s);
free(s);
```

### Pattern 2: Opaque Predicates
```cpp
// Original
if (x > 10) { doSomething(); }

// Obfuscated
int y = (x * x) % 7;  // Always produces 0, 1, 2, or 4
if ((y * y - 3 * y + 2) >= 0 && x > 10) {  // First part always true
    doSomething();
}
```

### Pattern 3: Control Flow Flattening
```cpp
// Original
funcA(); funcB(); funcC();

// Obfuscated
int state = 0;
while(1) {
    switch(state) {
        case 0: funcA(); state = 2; break;
        case 1: funcC(); return; 
        case 2: funcB(); state = 1; break;
    }
}
```

## Success Criteria
1. All obfuscated samples must remain functional
2. Static analysis tools should have difficulty analyzing them
3. Decompilers should produce less readable output
4. Measurable increase in analysis complexity
5. Representative of real-world obfuscation techniques