# F Interpreter
An interpreter for the F language, a language I made up. The definition of the language is in ```bnf.txt```. It is closely related to the C language.

## Features

- 3 data types: int, bool, char
- Numerical Operations: ```+```, ```-```, ```*```, ```/```, ```^```, and ```%```
- Boolean Operations: ```||```, ```&&```, and ```!```
- Relational Operations: ```<```, ```>```, ```<=```, ```>=```, ```==```, and ```!=```
- Control Flow: ```if```, ```else```, ```while```, and ```for```
- Function Definitions: pass and return variables through functions
- Input / Output: ```getchar``` and ```print```

## Usage

Pass a source file as an argument:

```
ffi.exe [source file].f
```

Source files must have the .f extension.

### Example Code

More examples can be found in the ```tests``` directory.

```
void main() {
    int a, b
    a = 4;
    b = 10;
    
    if (a + b < 12) {
        print("a plus b is less than 12");
    }
    else if (a + b == 12) {
        print("a plus b is equal to 12");
    }
    else {
        print("a plus b is greater than 12");
    }
}
```

### Example Output

```
a plus b is greater than 12
```