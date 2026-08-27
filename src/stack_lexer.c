#include "../include/stack_lexer.h"

lexic_branch* __initialize_branch(lexic_branch* branch){
	branch->children = 0; branch->parent = 0;
	branch->word_id = -1; branch->symbol = 0;
	branch->family_size = 0; branch->hard = 0;
	return branch;
}

int _insert_word(lexic_branch* root, lexic_branch* branches, lexic_word* word, int depth){
	char* text = word->word; int i = 0;
	lexic_branch* father = root; int carriage = 0;
	for(char c = text[i]; c != 0; c = text[++i]){
	for(lexic_branch* current = &branches[carriage];carriage < depth; current = &branches[++carriage]){
		if(current->symbol == 0){//We hit a null symbol, therefore the is no more words from here
			current->parent = father;
			current->symbol = c;
			father->family_size++;
			father = current;
			carriage++;
			__initialize_branch(&branches[carriage]);//Initalizes the next branch, so we know where the end is
			break;
		}
		if((current->symbol == c) && (current->parent == father)){
			father = current;
			carriage++;
			break;
		}
	}}
	father->word_id = word->word_id;
	father->hard = word->hard;
	return carriage;
}

void ___copy_branch(lexic_branch* source, lexic_branch* destination){
	destination->children = source->children;
	destination->word_id = source->word_id;
	destination->family_size = source->family_size;
	destination->hard = source->hard;
	destination->parent = source->parent;
	destination->symbol = source->symbol;
}

lexic_branch* __fetch_children(lexic_branch* parent, lexic_branch* source, lexic_branch* destination, int size){
	for(int i = 0; i < size; i++){
		lexic_branch* current = &source[i];
		if(current->parent != parent){ continue; }
		___copy_branch(current, destination);
		destination->children = current;//Used as a makeshift address holder.
		destination++;
	}

	return destination;
}

void _organize_branches(lexic_branch* source, lexic_branch* destination, int size){
	lexic_branch* end_destination = __fetch_children(0, source, destination, size);
	do{
		lexic_branch* previous_end_destination = end_destination;
		lexic_branch* current_end_destination = end_destination;

		for(lexic_branch* current_parent = destination; current_parent != end_destination; current_parent++){
			if(current_parent->family_size == 0){ current_parent->children = 0; continue; }
			current_end_destination = __fetch_children(current_parent->children, source, current_end_destination, size);
			current_parent->children = previous_end_destination;
			for(;previous_end_destination != current_end_destination; previous_end_destination++){
				previous_end_destination->parent = current_parent;
			}
		}
		destination = end_destination;
		end_destination = current_end_destination;
	}while(end_destination != destination);
}

int stack_lexer_build_codex(lexic_word* words, int wc, lexic_branch* branches, int bc){
	lexic_branch temp_branches[bc];
	lexic_branch* root = __initialize_branch(temp_branches);
	lexic_branch* newtop = __initialize_branch(temp_branches + 1);
	int general_top = 0;
	for(int i = 0; i < wc; i++){
		int curr_top = _insert_word(root, newtop, &words[i], bc);
		if(curr_top > general_top){ general_top = curr_top; }
	}

	_organize_branches(temp_branches, branches, general_top);

	return general_top;
}

//----------------------------------------------------------------------------------

lexic_branch* _find_child(lexic_branch* parent, lexic_branch* source, char target){
	if(!source){ return 0; }
	for(; source->parent == parent; source++){
		if(source->symbol == target){ return source; }
	}
	return 0;
}

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

void _write_token_word(lexic_token* token, int word_id){
	token->type = TOKEN_TYPE_WORD;
	token->value = word_id;
}

void _write_token_string(lexic_token* token, int size, char* ptr){
	token->type = TOKEN_TYPE_STRING;
	token->value = size;
	token->content = ptr;
}

void _write_token_number(lexic_token* token, int value){
	token->type = TOKEN_TYPE_NUMBER;
	token->value = value;
}

int stack_lexer_parse(char* source, lexic_branch* root, lexic_token* tokens){
	lexic_token* origin = tokens;

	int mode = 0, submode = 0;
	int value = 0; char* str = 0;

	lexic_branch* current_branch = root;
	lexic_branch* previous_branch = root;

	int id_start = 0, depth = 0;

	char c = source[0];
	for(int i = 0; c; c = source[++i]){
		switch(mode){
			case NONE: break;
			case STRING:
				if(c != submode){ value++; continue; }
				_write_token_string(tokens++, value, str);
				value = 0; str = 0; submode = 0; mode = NONE;
				continue;
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
				_write_token_number(tokens++, value);
				value = 0; i--; submode = 0; mode = NONE;
				continue;
		}

		previous_branch = current_branch;
		current_branch = _find_child(current_branch, current_branch->children, c);

		if(current_branch){
			if(mode != IDENTIFIER){ mode = IDENTIFIER; id_start = i; }
			depth++; continue;
		}//we found a token, skip other checks
        //If it reached here, we didn't find a token.

		current_branch = root;//Reset current, for the next loop

		if(previous_branch->word_id != -1){//if it was an endpoint token before invalidating
			if(previous_branch->hard){
				int id_end = i - depth;
				if(id_end != id_start){
					_write_token_string(tokens++, id_end - id_start, &source[id_start]);
				}
			}
			else if((depth < (i - id_start)) || (((c & 0xFF) > 32) && ((c & 0xFF) < 128))){
				previous_branch = current_branch; depth = 0; i--; continue;
			}

			_write_token_word(tokens++, previous_branch->word_id); depth = 0; i--;
			value = 0; submode = 0; mode = NONE;
			continue;
		}

		if(mode == IDENTIFIER){ if(c <= 32 && previous_branch->parent == 0){
			_write_token_string(tokens++, i - id_start, &source[id_start]);
			value = 0; submode = 0; mode = NONE;
		}}
		else if(depth == 0){
			if(c == 39 || c == 34 || c == 96){ mode = STRING; submode = c; str = &source[i + 1]; continue; }//if equals ' or " or ` XXX the part &source[i] might need to be i + 1
			if(c >= 48 && c <= 57){ mode = NUMBER; i--; continue; }
			if(c > 32){ mode = IDENTIFIER; id_start = i; continue; }
		}
		//If it reached here, we were on the path of a token, but got invalidated

		for(; depth > 0; depth--, i--){//step back in case we did at some point have a valid token.
			previous_branch = previous_branch->parent;
			if(previous_branch->word_id != -1 && previous_branch->hard){
				int id_end = i - depth;
				if(id_end != id_start){
					_write_token_string(tokens++, id_end - id_start, &source[id_start]);
					value = 0; str = 0; submode = 0; mode = NONE;
				}
				_write_token_word(tokens++, previous_branch->word_id); depth = 0; i -= 2; break;
			}
		}
	}

	return tokens - origin;
}
