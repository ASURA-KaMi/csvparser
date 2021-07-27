# csvparser 
## C++ parser with computing (OnlySTL)
To compile use g++ compile:
```
g++ csvparser.cpp -o csvparser.o
```
To run aplication use ./csvparser.o "filename"
```
./csvparser.o test.csv 
```
if filename arg is empty or overload, you got exception
```
asura@orchinon:~/Documents/cpp$ ./csvparser.o
ERR:You didn't enter a file name
```
```
asura@orchinon:~/Documents/cpp$ ./csvparser.o test.csv division.csv 
ERR:Too much arguments
```
