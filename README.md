# SinaScript

SinaScript is a programming language interpreter that I wrote in C to run my custom language that looks mostly similar to JavaScript in syntax. To make this, I wrote a lexer, a hand-written recursive parser, and a VM that traverses the AST and executes the code directly (so no bytecode generation yet).

## Example

Insertion sort in SinaScript:

```js
# implementation of insertion sort in SinaScript

arr = [48, -19, 34, 12, 53, 0, 1, -99, 28, 28, 8, 12];

show("The array before sorting:", arr, "\n");

i = 1;
while (i < length(arr)) {
	j = i;
	while (j > 0) {
		if (arr[j - 1] > arr[j]) {
			tmp = arr[j - 1];
			arr[j - 1] = arr[j];
			arr[j] = tmp;
		}
		j = j - 1;
	}
	i = i + 1;
}

show("The sorted array is:", arr, "\n");
```

## Features

SinaScript currently has these features:

- Automatic memory management (with reference counting)
- Concurrency (right now this is only implemented in the `ctf` branch and not `master`)
- Arrays (including nested/multi-dimensional arrays)
- Strings
- While/if statements
- Simple builtin functions (e.g. `show`, `length`, `debugprint`, `alert`, etc.)

## CTF Challenge

I originally developed SinaScript as a CTF challenge for ASIS CTF Quals 2024 (see the writeup for the challenge `link TBD`).
