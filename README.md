# CodeCave-PyArmor

A tool to dump everything in memory from executables packed with PyArmor.

## Use Cases

- **Reverse Engineering Malicious Programs**: Analyze how PyArmor-packed executables operate internally.
- **String Decryption**: Locate and decrypt obfuscated strings within PyArmor-packed applications.
- **Inspecting PYC Moduli**: See how Pyarmor protects executables by flattening program entries, and junk code injection.
- **Writing YaraX Signatures**: Use recursive memory searches to identify specific patterns in PyArmor-packed applications.
- **Cracking Programs** (with permission): Uncover the original functionality of packed applications.

## About

CodeCave-PyArmor patches into any PyArmor-packed application, regardless of the PyArmor version. Although PyArmor primarily serves as an obfuscator rather than a packer, continuous memory dumping with this tool can yield a nearly identical recreation of the original Python program.

With sufficient time and attention, you can thoroughly analyze and reconstruct PyArmor-packed executables, making this tool the best open source tool that can decrypt pyarmor's techniques.
