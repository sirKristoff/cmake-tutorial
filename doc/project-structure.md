# Project structure

## Structure template:

```bash
proj
├── CMakeLists.txt  # CMake Policies
|                   # Project Settings
|                   # Global Variables
|                   # Global Includes
|                   # Global Dependencies
├── cmake
│   ├── include
│   │   ├── function.cmake
│   │   └── macro.cmake
│   ├── module
│   │   └── FindPQXX.cmake
│   └── script  # one-off scripts
│       └── script.cmake
├── doc  # Used for building the documentation
|   └── CMakeLists.txt  # Add documentation target
├── extern  # Configuration for the external projects build from source
│   └── .keep  # For future extension
|           # Download external project
|           # Clone git repository
├── src
│   ├── CMakeLists.txt
│   ├── appfrogger  # executable
│   │   ├── CMakeLists.txt
│   │   ├── libgui  # library specific to 'appfrogger' only
│   │   │   ├── CMakeLists.txt
│   │   │   ├── entry_point.cpp
│   │   │   └── include
│   │   │       └── libgui
│   │   │           └── interface.hpp
│   │   └── main.cpp
│   └── libutils  # library used in the project or exported outside
│       ├── CMakeLists.txt
│       ├── drawing.cpp
│       ├── include
│       │   ├── drawing.hpp
│       │   └── libutils
│       │       └── utils.hpp  # public header file for export
│       └── utils.cpp
└── test
    ├── CMakeLists.txt  # Configure CTest
    |                   # Add test targets
    ├── appfrogger
    │   ├── CMakeLists.txt
    │   └── appfrogger_test.cpp
    └── libutils
        ├── CMakeLists.txt
        ├── drawing_test.cpp
        └── utils_test.cpp
```