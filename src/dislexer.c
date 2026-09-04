#include "dislexer.h"

static dislexer_branch* find_symbol(dislexer_branch* symbols, char symbol){
	for(; symbols; symbols = symbols->next){
		if(symbols->symbol == symbol){ return symbols; }
	}
	return 0;
}

static void write_token_string(dislexer_token* token, long long int size, char* str){
	token->type = DISLEXER_TOKEN_TYPE_STRING;
	token->value = size;
	token->content = str;
}

static void write_token_number(dislexer_token* token, long long int value){
	token->type = DISLEXER_TOKEN_TYPE_NUMBER;
	token->value = value;
	token->content = 0;
}

static void write_token_word(dislexer_token* token, long long int code){
	token->type = DISLEXER_TOKEN_TYPE_WORD;
	token->value = code;
	token->content = 0;
}

//-----------------------------------------------------------------------------------------------------------

void dislexer_init_branch(dislexer_branch* root){
	root->symbol = 0;

	root->parent = 0;
	root->children = 0;
	root->next = 0;

	root->hard = 0;
	root->word_id = -1;
}

int dislexer_test_word(dislexer_branch* codex, char* word){
	while(codex && word[0]){
		codex = find_symbol(codex->children, word[0]);
		if(!codex){ break; }
		word++;
	}

	int result = 0;
	while(word[0]){ word++; result++; }
	return result;
}

int dislexer_append_word(dislexer_branch* codex, dislexer_branch* slots, char* word, int hard, int id){
	int used = 0;
	for(dislexer_branch* found; word[0]; word++, codex = found){
		found = find_symbol(codex->children, word[0]);
		if(found){ continue; }

		found = slots++;
		dislexer_init_branch(found);
		found->symbol = word[0];
		found->parent = codex;
		found->next = codex->children;

		codex->children = found;

		used++;
	}

	codex->hard = hard;
	codex->word_id = id;

	return used;
}

//-----------------------------------------------------------------------------------------------------------

typedef enum {
	NONE        =   0,
	STRING      =   1,
	NUMBER      =   2,
	IDENTIFIER  =   3,
}parser_modes;

typedef enum{
	DECIMAL     =   1,
	BINARY      =   2,
	OCTAL       =   3,
	HEXADECIMAL =   4,
}parser_submodes;

#define CHAR_END '\x7f'

int dislexer_parse(dislexer_branch* codex, char* sample, dislexer_token* token){
	int mode = 0, submode = 0;
	int value = 0; char* str = 0;

	dislexer_branch* current_branch = codex;
	dislexer_branch* previous_branch = codex;

	int id_start = 0, depth = 0;

	token->content = 0;
	token->type = 0;
	token->value = 0;

	int i = 0;
	for(char c = sample[0]; c != CHAR_END; c = c ? sample[++i] : CHAR_END){
		switch(mode){
			case NONE: break;
			case STRING:
				if(c != submode){ value++; continue; }
				write_token_string(token, value, str);
				return i + 1;
			case NUMBER:
				switch(submode){
					default:
						if(value > 0){ submode = DECIMAL; }
						else if(c == 66 || c == 98){ submode = BINARY; continue; }
						else if(c == 79 || c == 111){ submode = OCTAL; continue; }
						else if(c == 88 || c == 120){ submode = HEXADECIMAL; continue; } // @suppress("No break at end of case")
						//Should leak to Decimal (default)
					case DECIMAL:
						if(c >= 48 && c <= 57){ value *= 10; value += c - 48; continue; }
						break;
					case BINARY:
						if(c >= 48 && c <= 49){ value <<= 1; value |= c - 48; continue; }
						break;
					case OCTAL:
						if(c >= 48 && c <= 55){ value <<= 3; value |= c - 48; continue; }
						break;
					case HEXADECIMAL:
						if(c >= 48 && c <= 57){ value <<= 4; value |= c - 48; continue; }
						if(c >= 65 && c <= 70){ value <<= 4; value |= c - 55; continue; }
						if(c >= 97 && c <= 102){ value <<= 4; value |= c - 87; continue; }
						break;
				}
				write_token_number(token, value);
				return i;
		}

		previous_branch = current_branch;
		current_branch = find_symbol(current_branch->children, c);

		if(current_branch){//we found a token, skip other checks
			if(mode != IDENTIFIER){ mode = IDENTIFIER; id_start = i; }
			depth++; continue;
		}
		//If it reached here, we didn't find a token.

		current_branch = codex;//Reset current, for the next loop

		if(previous_branch->word_id != -1){//if it was an endpoint token before invalidating
			if(previous_branch->hard){
				int id_end = i - depth;
				if(id_end != id_start){
					write_token_string(token, id_end - id_start, &sample[id_start]);
					return id_end;
				}
			}
			else if((depth < (i - id_start)) || (((c & 0xFF) > 32) && ((c & 0xFF) < 128))){
				previous_branch = current_branch; depth = 0; i--; continue;
			}
			write_token_word(token, previous_branch->word_id);
			return i;
		}

		if(mode == IDENTIFIER){ if(c <= 32 && previous_branch->parent == 0){
			write_token_string(token, i - id_start, &sample[id_start]);
			return i;
		}}
		else if(depth == 0){
			if(c == 39 || c == 34 || c == 96){ mode = STRING; submode = c; str = &sample[i + 1]; continue; }//if equals ' or " or ` XXX the part &source[i] might need to be i + 1
			if(c >= 48 && c <= 57){ mode = NUMBER; i--; continue; }
			if(c > 32){ mode = IDENTIFIER; id_start = i; continue; }
		}
		//If it reached here, we were on the path of a token, but got invalidated

		for(; depth > 0; depth--, i--){//step back in case we did at some point have a valid token.
			previous_branch = previous_branch->parent;
			if(previous_branch->word_id != -1 && previous_branch->hard){
				int id_end = i - depth;
				if(id_end != id_start){
					write_token_string(token, id_end - id_start, &sample[id_start]);
					return id_end - id_start;
				}
				write_token_word(token, previous_branch->word_id);
				return i - 1;
			}
		}
	}

	return i;
}
