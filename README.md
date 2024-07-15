# C-Macro-Processor

## Introduction

System software serves as the intermediary between hardware and application software, playing a crucial role in the efficient functioning of a computer system. It acts as a platform for other software applications and provides essential services such as managing memory, handling input/output operations, and facilitating communication between hardware components. The primary objective of system software is to ensure that the computer hardware operates smoothly and efficiently, providing a stable environment for the execution of various applications.


Macroprocessors play a significant role in enhancing the flexibility and efficiency of programming languages. In the context of the C programming language, a macroprocessor is a tool that performs textual substitution on the source code before the actual compilation process begins. Macros in C are defined using the #define directive and are essentially symbolic names representing a sequence of code.


One of the primary purposes of macroprocessors in C is to create reusable and parameterized code snippets, known as macros, which can be invoked throughout the program. This facilitates code modularity, reduces redundancy, and enhances code readability. Macros can be simple, like constants or inline functions, or more complex, providing a way to customize code generation.


##  Algorithm and Design

### Data Structure

**NAMTAB** : The Name Table (NAMTAB) plays a crucial role in the macro preprocessor, serving as a repository for the names of macros encountered in the source code. The primary purpose is to prevent the inadvertent redefinition of macros. When a #define directive is encountered, the name of the macro is added to the NAMTAB, ensuring that each macro has a unique identity. This prevents conflicts and helps in identifying instances where a macro is defined multiple times, avoiding ambiguity and errors during the preprocessing phase. The NAMTAB is implemented using the ofstream object to facilitate writing to the "NAMTAB.txt" file.

**DEFTAB** : The Definition Table (DEFTAB) serves as a repository for the definitions of macros encountered in the source code. When a #define directive is processed, both the name and definition of the macro are appended to the DEFTAB. This table is crucial for later stages of the preprocessing phase when macro calls are encountered. It allows for efficient retrieval of macro definitions based on their names, enabling the replacement of macro calls with their corresponding definitions in the source code. The DEFTAB is implemented using the ofstream object to write data to the "DEFTAB.txt" file.

**ARGTAB** : The Argument Table (ARGTAB) is responsible for storing information about macros that have parameters, specifically their names and associated arguments. This table is essential for handling macro calls that involve arguments. When a #define directive with arguments is processed, both the name and arguments of the macro are added to the ARGTAB. This information is later used to ensure that macro calls with the correct number of arguments are replaced appropriately during the preprocessing phase. Similar to the NAMTAB and DEFTAB, the ARGTAB is implemented using the ofstream object for writing to the "ARGTAB.txt" file.


### Algorithm

This algorithm processes C macro-processors. It reads an input file, handles directives, and generates an output file accordingly.

### Algorithm

```
begin:
  read line from input file
  if directive="#define":
    check if the name of macro is in NAMTAB
    if yes:
      set ERROR: MACRO REDEFINED.
    else:
      Put the name of macro into NAMTAB
      Put the definition of macro into DEFTAB
      Put the arguments of macro into ARGTAB.
  if directive="#ifdef" or directive="#ifndef":
    check if the macro is defined in case of #ifdef or not defined in case of #ifndef.
    Store this in isSkipping.
    if isSkipping is true:
      Execute the block.
  else if directive="#else" && !isSkipping:
    Execute the block.
  else:
    check if there is macro in the line
    if there is:
      replace the dummy arguments with actual arguments.
      replace the definition.
  put the line into output file.
  read next line
end
```

---



Here is the c++ code for ![C Pre-Processor](https://github.com/PragatiDBhat/C-Macro-Processor/blob/main/cmacroprocessor.cpp)


