# Table of contents

1. [Introduction](#introduction)
   * [Motivation](#motivation)
   * [Code Example](#example)
2. [Containers](#containers)
3. [MathLink support](#mathlink)
    * [Main features](#mathlink-features)
    * [Example](#mathlink-example)
4. [Error handling](#error-handling)
5. [Limitations](#limitations)
6. [How to Use](#howToUse)
    * [Prerequisites](#howToUse-prerequisites)
    * [Step-by-Step](#howToUse-stepbystep)
    * [Note for I/E developers](#howToUse-note)
7. [API reference](#APIreference)
8. [Contributors](#contributors)

<a name="introduction"></a>
# Introduction

_LibraryLink Utilities_ (abbr. LLU) is a set of modern C++ wrappers for most elements of standard _LibraryLink_ C interface. 
Containers like MImage or MTensor are wrapped in templated classes. Managing MArguments (both input and output) is also 
delegated to a separate class.

For more details about each class see [the documentation](http://algorithms.wolfram.com:8080/documentation/LibraryLinkUtilities).

__Please send all suggestions, feature requests and bug reports to <rafalc@wolfram.com>__

<a name="motivation"></a>
## Motivation

_LibraryLink_ is a great tool for connecting Wolfram Language with external libraries and programs written in C and it is widely used internally for developing paclets.
But as more and more paclets are now being developed in modern C++ the integration with _LibraryLink_, although still possible, becomes cumbersome and inelegant. 
Most significant features missing in _LibraryLink_ are:

* Automatic resource management
* Exception handling
* Container iterators
* Class-like interface for _LibraryLink_ data structures, for example `rank()` as member function of Tensor class instead of separate function 
`mint (*MTensor_getRank)(MTensor)`, or a copy constructor instead of `int (*MTensor_clone)(MTensor, MTensor*)`
* Type safety

The motivation behind _LibraryLink Utilities_ is to provide the aforementioned features without touching _LibraryLink_ sources.

<a name="example"></a>
## Code Example

Probably the best way to see how to use LLU and what advantages it has over classic _LibraryLink_ is by comparing the same function written in two different styles. 
Below we will implement a simple function `repeatCharacters` that takes a string `s` and a tensor `t` and returns a new string `s2` that consists of each 
character `s[i]` from original string but repeated `t[i]` times, so for example

	repeatCharacters("abc", {3, 2, 1})
gives 

	"aaabbc"  

C - style implementation:
```c

// global variable which is the buffer for strings returned to LibraryLink
char* outString = NULL;

EXTERN_C DLLEXPORT int repeatCharacters(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
    char* string = NULL;
    MNumericArray counts;
    uint8_t* countsData = NULL;
    size_t outStringIndex = 0;
    size_t len, j;
    mint sum = 0;
    mint c;
    
    string = MArgument_getUTF8String(Args[0]);
    counts = MArgument_getMNumericArray(Args[1]);

    // check NumericArray type
    if (libData->numericarrayLibraryFunctions->MNumericArray_getType(counts) != MNumericArray_Type_UBit8) {
        libData->UTF8String_disown(string);
        return LIBRARY_TYPE_ERROR;
    }

    // check NumericArray rank
    if (libData->numericarrayLibraryFunctions->MNumericArray_getRank(counts) != 1) {
        libData->UTF8String_disown(string);
        return LIBRARY_RANK_ERROR;
    }

    // check if NumericArray length is equal to input string length
    len = strlen(string);
    if (libData->numericarrayLibraryFunctions->MNumericArray_getFlattenedLength(counts) != len) {
        libData->UTF8String_disown(string);
        return LIBRARY_DIMENSION_ERROR;
    }

    // before we allocate memory for the output string, we have to sum all NumericArray elements to see how many bytes are needed
    countsData = (uint8_t*) libData->numericarrayLibraryFunctions->MNumericArray_getData(counts);
    for (j = 0; j < len; j++) {
        sum += countsData[j];
    }

    // free memory owned by global buffer
    free(outString);
    outString = NULL;

    // allocate memory for output string, outString has to be a global variable, because it will be returned to LibraryLink
    outString = (char*) malloc(sum + 1);
    if (!outString) {
        libData->UTF8String_disown(string);
        return LIBRARY_FUNCTION_ERROR;
    }

    // populate output string
    for (j = 0; j < len; j++) {
        for (c = 0; c < countsData[j]; c++) {
            outString[outStringIndex++] = string[j];
        }
    }

    // add null terminator
    outString[sum] = '\0';

    // clean up and set result
    libData->UTF8String_disown(string);
    MArgument_setUTF8String(Res, outString);

    return LIBRARY_NO_ERROR;
}
```

and C++ version with _LibraryLink Utilities_:

```cpp

EXTERN_C DLLEXPORT int repeatCharactersNew(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
    auto err = ErrorCode::NoError;
    try {
        // Create manager object
        MArgumentManager mngr(libData, Argc, Args, Res);

        // Read string and NumericArray arguments
        auto string = mngr.getString(0);
        auto counts = mngr.getNumericArray<std::uint8_t>(1);

        // check NumericArray rank
        if (counts.rank() != 1) {
            ErrorManager::throwException(ErrorName::RankError);
        }

        // check if NumericArray length is equal to input string length
        if (counts.size() != string.size()) {
            ErrorManager::throwException(ErrorName::DimensionsError);
        }

        // before we allocate memory for the output string, we have to sum all NumericArray elements to see how many bytes are needed
        auto sum = std::accumulate(counts.begin(), counts.end(), static_cast<size_t>(0));

        // allocate memory for output string
        std::string outString;
        outString.reserve(sum);

        // populate output string
        for (mint i = 0; i < counts.size(); i++) {
            outString.append(std::string(counts[i], string[i]));
        }

        // clean up and set result
        mngr.set(std::move(outString));
    }
    catch (const LibraryLinkError& e) {
        err = e.id();
    }
    catch (const std::exception&) {
        err = ErrorCode::FunctionError;
    }
    return err;
}
```
<a name="containers"></a>
# Containers

Raw _LibraryLink_ containers like MTensor or MNumericArray store their element type as a regular field in the stucture 
meaning that the type cannot be used in compile-time context, which makes writing generic code that does something with 
the underlying data very difficult (lots of switches and code repetition).

On the other hand, having element type as template parameter, like in STL containers, is often inconvenient and requires 
some template magic for simple things like passing forward the container or reading metadata when the data type is not 
known a priori.

To get the best of two worlds at cost of a bit more complicated interface LLU provides the following hierarchy of container 
classes:

#### Level 0:

* MTensor
* MNumericArray
* MImage
* DataStore

These are just raw _LibraryLink_ containers. If someone wants to use them directly, they probably don't need LLU at all, but it's possible to use them within LLU as well.

#### Level 1A:

* [GenericTensor\<PassingMode\>](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1MContainer_3_01MArgumentType_1_1Tensor_00_01PassingMode_01_4.html)
* [GenericNumericArray\<PassingMode\>](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1MContainer_3_01MArgumentType_1_1NumericArray_00_01PassingMode_01_4.html)
* [GenericImage\<PassingMode\>](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1MContainer_3_01MArgumentType_1_1Image_00_01PassingMode_01_4.html)
* [GenericDataList\<PassingMode\>](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1MContainer_3_01MArgumentType_1_1DataStore_00_01PassingMode_01_4.html)

These are type-unaware wrappers, offer automatic memory management and basic interface like access to metadata (dimensions, rank, etc). No direct access to underlying data.

#### Level 1B:
- Type-aware interface to [Tensor](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1TypedTensor.html)
- Type-aware interface to [NumericArray](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1TypedNumericArray.html)
- Type-aware interface to [Image](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1TypedImage.html)

These template classes offer iterators and data access functions for each container. They shouldn't be used directly, 
as they don't hold any data. Instead, use containers from level 2 which inherits from level 1B containers.

#### Level 2:
- [Tensor<T, PassingMode>](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1Tensor.html)
- [NumericArray<T, PassingMode>](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1NumericArray.html)
- [Image<T, PassingMode>](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1Image.html)
- [DataList<T, PassingMode>](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1DataList.html)

Full-fledged wrappers with automatic memory management (via Passing policies, see section below), type-safe data access, iterators, etc. 

The following table summarizes current status of _LibraryLink_ containers and their LLU wrappers:

| LibraryLink element |    Generic wrapper     |   Typed wrapper    |
|:-------------------:|:----------------------:|:------------------:|
|       MTensor       |    GenericTensor\<P\>    |    Tensor<T, P>    |
|    MNumericArray    | GenericNumericArray\<P\> | NumericArray<T, P> |
|       MImage        |    GenericImage\<P\>     |    Image<T, P>     |
|      DataStore      |   GenericDataList\<P\>   |   DataList<T, P>   |

## Passing policies
When passing a container from WL to a C++ library you have to choose one of the 4 available passing modes:

* Automatic
* Constant
* Manual
* Shared

With exception of DataStore, which cannot be Shared.

All of the above are described in the [LibraryLink documentation](https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithWolframLanguage.html#97446640).

In plain _LibraryLink_, the choice you make is reflected only in the WL code where you call `LibraryFunctionLoad` and specify 
the list of parameters for the library function. There is no way to query the WolframLibraryData or MArgument about 
the passing modes of function arguments from within C++ code. Therefore, the programmer must remember the passing mode 
for each argument and then ensure the correct action is taken (releasing/not releasing memory depending 
on the combination of passing mode and whether the container has been returned from the library function as result to WL). 
This design is far from perfect because manual resource management often leads to bugs and leaks.

As a remedy for this flaw of _LibraryLink_, LLU encodes the passing mode in a form of template parameter for each 
container wrapper. It makes sense because passing mode is known at compile time and cannot be changed throughout 
the life of container.

LLU defines 3 classes representing passing policies:

* Passing::Automatic
* Passing::Manual
* Passing::Shared

They serve as base classes to containers and they store and update the information whether the underlying raw container 
should be freed when the wrapper ends its life.

There is also `Passing::Constant` which is just an alias for Passing::Automatic because from the memory management 
point of view these two policies are equivalent.

Some examples:
```cpp
Tensor<mint, Passing::Manual> t { 1, 2,  3, 4, 5 };    // fine, new MTensor is allocated and it will be freed when t goes out of scope

Tensor<mint, Passing::Automatic> s { 1, 2,  3, 4, 5 };     // compile-time error, you cannot create a container with Automatic mode 
                                                           // because LibraryLink doesn't know about it and will not free it automatically

auto t = mngr.getGenericImage<Shared>(0);   // OK

auto copy = t;    // compile-time error, you cannot copy a Shared container because the copy will not be shared

LLU::GenericImage<Manual> clone {t};   // but this is fine, we make a deep copy which is no longer Shared
```

More examples can be found in unit tests.

<a name="mathlink"></a>
# MathLink support

_LibraryLink_ allows you to pass LinkObject as argument, which may then be utilized to exchange data between your library and the Kernel using MathLink. 
The original MathLink API is in old C style with error codes, macros, manual memory management, etc. Therefore, __LLU__ provides a wrapper for the LinkObject called `MLStream`.

`MLStream` is actually a class template parameterized by the default encoding to be used for strings, but for the sake of clarity, the template parameter is skipped in the remainder of this README.

<a name="mathlink-features"></a>
## Main features
### Convenient syntax

In this extension to __LLU__ MathLink is interpreted as an i/o stream, so operators << and >> are utilized to make the syntax cleaner and more concise. 
This means that the framework frees the developer from the responsibility to choose proper MathLink function for the data they intend to read or write.

### Error checking

Each call to MathLink has its return status checked. In case of failure an exception is thrown. Such exceptions carry some debug info to help locate the problem. Sample debug info looks like this:

```
Error code reported by MathLink: 48
"Unable to convert from given character encoding to MathLink encoding"
Additional debug info: MLPutUTF8String
```

### Memory cleanup

You're no longer required to call `MLRelease*` on the data received from MathLink. The framework does it for you.

### Automated handling of common data types

Some sophisticated types can be sent to Mathematica directly via `MLStream` class. For example nested maps:

```cpp
std::map<std::string, std::map<int, std::vector<double>>> myNestedMap
```

Just write `ms << myNestedMap` and you will get a nested Association on the other side. It works in the other direction as well. 
Obviously, for the above to work, key and value type in the map must be supported by MathLink.

If you have any particular type that you think should be directly supported by `MLStream`, please let me know.

### Easily extendable to custom classes

Suppose you have a structure

```cpp
struct Color {
    double red;
    double green;
    double blue;
};
```

It is enough to overload `operator<<` like this:

```cpp
MLStream& operator<<(MLStream& ms, const Color& c) {
    return ms << ML::Function("RGBColor", 3) << c.red << c.green << c.blue;
}
```

And now you're able to send objects of class `Color` directly via `MLStream`.

### Supports expressions of unknown length

Whenever you send an expression via MathLink you have to first specify the head and the number of arguments. It proved multiple times that this design is not very flexible, 
for example when you are reading from a file and cannot easily tell how much data is left.

As a workaround, one can create a temporary loopback link, accumulate all the arguments there (without the head), 
count the arguments and then send everything to the "main" link as usual.

The same strategy has been incorporated into `MLStream` so that developers do not have to implement it manually any longer. Now you can send a `List` like this:
```cpp
MLStream ms(mlp);

ms << ML::BeginExpr("List");
while (dataFromFile != EOF) {
	// process data from file and send to MLStream
}
ms << ML::EndExpr();
```

##### Word of caution

This feature should only be used if necessary since it requires a temporary link and makes extra copies
of data. Simple benchmarks showed a ~2x slowdown compared to the usual `MLPutFunction`.

<a name="mathlink-example"></a>
## Example
Again, let's compare the same piece of code written in plain _LibraryLink_ with one written with _LLU_ and `MLStream`. Take a look at the code snippet taken from one of the Import/Export paclets:

```cpp
if (!MLNewPacket(mlp)) {
    wsErr = -1;
    goto cleanup;
}
if (!MLPutFunction(mlp, "List", nframes)) {
    wsErr = -1;
    goto cleanup;
}
for (auto& f : extractedFrames) {
    if (!MLPutFunction(mlp, "List", 7)) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutFunction(mlp, "Rule", 2)) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutString(mlp, "ImageSize")) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutFunction(mlp, "List", 2)) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutInteger64(mlp, f->width)) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutInteger64(mlp, f->height)) {
        wsErr = -1;
        goto cleanup;
    }
    (...)
    if (!MLPutFunction(mlp, "Rule", 2)) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutString(mlp, "ImageOffset")) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutFunction(mlp, "List", 2)) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutInteger64(mlp, f->left)) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutInteger64(mlp, f->top)) {
        wsErr = -1;
        goto cleanup;
    }
    (...)
    if (!MLPutFunction(mlp, "Rule", 2)) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutString(mlp, "UserInputFlag")) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutSymbol(mlp, f->userInputFlag == true ? "True" : "False")) {
        wsErr = -1;
        goto cleanup;
    }
}
if (!MLEndPacket(mlp)) { 
/* unable to send the end-of-packet sequence to mlp */
}
if (!MLFlush(mlp)){ 
/* unable to flush any buffered output data in mlp */
}
```

and now the same code using `MLStream`:

```cpp
MLStream ms(mlp);

ms << ML::NewPacket;
ms << ML::List(nframes);

for (auto& f : extractedFrames) {
    ms << ML::List(7)
        << ML::Rule 
            << "ImageSize" 
            << ML::List(2) << f->width << f->height
        (...)
        << ML::Rule 
            << "ImageOffset" 
            << ML::List(2) << f->left << f->top
        (...)
        << ML::Rule 
            << "UserInputFlag" 
            << f->userInputFlag
}

ms << ML::EndPacket << ML::Flush;
```
<a name="error-handling"></a>
# Error handling
Every _LibraryLink_ function in C code has a fixed signature `int f (WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res)`. The actual result of computations should be returned via the
"out-parameter" `Res`. The value of `Res` is only considered in top-level if the actual return value of `f` (the `int`) was equal to `LIBRARY_NO_ERROR` (with LLU use `ErrorCode::NoError`!).

That means, that the only information about an error which occurred in the library that makes it to the top-level is a single integer. In C++ exceptions are the preferred way of error handling, so LLU
offers a special class of exceptions that can be easily translated to error codes, returned to _LibraryLink_ and then translated to descriptive `Failure` objects in Wolfram Language.

Such exceptions are identified in the C++ code by name - a short string. For example, imagine you have a function that reads data from a source. If the source does not exist or is empty, you want to throw
exceptions, let's call them "NoSourceError" and "EmptySourceError", respectively. First, you **must** register all your exceptions inside `WolframLibrary_initialize` function:

```cpp
EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
    try {
        ErrorManager::registerPacletErrors({
            {"NoSourceError", "Requested data source does not exist."},
            {"EmptySourceError", "Requested data source has `1` elements, but required at least `2`."}
        });
    } catch(...) {
        return LLErrorCode::FunctionError;
    }
    return LLErrorCode::NoError;
}
```

In the code above, the second element of each pair is a textual description of the error which will be visible in the `Failure` object.
This text may contain "slots" denoted as \`1\`, \`2\`, etc. that work like [TemplateSlots](https://reference.wolfram.com/language/ref/TemplateSlot.html) in Wolfram Language.
Notice that there is no way to assign specific error codes to your custom exceptions, this is handled internally by LLU.

Now, in the function that reads data:

```cpp

void readData(std::shared_ptr<DataSource> source) {
    if (!source) {
        ErrorManager::throwException("NoSourceError");
    }
    if (source->elemCount() < 3) {
        ErrorManager::throwException("EmptySourceError", source->elemCount(), 3);
    }
    //...
}
```

Each call to `ErrorManager::throwException` causes an exception of class `LibraryLinkError` with predefined name and error code to be thrown. 
All parameters in `ErrorManager::throwException` call after the first one are used to populate consecutive template slots in the error message.
The only thing left do now is to catch the exception. 
Usually, you will catch only in the interface functions (the ones with `EXTERN_C DLLEXPORT`), extract the error code from exception and return it:

```cpp
EXTERN_C DLLEXPORT int MyFunction(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
    auto err = LLErrorCode::NoError;    // no error initially
    try {
        //...
    } catch (const LibraryLinkError& e) {
        err = e.which();    // extract error code from LibraryLinkError
    } catch (...) {
        err = LLErrorCode::FunctionError;   // to be safe, handle non-LLU exceptions as well and return generic error code
    }
    return err;
}
```

The Wolfram Language part of the error-handling functionality of LLU is responsible for converting error codes returned by library functions into nice and informative `Failure` objects.

Finally, you can take a look at [PacletTemplate](https://stash.wolfram.com/projects/IMEX/repos/paclettemplate) (both source code and the test notebook), which is a simple "model paclet" that uses LLU.

<a name="limitations"></a>
# Limitations with respect to LibraryLink

There are some _LibraryLink_ features currently not covered by _LLU_, most notably:

- Sparse Arrays
- Tensor subsetting: `MTensor_getTensor`
- Managed Library Expressions
- Callbacks
- Wolfram IO Library (asynchronous tasks)

<a name="howToUse"></a>
# How to use _LibraryLink Utilities_?

<a name="howToUse-prerequisites"></a>
## Prerequisites

Since the source code uses C++14 features, you have to make sure your compiler supports C++14. This means:
 * **Visual Studio** >= 15.0 
 * **gcc** >= 5.0 
 * **clang** >= 3.4

Plus: 
 * **CMake** >= 3.8.0
 * **MathLink** interface version 4
 * **WolframLibrary** >= 5
 
 
MathLink library and WolframLibrary header files can be found in any Mathematica installation. For testing `wolframscript` must be available on the system.

<a name="howToUse-stepbystep"></a>
## Step-by-step
_LLU_ can be used like any other library after completing the following steps:

### 1. Get source code
You can clone _LLU_ from Stash like any other repository. _LLU_ resides in the _IMEX_ bank and is a public repo so there should never be problems with read access.
```
ssh://git@stash.wolfram.com:7999/imex/librarylinkutilities.git
```
If you don't want to clone a git repo you can just download a zip package from Stash containing a snapshot from any chosen branch.

### 2. Configure

#### Dependencies
_LLU_ depends on MathLink and on WolframLibrary so you must have those 2 installed on your system. 
Below is a quick overview of CMake variables which you can use to customize build process. Let's consider a number of possible scenarios:

1. __Use MathLink and WolframLibrary from Mathematica installation__

    If you have Mathematica __12.1__ installed in a default location, you don't have to set any variables and the build configuration step should succeed out of the box.
    If you have other version of Mathematica installed in a default location you only need to specify `MATHEMATICA_VERSION`, for example:
    ```bash
    cmake .. -DMATHEMATICA_VERSION=12.0
    ```
	If you installed Mathematica in nonstandard location, you have to provide the path to Mathematica installation
	(you don't need to specify `MATHEMATICA_VERSION`), for example if you want to link to WolframLibrary and MathLink from the XKernel:
	```bash
	cmake .. -DMATHEMATICA_INSTALL_DIR=/home/path/to/XKernel
    ```
     
2. __Use MathLink and WolframLibrary from arbitrary locations__
  
   For some reason you may want to link to MathLink or WolframLibrary not coming from Mathematica installation. In that case you have to pass 2 paths to cmake:
   ```bash
   cmake .. -DWOLFRAM_LIBRARY_PATH=/path/to/WolframLibrary -DWOLFRAM_MATHLINK_PATH=/my/own/MathLink/installation
   ```
   
#### Other Options
Other useful cmake variables used by _LLU_ include:

 - `BUILD_SHARED_LIBS` - Whether to build _LLU_ as shared library. By default static library is created.
 - `CMAKE_BUILD_TYPE` - Choose the type of build, this should match the type of build of your project.
 - `CMAKE_INSTALL_PREFIX` - Where to install _LLU_. By default it is installed to _install_ directory in the source tree.
 - `CMAKE_VERBOSE_MAKEFILE` - Useful for debugging.

### 3. Build, Install and (optionally) Test
After successful configuration you are just one `make && make install` away from the end.

When you have the library installed you may want to run unit test to confirm that everything went well. Currently there are 10 test modules defined:
- DataList
- ErrorReporting
- GenericContainers
- Image
- MathLink
- NumericArray
- ProgressMonitor
- Scalar
- String
- Tensor

You can run all of them with `make test` or `ctest`. It is possible to run specific test module, for example
```bash
ctest -R MathLink
```
Test targets actually call `wolframscript` under the hood, so you must have it installed in your system. That also means
that you will not see any useful output from `make test` (for example, you will not be shown which specific tests from each module failed). 

To improve the test feedback another CMake target called `TestWithOutputOnFailure` is defined. If you run 
```
make TestWithOutputOnFailure
```
and a test module fails,
you will see the whole output produced by ctest and wolframscript. There is still room for improvement in this area and all suggestions are welcome.

### 4. Link from your project
_LLU_ tries to follow modern CMake standards therefore it defines export target and hides build details. You don't need to manually set up any dependencies,
compiler flags, include paths, etc. After _LLU_ is installed, in your CMakeLists.txt call:
```cmake
find_package(LLU)
```
and later
```cmake
target_link_libraries(MyTarget PRIVATE LLU::LLU)
```
The last step is to copy the file with Wolfram Language code if you need the top-level features of LLU as well, for example:
```cmake
install(FILES "${LLU_LOCATION}/share/LibraryLinkUtilities.wl"
	DESTINATION "${PACLET_NAME}/LibraryResources"
)
```

<a name="howToUse-note"></a>
## Note for Import/Export developers

There is an alternative approach to what is described above that Import/Export developers tend to use.
In I/E paclets _LLU_ is included as a git submodule. Submodules are simply git repos inside other repos but working with them may sometimes be tricky. See this 
excellent [tutorial on submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules).

For SourceTree users there is also a helpful [blog post](https://blog.sourcetreeapp.com/2012/02/01/using-submodules-and-subrepositories/).

In most cases you will access _LibraryLink Utilities_ in "read-only" manner, i.e. you will just update the submodule to make sure you use the most recent version. 

When you work on your paclet you may occasionally find a bug in LLU or a missing feature. You should either report it or try to make changes yourself following the usual workflow: 
create new branch - implement changes - open PR - merge. It is possible to do it with LLU as a submodule in your project but you should really read the tutorial before you try.


Here is a list of commands that will be useful to developers working on Import/Export paclets (names ending with "Tools"). 
Usually these paclets have _CPPSource/_ directory containing the source code. It is easy to modify these commands so that they work for arbitrary project.

##### Adding _LibraryLink Utilities_ to your paclet
```bash
git submodule add ssh://git@stash.wolfram.com:7999/imex/librarylinkutilities.git CPPSource/LibraryLinkUtilities
```

##### Cloning a project that already uses _LibraryLink Utilities_
```bash
git clone --recursive ssh://git@stash.wolfram.com:7999/imex/<paclet_name_in_lowercase>.git
```

##### Updating _LibraryLink Utilities_ in your project
```bash
git submodule update --remote CPPSource/LibraryLinkUtilities/
```
Submodules work in a "detached head" state which means they stick to a chosen commit, so even if there are backwards incompatible changes merged to _LLU_ master
your project will not be affected unless you manually update the submodule.

With _LLU_ attached to your project in a submodule you always have the sources so you only need to follow steps 2 - 4 described above.

## Paclets that currently use _LibraryLink Utilities_
If you look for examples of _LLU_ usage and whatever is in this README was not enough you can take a look at paclets that already use _LLU_.
The list below may not be complete.

- [PacletTemplate](https://stash.wolfram.com/projects/IMEX/repos/paclettemplate) - this is a model paclet for Import/Export developers
- [GIFTools](https://stash.wolfram.com/projects/IMEX/repos/giftools)
- [MediaTools](https://stash.wolfram.com/projects/IMEX/repos/mediatools)
- [RAWTools](https://stash.wolfram.com/projects/IMEX/repos/rawtools)

<a name="APIreference"></a>
# API Reference

Doxygen is used to generate documentation for _LibraryLink Utilities_ API. You can browse generated docs online here: 

<http://algorithms.wolfram.com:8080/documentation/LibraryLinkUtilities>

<a name="contributors"></a>
# Contributors

* Rafał Chojna (<rafalc@wolfram.com>) - main developer
* Sean Cheren  (<scheren@wolfram.com>) - top-level code for error handling, CMake improvements
* Rebecca Frederick (<rebeccaf@wolfram.com> - CMake improvements
