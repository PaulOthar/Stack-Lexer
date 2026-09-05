#include <stdio.h>
#include "dislexer.h"

dislexer_branch root;

int main(){
	dislexer_init_branch(&root);
	dislexer_branch prebranch[dislexer_test_word(&root, "pumpking")];
	dislexer_branch prebranch2[dislexer_test_word(&root, "pump")];
	dislexer_append_word(&root, prebranch, "pumpking", 0, 50);
	dislexer_append_word(&root, prebranch2, "pump", 1, 55);

	dislexer_token token;
	char* sample = " i am pumping my pumpking called pumpkeg";
	do{
		int size = dislexer_parse(&root, sample, &token);
		printf("%d -> %lld, %s\n", size, token.value, token.content ? token.content : "-");
		sample += size;
	}while(sample[0]);

	return 0;
}
