# CppStateMachineFramework

The purpose of this framework is to simplify creation of state machines for C++ applications.

The features and terminology is based on the
[UML state machine](https://en.wikipedia.org/wiki/UML_state_machine). This framework does not
support all of those features and it does some things a bit differently. For example while
hierarchical state machines are not directly supported a similar functionality could be achieved by
adding a state that is configured to operate another state machine.


## Requirements

Requirements document can be found [here](docs/Requirements.md).


## Design

Design document can be found [here](docs/Design.md).


## Build and deployment

For building the following requirements are needed:

* CMake v3.5 or later
* C++14 compliant compiler
* Qt v5.9 or later (only Core and Test modules)

If the library needs to be deployed to a custom location you need to set the
```CMAKE_INSTALL_PREFIX``` variable or if you need to use a custom Qt installation you need to set
the ```CMAKE_PREFIX_PATH``` variable.

Example:

```
$ cmake -DCMAKE_INSTALL_PREFIX=path/to/install/dir -DCMAKE_PREFIX_PATH=path/to/qt/dir path/to/source/dir
```

Then build and deploy the library:

```
$ cmake --build . --target install
```


## Usage

### CMake Integration

To use this module in a CMake project you need to build and deploy it. Then just add it as a package
to your project's CMakeLists.txt:

```CMake
find_package(CedarFramework REQUIRED)
```

And link it to your target:

```CMake
target_link_libraries(target_name PUBLIC CedarFramework::CedarFramework)
```

TODO: code and usage examples
