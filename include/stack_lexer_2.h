#ifndef STACK_LEXER
#define STACK_LEXER

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

extern int stack_lexer_build_codex(lexic_word* words, int wc, lexic_branch* branches, int bc);

extern int stack_lexer_parse(char* source, lexic_branch* root, lexic_token* tokens);

#endif
