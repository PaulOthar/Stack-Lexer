#include "stack_lexer.h"

uint32_t tokens_stack[STACK_LEXER_TOKENS_STACK_SIZE];
uint32_t tokens_carriage;

uint8_t generic_stack[STACK_LEXER_GENERIC_STACK_SIZE];
uint32_t generic_carriage;

static void _stack_lexer_write_number(uint64_t value){
	if(!value){
		return;
	}

	uint8_t size = 0;
	uint32_t address = generic_carriage;
	for(size = 0;size < 8;size++){
		if(!value){
			break;
		}
		PUSH_GENERIC((value & 0xff));
		value = value >> 8;
	}
	PUSH_TOKEN(((size << 24) | address));
}

uint64_t stack_lexer_read_number(uint32_t token){
	uint64_t result = 0;
	uint8_t* generic = generic_stack + (token & 0xffffff);
	int size = (token >> 24) & 0xf;

	uint64_t holder = 0;
	for(int i = 0;i<size;i++){
		holder = generic[i];
		result |= (holder << (8 * i));
	}

	return result;
}

char* stack_lexer_read_string(uint32_t token){
	return (char*)(generic_stack + (token & 0xffffff));
}

typedef struct branch{
	struct branch* parent;
	struct branch* children[16];
	char symbol;
	uint32_t result;
	uint32_t index;
	uint32_t children_size;
}_branch;

//----<===={32 bit stack lexer}====>----//

#define SYMBOL_32(BRANCH) (BRANCH & 0xff)
#define PARENT_32(BRANCH) ((BRANCH >> 8) & 0xff)
#define CHILDS_32(BRANCH) ((BRANCH >> 16) & 0xff)
#define RESULT_32(BRANCH) ((BRANCH >> 24) & 0xff)
#define BRANCH_32(_SYMBOL,_PARENT,_CHILDS,_RESULT) ((_SYMBOL) | ((_PARENT) << 8) | ((_CHILDS) << 16) | ((_RESULT) << 24))
#define RETURN_32(_BRANCH,_RESULT) (((_BRANCH) & 0xffffff) | ((_RESULT) << 24))
#define ADOPTC_32(_BRANCH,_SCHILD) (((_BRANCH) & 0xff00ffff) | ((_SCHILD) << 16))

void stack_lexer_32_attach(char* word,uint8_t result,uint32_t codex[256],uint8_t parent,uint8_t* stack_top){
	if(!word[0]){//if we reached the null char
		codex[parent] = RETURN_32(codex[parent],result);//redefine the result of the current word
		return;
	}

	if(!stack_top){
		uint8_t top = 0;
		for(top = 0;top<255;top++){
			if(!codex[top]){
				break;
			}
		}
		stack_lexer_32_attach(word,result,codex,parent,&top);
		return;
	}

	if(CHILDS_32(codex[parent]) == 0xff){//If the parent is pointing to a invalid position, revalidate by pointing to top of the stack.
		codex[parent] = ADOPTC_32(codex[parent],stack_top[0]);
	}

	int initial_index = (parent == 0xff ? 0 : CHILDS_32(codex[parent]));//if the parent is 0xff, then it has no parent, therefore start for the beginning.
	for(int i = initial_index;i<256;i++){
		uint32_t branch = codex[i];
		if(!branch){//no value, hence, no more branches from now on.
			if(!word[1]){//if we will reach a null char
				codex[i] = BRANCH_32(*word,parent,0xff,result);
				break;
			}
			codex[i] = BRANCH_32(*word,parent,i+1,0);
			stack_lexer_32_attach(++word,result,codex,i,stack_top);
			break;
		}
		if(PARENT_32(branch) == parent && SYMBOL_32(branch) == (*word&0xff)){
			stack_lexer_32_attach(++word,result,codex,i,stack_top);
			break;
		}
	}
}

static void _stack_lexer_32_sort_initialize(uint32_t codex[256],_branch branches[],int size){
	int item = 0;
	for(item = 0;item<size;item++){//iterates over the codex, initializing and converting uint32_t in branch.
		uint8_t parent = PARENT_32(codex[item]);
		if(parent == 0xff){
			branches[item].parent = 0;
		}
		else{
			branches[item].parent = &branches[parent];
			branches[parent].children[branches[parent].children_size++] = &branches[item];
		}
		for(int l = 0;l<16;l++){
			branches[item].children[l] = 0;
		}
		branches[item].symbol = (char)SYMBOL_32(codex[item]);
		branches[item].result = RESULT_32(codex[item]);
		branches[item].children_size = 0;
	}
}

static void _stack_lexer_32_sort_children(_branch sorted[],_branch* parent,int* stack_size){
	int prev_size = *stack_size;
	int curr_size = *stack_size;
	for(uint32_t i = 0;i<parent[0].children_size;i++){
		parent[0].children[i][0].index = curr_size;
		sorted[curr_size++] = (parent[0].children[i][0]);
	}
	stack_size[0] = curr_size;

	for(int i = prev_size;i < curr_size;i++){
		_stack_lexer_32_sort_children(sorted,&sorted[i],stack_size);
	}
}

void stack_lexer_32_sort(uint32_t codex[256]){
	int size = 0;
	for(size = 0;size<256;size++){
		if(!codex[size]){
			break;
		}
	}
	_branch branches[size];
	_stack_lexer_32_sort_initialize(codex,branches,size);

	_branch sorted[size];
	int stack_size = 0;
	for(int i = 0;i<size;i++){//puts the root words in the sorted array, setting its index accordingly.
		if(!branches[i].parent){
			branches[i].index = stack_size;
			sorted[stack_size++] = branches[i];
		}
	}

	int prev_size = stack_size;
	for(int i = 0;i<prev_size;i++){
		_stack_lexer_32_sort_children(sorted, &sorted[i], &stack_size);
	}

	for(int i = 0;i<size;i++){//Rebuilds the codex, based on the sorted array.
		_branch current = sorted[i];
		uint8_t sorted_parent = 0xff;
		if(current.parent){
			sorted_parent = current.parent->index;
		}
		uint8_t sorted_child = 0xff;
		if(current.children[0]){
			sorted_child = current.children[0]->index;
		}
		codex[i] = BRANCH_32(current.symbol,sorted_parent,sorted_child,current.result);
	}
}

void stack_lexer_32_build_codex(uint32_t codex[256], char* words){
	char word[256];
	int result = 1;
	for(int i = 0;i<256;i++){ codex[i] = 0; word[i] = 0; }
	int word_carriage = 0;
	uint8_t stack_top = 0;
	for(char letter = *words++;letter;letter = *words++){
		if(letter == STACK_LEXER_BUILDER_SEPARATOR){
			word[word_carriage] = '\0';
			word_carriage = 0;
			stack_lexer_32_attach(word,result++,codex,0xff,&stack_top);
			continue;
		}
		word[word_carriage++] = letter;
	}
	if(word_carriage){
		word[word_carriage] = '\0';
		stack_lexer_32_attach(word,result++,codex,0xff,&stack_top);
	}

	stack_lexer_32_sort(codex);
}

void stack_lexer_32_scan(char* text,uint32_t codex[256]){
	tokens_carriage = 0;
	generic_carriage = 0;

	uint64_t number = 0;
	uint32_t string_start = 0;
	uint8_t current_branch = 0xff;
	uint8_t previous_branch = 0xff;
	uint8_t mode = 0;
	uint8_t result = 0;
	for(char current = *(text++);current;current = *(text++)){
		if(current_branch == 0xff && !mode){//if we ARE NOT dealing with a token nor a generic type
			if(previous_branch != 0xff){//but we WAS dealing with a token
				result = RESULT_32(codex[previous_branch]);
				if(result){ PUSH_TOKEN(result); }
				result = 0;
				previous_branch = 0xff;
			}
			for(int i = 0;i<255;i++){//Try to find a root token that has the current symbol
				if(PARENT_32(codex[i]) != 0xff){ break; }
				if(SYMBOL_32(codex[i]) == (current & 0xff)){ current_branch = i; break; }
			}
			if(current_branch == 0xff){//If we have not found the token, maybe it is a generic type?
				switch(current){
				case STACK_LEXER_SYMBOL_NUMBER_START: mode = 1; number = 0; break;
				case STACK_LEXER_SYMBOL_STRING_CAPTURING: mode = 2; string_start = generic_carriage; break;
				}
			}
		}
		else if(mode){//if we are dealing with a generic
			switch(current){
			case STACK_LEXER_SYMBOL_NUMBER_NEXT: _stack_lexer_write_number(number); number = 0; continue;
			case STACK_LEXER_SYMBOL_NUMBER_END: mode = 0; _stack_lexer_write_number(number); continue;
			case STACK_LEXER_SYMBOL_STRING_CAPTURING: mode = 0; PUSH_TOKEN(STRING_TOKEN(string_start)); continue;
			}
			switch(mode){
			case 1: number *= 10; number += (current & 0xff) - 48; continue;
			case 2: PUSH_GENERIC(current); continue;
			}
		}
		else{//if we are dealing with a token
			previous_branch = current_branch;
			if(CHILDS_32(codex[current_branch]) == 0xff){
				current_branch = 0xff;
				text--;
				continue;
			}
			for(int i = CHILDS_32(codex[current_branch]);i<255;i++){
				if(current_branch != PARENT_32(codex[i])){//if it is not from this sequence
					current_branch = 0xff;
					text--;
					break;
				}
				if((current & 0xff) == SYMBOL_32(codex[i])){
					current_branch = i;
					break;
				}
			}
		}
	}
	switch(mode){
	case 1: _stack_lexer_write_number(number); return;
	case 2: PUSH_TOKEN(STRING_TOKEN(string_start)); return;
	}
	if(current_branch != 0xff){
		result = RESULT_32(codex[current_branch]);
		if(result){ PUSH_TOKEN(result); }
		return;
	}
}

//----<===={64 bit stack lexer}====>----//
