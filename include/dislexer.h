#ifndef DISLEXER
#define DISLEXER

typedef enum {
	DISLEXER_TOKEN_TYPE_WORD	=	1,
	DISLEXER_TOKEN_TYPE_STRING	=	2,
	DISLEXER_TOKEN_TYPE_NUMBER	=	3
}dislexer_token_type;

typedef struct {
	int type;
	long long int value;
	void* content;
}dislexer_token;

typedef struct dslxbranch {
	char symbol;

	struct dslxbranch* parent;
	struct dslxbranch* children;

	struct dslxbranch* next;

	int word_id, hard;
}dislexer_branch;

/**
 * Initializes a branch with the expected "void" values.
 * @fn void dislexer_init_branch(dislexer_branch*)
 * @param root
 */
void dislexer_init_branch(dislexer_branch* root);

/**
 * Calculates how many branches does it need to add this word to the specified codex.
 * @fn int dislexer_test_word(dislexer_branch*, char*)
 * @param codex To be tested against
 * @param word To be tested
 * @return Count of branches needed
 */
int dislexer_test_word(dislexer_branch* codex, char* word);

/**
 * Adds the specified word to the codex, using preallocated slots.
 * Before using, it may be interesting to allocate the memory using the `dislexer_test_word` function.
 * @fn int dislexer_append_word(dislexer_branch*, dislexer_branch*, char*, int, int)
 * @param codex To be incremented
 * @param slots Array containing at least the required amount of free branches
 * @param word To be added
 * @param hard If 1, tells the parser that this word may be extracted even if it is part of an unrecognized word.
 * @param id Code identifier for the specified word
 * @return Count of how many branches of the preallocated slots were used
 */
int dislexer_append_word(dislexer_branch* codex, dislexer_branch* slots, char* word, int hard, int id);

/**
 * Analyses the specified sample, and extracts the first token data it can find, based on the specified codex.
 * @fn int dislexer_parse(dislexer_branch*, char*, dislexer_token*)
 * @param codex Reference of words
 * @param sample Provided text
 * @param token Preallocated token to recive the result.
 * @return How many chars were read to generate the result
 */
int dislexer_parse(dislexer_branch* codex, char* sample, dislexer_token* token);

#endif
