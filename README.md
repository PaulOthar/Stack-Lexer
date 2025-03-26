# Stack-Lexer - Simple lexical analysis library
Stack-Lexer is a simple static library for performing lexical analysis on strings. It is designed to store tokens and value in the stack, allowing you to """"pre-compile"""" a lexical codex for static token identification.

## How to use
### Step 1: Include the library in your project

```c
#include "stack_lexer.h"
```

### Step 2: Compile and link the library
To link the library you will need to include a few flags:
```bash
# if the library folder is in the root of the project:
-I./Stack-Lexer/include -L./Stack-Lexer/lib -lstack_lexer

#if the library folder is in some 'lib' folder, inside the project:
-I./lib/Stack-Lexer/include -L./lib/Stack-Lexer/lib -lstack_lexer
```

## Available Functions

### 1. **`uint64_t stack_lexer_read_number(uint32_t token)`**

Unpacks and reads the token based on the value stored in the `generic_stack` global variable.

- **Parameters**: `token` — The token.
- **Returns**: The unpacked value from the token.

### 2. **`char* stack_lexer_read_string(uint32_t token)`**

Converts the token into a string based on the value stored in the `generic_stack` global variable.

- **Parameters**: `token` — The token.
- **Returns**: A pointer to the string unpacked from the token.

### 3. **`void stack_lexer_32_attach(char* word, uint8_t result, uint32_t codex[256], uint8_t parent, uint8_t* stack_top)`**

Adds a word to the vocabulary (codex), allowing it to be identified during lexical analysis.

- **Parameters**:
  - `word`: The word to be added to the vocabulary.
  - `result`: The identifier code for this word (ranging from 1 to 254).
  - `codex`: The vocabulary (an array of 256 entries) where the word will be stored.
  - `parent`: The parent word (set as `0xff` if it’s a new word).
  - `stack_top`: The top of the stack, which is automatically updated.

### 4. **`void stack_lexer_32_sort(uint32_t codex[256])`**

Sorts the items in the vocabulary (`codex`), making it ready for use in lexical analysis.

- **Parameters**: `codex` — The vocabulary to be sorted.

### 5. **`void stack_lexer_32_build_codex(uint32_t codex[256], char* words)`**

Builds a new vocabulary from a set of words.

- **Parameters**:
  - `codex`: A pre-allocated codex to be populated.
  - `words`: A list of words to be identified (separated by commas).

### 6. **`void stack_lexer_32_scan(char* text, uint32_t codex[256])`**

Performs lexical analysis on a text, scanning for tokens, and stores the found tokens in the global `tokens_stack`. Literals of both values and strings are stored in the `generic_stack`.

- **Parameters**:
  - `text`: The text to be scanned for tokens.
  - `codex`: The vocabulary to be used for identifying tokens.

## Important Variables and Macros

### Stacks Used:

- **`tokens_stack[STACK_LEXER_TOKENS_STACK_SIZE]`**: Stack for storing tokens during lexical analysis.
- **`generic_stack[STACK_LEXER_GENERIC_STACK_SIZE]`**: Stack for storing literals (strings, numbers, etc.).

### Macro Functions:

- **`PUSH_TOKEN(TOKEN)`**: Pushes a token onto the `tokens_stack`.
- **`PUSH_GENERIC(GENERIC)`**: Pushes a value onto the `generic_stack`.

### Token Types:

- **`STRING_TOKEN(LOCATION)`**: Token for strings.
- **`UNSIGNED_INT__8_TOKEN(LOCATION)`**: Token for 8-bit unsigned integer.
- **`UNSIGNED_INT_16_TOKEN(LOCATION)`**: Token for 16-bit unsigned integer.
- **`UNSIGNED_INT_32_TOKEN(LOCATION)`**: Token for 32-bit unsigned integer.
- **`UNSIGNED_INT_64_TOKEN(LOCATION)`**: Token for 64-bit unsigned integer.

### Customizable Macros

There are several macros in the `stack_lexer.h` header that can be customized to fit the needs of your specific use case. These macros are **defined with `#ifndef`** to allow overriding during compilation or in your project’s code.

### 1. **`STACK_LEXER_BUILDER_SEPARATOR`**
Defines the separator used when building a list of words in the vocabulary. By default, it is a comma (`,`).

```c
#ifndef STACK_LEXER_BUILDER_SEPARATOR
#define STACK_LEXER_BUILDER_SEPARATOR ','
#endif
```

- **Customizable**: You can modify this to use any other separator, like a space (`' '`), semicolon (`;`), etc.

### 2. **`STACK_LEXER_SYMBOL_NUMBER_START`**
Defines the starting symbol for identifying number tokens. By default, it is set to `'['`.

```c
#ifndef STACK_LEXER_SYMBOL_NUMBER_START
#define STACK_LEXER_SYMBOL_NUMBER_START '['
#endif
```

- **Customizable**: Change this symbol to any other character for your lexic analysis needs.

### 3. **`STACK_LEXER_SYMBOL_NUMBER_END`**
Defines the ending symbol for number tokens. By default, it is set to `']'`.

```c
#ifndef STACK_LEXER_SYMBOL_NUMBER_END
#define STACK_LEXER_SYMBOL_NUMBER_END ']'
#endif
```

- **Customizable**: Adjust it as needed to match your format.

### 4. **`STACK_LEXER_SYMBOL_NUMBER_NEXT`**
Defines the symbol that separates multiple numbers. By default, it is a comma (`,`).

```c
#ifndef STACK_LEXER_SYMBOL_NUMBER_NEXT
#define STACK_LEXER_SYMBOL_NUMBER_NEXT ','
#endif
```

- **Customizable**: Change it to another symbol if required, such as a semicolon (`;`), space (`' '`), etc.

### 5. **`STACK_LEXER_SYMBOL_STRING_CAPTURING`**
Defines the symbol used for capturing string tokens. By default, it is set to `"`, the double quote.

```c
#ifndef STACK_LEXER_SYMBOL_STRING_CAPTURING
#define STACK_LEXER_SYMBOL_STRING_CAPTURING '"'
#endif
```

- **Customizable**: Modify it to any other character used for string capturing.

### 6. **`STACK_LEXER_TOKENS_STACK_SIZE`**
Defines the size of the token stack. The default size is 256 tokens.

```c
#ifndef STACK_LEXER_TOKENS_STACK_SIZE
#define STACK_LEXER_TOKENS_STACK_SIZE 256
#endif
```

- **Customizable**: Change this value to increase or decrease the size of the token stack.

### 7. **`STACK_LEXER_GENERIC_STACK_SIZE`**
Defines the size of the generic stack. The default size is 1024 values.

```c
#ifndef STACK_LEXER_GENERIC_STACK_SIZE
#define STACK_LEXER_GENERIC_STACK_SIZE 1024
#endif
```

- **Customizable**: Adjust the size of the generic stack as necessary.

### Example of Modifying Macros

If you want to change the token separator from a comma to a semicolon and increase the size of the `generic_stack` to 2048, you can modify the macros like this:

```c
#define STACK_LEXER_BUILDER_SEPARATOR ';'
#define STACK_LEXER_GENERIC_STACK_SIZE 2048
```

## License

This library is licensed under the **MIT License**. You can read more about it at [https://opensource.org/licenses/MIT](https://opensource.org/licenses/MIT).
