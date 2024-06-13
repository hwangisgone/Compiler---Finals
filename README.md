# MakeACompiler

### Building
Build in `build` directory

```
cmake ..
make
```

### Testing

Run example
```
./parser/run_parser ../typecheck/tests/example1.kpl
./parser/run_parser ../typecheck/tests/example2.kpl
...
```


Automated Testing is done in `build` by using CTest (Currently only works with parser, scanner)
```
ctest --output-on-failure --test-dir ./scanner/
ctest --output-on-failure --test-dir ./parser/
```
 
