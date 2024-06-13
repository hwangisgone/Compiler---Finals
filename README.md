# MakeACompiler

### Building
Build in `build` directory

```
cmake ..
make
```

### Testing

Testing is done in `build` by using CTest

```
ctest --output-on-failure --test-dir ./scanner/
ctest --output-on-failure --test-dir ./parser/
```
 
