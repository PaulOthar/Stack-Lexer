#include "stack_lexer_2.h"

#include <stdio.h>

void __initialize_branch(lexic_branch* branch){
	branch->children = 0; branch->parent = 0;
	branch->word_id = -1; branch->symbol = 0;
	branch->family_size = 0; branch->hard = 0;
}

int _insert_word(lexic_word* word, lexic_branch* branches, int depth){
	char* text = word->word;int i = 0;
	lexic_branch* father = 0; int carriage = 0;
	for(char c = text[i]; c != 0; c = text[++i]){
	for(lexic_branch* current = &branches[carriage];carriage < depth; current = &branches[++carriage]){
		if(current->symbol == 0){//We hit a null symbol, therefore the is no more words from here
			current->parent = father;
			current->symbol = c;
			if(father){ father->family_size++; }
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

extern int stack_lexer_build_codex(lexic_word* words, int wc, lexic_branch* branches, int bc){
	lexic_branch temp_branches[bc]; __initialize_branch(temp_branches);
	int general_top = 0;
	for(int i = 0; i < wc; i++){
		int curr_top = _insert_word(&words[i], temp_branches, bc);
		if(curr_top > general_top){ general_top = curr_top; }
	}

	_organize_branches(temp_branches, branches, general_top);

	return general_top;
}

extern int stack_lexer_parse(char* source, lexic_branch* root, lexic_token* tokens){
	return 0;
}
