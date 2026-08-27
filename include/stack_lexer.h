#ifndef STACK_LEXER
#define STACK_LEXER

typedef enum {
	TOKEN_TYPE_WORD		=	1,
	TOKEN_TYPE_STRING	=	2,
	TOKEN_TYPE_NUMBER	=	3
}token_type;

typedef struct {
	int type;
	int value;
	void* content;
}lexic_token;

typedef struct lbranch {
	char symbol;
	struct lbranch* parent;
	struct lbranch* children; int family_size;
	int word_id;
	int hard;
}lexic_branch;

typedef struct {
	char* word;
	int word_id;
	int hard;
}lexic_word;

/**
 * Builds a 'lexic_codex' from a list of 'lexic_word'.
 * WARNING: if the branches array is too small, this system may be prone to segfault.
 * @fn int stack_lexer_build_codex(lexic_word*, int, lexic_branch*, int)
 * @param words List of pre-built words
 * @param words_size Size of words list
 * @param branches Array of pre-allocated branches (for appending words).
 * @param branches_size Size of the pre-allocated array.
 * @return Amount of used branches
 */
extern int stack_lexer_build_codex(lexic_word* words, int words_size, lexic_branch* branches, int branches_size);

/**
 * Parses a text and generates tokens, based on the text's content.
 * WARNING: if the tokens array is too small, this system may be prone to segfault.
 * @fn int stack_lexer_parse(char*, lexic_branch*, lexic_token*)
 * @param source Text to be parsed
 * @param root Root of the 'lexic_codex' we want to use for this text
 * @param tokens Array of pre-allocated tokens
 * @return Amount of tokens generated
 */
extern int stack_lexer_parse(char* source, lexic_branch* root, lexic_token* tokens);

#endif
