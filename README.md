# PECWMA(Program dEpendence based Concurrent Weak Mutation Analysis) - A C++ mutation testing tool

## Mutant opreators
PECWMA supports the opeartors as below:

|Name         | Description           | Example  |
| :-------------: |:-------------:| :-----:|
|AOR  |  Replace arithmetic operator | `a + b` -> `a − b` |
|UOR  |Replace unary operator  | `a++` -> `a--` |
|CSR  | Replace condition | `if(a==b)` -> `if(1)`|
| LOR | Replace logic operator     | `a && b` -> `a` &#124;&#124; `b`  |
| ROR | Replace relational operator     | `a == b` -> `a >= b`       |
| LVR | Replace literal value       | `T` -> `T+1`, `T` -> `0`, `T` -> `1`  |
| BOR | Replace bit operator        | `a & b` -> `a` &#124; `b` |
| SOR | Replace shift operator          | `a >> b` -> `a << b`       |
| STDC | Delete a call                  | `foo()` -> nop          |
| STDS | Delete a store                 | `a = 5` -> nop          |
| UOI | Insert a unary operation        | `b = a` -> `b = ++a` OR `foo(a)`->`foo(++a)`  |
| ROV | Replace the operation value     | `foo(a,b)` -> `foo(b,a)` OR `a-b`->`b-a`      |
| ABV | Take absolute value     | `foo(a,b)` -> `foo(abs(a),b)` OR `a-b` ->  `abs(a)-b`      |

## Build Steps
1. make a directory, such as `mkdir build`, `cd build`
2. run `cmake dir-to-pecwma`, if cmake run in infinite loop, you should stop it with `Ctrl^C`, and run
```
export CC=/usr/local/bin/clang
export CXX=/usr/local/bin/clang++
```
[Maybe this is the reason?](https://stackoverflow.com/questions/7031126/switching-between-gcc-and-clang-llvm-using-cmake) Or it is a bug?
3. run `make`
4. create a pecwma.json like below,
```json
{
    "object":{
        "bubble":{
            "mutantsPath":"/home/tang/tools/test/CPMS/src/mutator/source/shuju/bubble/MutationT/",
            "programPath":"/home/tang/tools/test/CPMS/src/mutator/source/shuju/bubble/Source/bubble.c",
            "rootPath":"/home/tang/tools/test/CPMS/src/mutator/build/",
            "testcasePath":"/home/tang/tools/test/CPMS/src/mutator/source/shuju/bubble/Testcase/"
        },
        "tcas":{
            "mutantsPath":"/home/tang/tools/test/CPMS/src/mutator/source/shuju/tcas/MutationT/",    //directory of mutants
            "programPath":"/home/tang/tools/test/CPMS/src/mutator/source/shuju/tcas/Source/tcas.c", //path to source file
            "rootPath":"/home/tang/tools/test/CPMS/src/mutator/build/",                             //work directory
            "testcasePath":"/home/tang/tools/test/CPMS/src/mutator/source/shuju/tcas/Testcase/"     //directory of testcase 
        }
    }
}
```
5. run `./Pecwma`
