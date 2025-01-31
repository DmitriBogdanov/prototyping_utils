# Selecting only specific UTL modules

[<- back to README.md](..)

Specific modules can be selected to restrict accessible functionality and reduce the amount of included standard headers.

This is achievable by declaring `#define UTL_PICK_MODULES` and `#define <module_identifier>` for all necessary modules before including the library header, see [examples](#selecting-specific-modules).

## Module Identifiers

| Name | Identifier |
| --- | --- |
| utl::json | UTLMODULE_JSON |
| utl::math | UTLMODULE_MATH |
| utl::mvl | UTLMODULE_MVL |
| utl::predef | UTLMODULE_PREDEF |
| utl::profiler | UTLMODULE_PROFILER |
| utl::progressbar | UTLMODULE_PROGRESSBAR |
| utl::random | UTLMODULE_RANDOM |
| utl::shell | UTLMODULE_SHELL |
| utl::sleep | UTLMODULE_SLEEP |
| utl::stre | UTLMODULE_STRE |
| utl::table | UTLMODULE_TABLE |
| utl::timer | UTLMODULE_TIMER |
| utl::voidstream | UTLMODULE_VOIDSTREAM |

## Examples

### Selecting specific modules

[ [Run this code](https://godbolt.org/#g:!((g:!((g:!((h:codeEditor,i:(filename:'1',fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,selection:(endColumn:23,endLineNumber:5,positionColumn:23,positionLineNumber:5,selectionStartColumn:23,selectionStartLineNumber:5,startColumn:23,startLineNumber:5),source:'//+Indicate+that+you+want+to+select+specific+modules%0A%23define+UTL_PICK_MODULES%0A%0A//+List+modules+that+should+be+compiled%0A%23define+UTLMODULE_MATH%0A%23define+UTLMODULE_PROGRESSBAR%0A%0A%23include+%3Chttps://raw.githubusercontent.com/DmitriBogdanov/prototyping_utils/master/include/proto_utils.hpp%3E%0A%0A%0Aint+main(int+argc,+char+**argv)+%7B%0A++++//+%3C+your+code+here+%3E++%0A++++return+0%3B%0A%7D%0A'),l:'5',n:'0',o:'C%2B%2B+source+%231',t:'0')),k:71.71783148269105,l:'4',n:'0',o:'',s:0,t:'0'),(g:!((g:!((h:compiler,i:(compiler:clang1600,filters:(b:'0',binary:'1',binaryObject:'1',commentOnly:'0',debugCalls:'1',demangle:'0',directives:'0',execute:'0',intel:'0',libraryCode:'0',trim:'1',verboseDemangling:'0'),flagsViewOpen:'1',fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,libs:!(),options:'-std%3Dc%2B%2B17+-O2',overrides:!(),selection:(endColumn:1,endLineNumber:1,positionColumn:1,positionLineNumber:1,selectionStartColumn:1,selectionStartLineNumber:1,startColumn:1,startLineNumber:1),source:1),l:'5',n:'0',o:'+x86-64+clang+16.0.0+(Editor+%231)',t:'0')),header:(),l:'4',m:50,n:'0',o:'',s:0,t:'0'),(g:!((h:output,i:(compilerName:'x86-64+clang+16.0.0',editorid:1,fontScale:14,fontUsePx:'0',j:1,wrap:'1'),l:'5',n:'0',o:'Output+of+x86-64+clang+16.0.0+(Compiler+%231)',t:'0')),k:46.69421860597116,l:'4',m:50,n:'0',o:'',s:0,t:'0')),k:28.282168517308946,l:'3',n:'0',o:'',t:'0')),l:'2',n:'0',o:'',t:'0')),version:4) ]
```cpp
// Indicate that you want to select specific modules
#define UTL_PICK_MODULES

// List modules that should be compiled
#define UTLMODULE_MATH
#define UTLMODULE_PROGRESSBAR

#include "proto_utils.hpp"


int main(int argc, char **argv) {
    // < your code here >  
    return 0;
}
```

### Selecting modules in different locations

[ [Run this code](https://godbolt.org/#g:!((g:!((g:!((h:codeEditor,i:(filename:'1',fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,selection:(endColumn:64,endLineNumber:23,positionColumn:64,positionLineNumber:23,selectionStartColumn:64,selectionStartLineNumber:23,startColumn:64,startLineNumber:23),source:'//+Pick+some+modules+in+one+header%0A%23define+UTL_PICK_MODULES%0A%23define+UTLMODULE_CONFIG%0A%23include+%3Chttps://raw.githubusercontent.com/DmitriBogdanov/prototyping_utils/master/include/proto_utils.hpp%3E%0A%0A//+...%0A%0A//+Pick+some+more+modules+in+another+header%0A%23define+UTL_PICK_MODULES%0A%23define+UTLMODULE_PROFILER%0A%23define+UTLMODULE_LOG%0A%23include+%3Chttps://raw.githubusercontent.com/DmitriBogdanov/prototyping_utils/master/include/proto_utils.hpp%3E%0A%0A//+...%0A%0A//+Translation+unit+that+includes+both+headers+in+any+order+will%0A//+have+access+to+utl::config,+UTL_PROFILER+and+UTL_LOG%0A%0A%0A//+This+allows+one+to+pull+only+necessary+modules+in+each+header.%0A//+Essentially,+as+long+as+UTL_PICK_MODULES+is+defined,+%22proto_utils.hpp%22%0A//+acts+as+a+collection+of+individual+module+headers+concatenated%0A//+into+a+single+file+and+enabled+with+%23define+UTLMODULE_%7BNAME%7D%0A%0A%0Aint+main(int+argc,+char+**argv)+%7B%0A++++//+%3C+your+code+here+%3E++%0A++++return+0%3B%0A%7D%0A'),l:'5',n:'0',o:'C%2B%2B+source+%231',t:'0')),k:71.71783148269105,l:'4',n:'0',o:'',s:0,t:'0'),(g:!((g:!((h:compiler,i:(compiler:clang1600,filters:(b:'0',binary:'1',binaryObject:'1',commentOnly:'0',debugCalls:'1',demangle:'0',directives:'0',execute:'0',intel:'0',libraryCode:'0',trim:'1',verboseDemangling:'0'),flagsViewOpen:'1',fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,libs:!(),options:'-std%3Dc%2B%2B17+-O2',overrides:!(),selection:(endColumn:1,endLineNumber:1,positionColumn:1,positionLineNumber:1,selectionStartColumn:1,selectionStartLineNumber:1,startColumn:1,startLineNumber:1),source:1),l:'5',n:'0',o:'+x86-64+clang+16.0.0+(Editor+%231)',t:'0')),header:(),l:'4',m:50,n:'0',o:'',s:0,t:'0'),(g:!((h:output,i:(compilerName:'x86-64+clang+16.0.0',editorid:1,fontScale:14,fontUsePx:'0',j:1,wrap:'1'),l:'5',n:'0',o:'Output+of+x86-64+clang+16.0.0+(Compiler+%231)',t:'0')),k:46.69421860597116,l:'4',m:50,n:'0',o:'',s:0,t:'0')),k:28.282168517308946,l:'3',n:'0',o:'',t:'0')),l:'2',n:'0',o:'',t:'0')),version:4) ]
```cpp
// Pick some modules in one header
#define UTL_PICK_MODULES
#define UTLMODULE_CONFIG
#include "proto_utils.hpp"

// ...

// Pick some more modules in another header
#define UTL_PICK_MODULES
#define UTLMODULE_PROFILER
#define UTLMODULE_LOG
#include "proto_utils.hpp"

// ...

// Translation unit that includes both headers in any order will
// have access to utl::config, UTL_PROFILER and UTL_LOG


// This allows one to pull only necessary modules in each header.
// Essentially, as long as UTL_PICK_MODULES is defined, "proto_utils.hpp"
// acts as a collection of individual module headers concatenated
// into a single file and enabled with #define UTLMODULE_{NAME}


int main(int argc, char **argv) {
    // < your code here >  
    return 0;
}
```

## Including modules directly

Individual modules can also be grabbed directly from [`include/UTL/`](../include/UTL/). In this case they can be included directly and no other action is needed.