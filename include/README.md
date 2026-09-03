# Header Files (`include/`)

Place all public header files (`.h` / `.hpp`) in this directory.

### Usage
The CMake build system automatically adds this directory to the compiler's include path. You can include your headers from any source file in `src/` using standard include directives:

```cpp
#include "your_header.h"
```