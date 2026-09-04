# Dislexer - Lexer library

Dislexer is a lexer library.

## Available Types and Structures

### 1. **`dislexer_token_type`**

```c
typedef enum {
	DISLEXER_TOKEN_TYPE_WORD	=	1,
	DISLEXER_TOKEN_TYPE_STRING	=	2,
	DISLEXER_TOKEN_TYPE_NUMBER	=	3
}dislexer_token_type;

```

### 2. **`dislexer_token`**

```c
typedef struct {
	int type;
	long long int value;
	void* content;
}dislexer_token;

```

### 3. **`dislexer_branch`**

```c
typedef struct dslxbranch {
	char symbol;

	struct dslxbranch* parent;
	struct dslxbranch* children;

	struct dslxbranch* next;

	int word_id, hard;
}dislexer_branch;

```

## Available Functions - `dislexer`

### 1. **`void dislexer_init_branch(dislexer_branch* root)`**

Initializes a branch with the expected "void" values.

* **Parameters**:
	* `root`



### 2. **`int dislexer_test_word(dislexer_branch* codex, char* word)`**

Calculates how many branches does it need to add this word to the specified codex.

* **Parameters**:
	* `codex` To be tested against
	* `word` To be tested


* **Returns**: Count of branches needed

### 3. **`int dislexer_append_word(dislexer_branch* codex, dislexer_branch* slots, char* word, int hard, int id)`**

Adds the specified word to the codex, using preallocated slots.
Before using, it may be interesting to allocate the memory using the `dislexer_test_word` function.

* **Parameters**:
	* `codex` To be incremented
	* `slots` Array containing at least the required amount of free branches
	* `word` To be added
	* `hard` If 1, tells the parser that this word may be extracted even if it is part of an unrecognized word.
	* `id` Code identifier for the specified word


* **Returns**: Count of how many branches of the preallocated slots were used

### 4. **`int dislexer_parse(dislexer_branch* codex, char* sample, dislexer_token* token)`**

Analyses the specified sample, and extracts the first token data it can find, based on the specified codex.

* **Parameters**:
	* `codex` Reference of words
	* `sample` Provided text
	* `token` Preallocated token to recive the result.


* **Returns**: How many chars were read to generate the result