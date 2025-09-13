# Sample Verification Requirements

## Critical Testing Protocol
For each C sample implementation:

1. **Compile the sample** - Create stripped PE binary with MinGW
2. **Test execution** - Run the binary to verify it works correctly
3. **Provide payloads** - If program requires input/arguments, provide test payloads
4. **Full verification** - Ensure PE works completely before moving to next sample
5. **Document results** - Note any special requirements or behaviors

## Current Compiler Path
`../../../mingw64/mingw64/bin/gcc.exe`

## Build Flags
`-O2 -Wall -Wextra -s` (stripped symbols for analysis)

This verification step is SUPER IMPORTANT - no sample should be considered complete until the PE binary is fully tested and working.