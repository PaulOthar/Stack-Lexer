#ifndef STACK_LEXER
#define STACK_LEXER

#include <stdint.h>

#ifndef STACK_LEXER_BUILDER_SEPARATOR
#define STACK_LEXER_BUILDER_SEPARATOR ','
#endif

#ifndef STACK_LEXER_SYMBOL_NUMBER_START
#define STACK_LEXER_SYMBOL_NUMBER_START '['
#endif

#ifndef STACK_LEXER_SYMBOL_NUMBER_END
#define STACK_LEXER_SYMBOL_NUMBER_END ']'
#endif

#ifndef STACK_LEXER_SYMBOL_NUMBER_NEXT
#define STACK_LEXER_SYMBOL_NUMBER_NEXT ','
#endif

#ifndef STACK_LEXER_SYMBOL_STRING_CAPTURING
#define STACK_LEXER_SYMBOL_STRING_CAPTURING '"'
#endif

#ifndef STACK_LEXER_TOKENS_STACK_SIZE
#define STACK_LEXER_TOKENS_STACK_SIZE 256
#endif

#ifndef STACK_LEXER_GENERIC_STACK_SIZE
#define STACK_LEXER_GENERIC_STACK_SIZE 1024
#endif

extern uint32_t tokens_stack[STACK_LEXER_TOKENS_STACK_SIZE];
extern uint32_t tokens_carriage;
#define PUSH_TOKEN(TOKEN) tokens_stack[tokens_carriage++] = TOKEN

extern uint8_t generic_stack[STACK_LEXER_GENERIC_STACK_SIZE];
extern uint32_t generic_carriage;
#define PUSH_GENERIC(GENERIC) generic_stack[generic_carriage++] = GENERIC

#define STRING_TOKEN(LOCATION) (0x80000000 | LOCATION)
#define UNSIGNED_INT__8_TOKEN(LOCATION) (0x10000000 | LOCATION)
#define UNSIGNED_INT_16_TOKEN(LOCATION) (0x02000000 | LOCATION)
#define UNSIGNED_INT_24_TOKEN(LOCATION) (0x03000000 | LOCATION)
#define UNSIGNED_INT_32_TOKEN(LOCATION) (0x04000000 | LOCATION)
#define UNSIGNED_INT_40_TOKEN(LOCATION) (0x05000000 | LOCATION)
#define UNSIGNED_INT_48_TOKEN(LOCATION) (0x06000000 | LOCATION)
#define UNSIGNED_INT_56_TOKEN(LOCATION) (0x07000000 | LOCATION)
#define UNSIGNED_INT_64_TOKEN(LOCATION) (0x08000000 | LOCATION)

/**
 * Unpacks and reads the specified token, based on the values stored in the 'generic_stack'.
 * @fn uint64_t stack_lexer_read_number(uint32_t)
 * @param token Token extracted from the 'tokens_stack'
 * @return Value
 */
extern uint64_t stack_lexer_read_number(uint32_t token);

/**
 * Translates and cast the specified token into a string, based on the values stored in the 'generic_stack'.
 * @fn char stack_lexer_read_string*(uint32_t)
 * @param token Token extracted from the 'tokens_stack'
 * @return String pointer
 */
extern char* stack_lexer_read_string(uint32_t token);

//----<===={32 bit stack lexer}====>----//

#define SYMBOL_32(BRANCH) (BRANCH & 0xff)
#define PARENT_32(BRANCH) ((BRANCH >> 8) & 0xff)
#define CHILDS_32(BRANCH) ((BRANCH >> 16) & 0xff)
#define RESULT_32(BRANCH) ((BRANCH >> 24) & 0xff)

/**
 * Includes a new word to the codex, making it possible to identify it in a lexic analysis.
 * @fn void lexer_attach(char*, uint8_t, uint32_t[], uint8_t)
 * @param word The identifier that will be included.
 * @param result Code of this identifier, ranging between 1 and 254.
 * @param codex Codex that will include this word.
 * @param parent Parent word of the new word (Should be 0xff if this is a brand new word).
 * @param stack_top Top of the codex, that is, the first zero branch after the last valid branch. If zero, it will automatically grab the top.
 */
extern void stack_lexer_32_attach(char* word,uint8_t result,uint32_t codex[256],uint8_t parent,uint8_t* stack_top);

/**
 * Sorts the items of a codex, making it able to be used in an lexic analysis.
 * @fn void stack_lexer_sort(uint32_t[])
 * @param codex Codex to be sorted.
 */
extern void stack_lexer_32_sort(uint32_t codex[256]);

/**
 * Creates a new codex based on a set of strings.
 * The result codes will follow the order of inclusion:
 * Example: "Ship,Shoe,Shame" = 1,2,3.
 * @fn void stack_lexer_build_codex(uint32_t[], char*[])
 * @param codex Pre-allocated codex to be initialized and written to.
 * @param ... Words to be identified, by default it is separated by a comma ','.
 * @sa stack_lexer_attach
 * @sa stack_lexer_sort
 */
extern void stack_lexer_32_build_codex(uint32_t codex[256], char* words);

/**
 * Scans the specified text for tokens, based on the specified codex.
 * Tokens found will be stored in the global variable 'tokens_stack', you may use 'tokens_carriage' as a reference of size.
 * Literals of both values and strings will be stored in the global variable 'generic_stack',
 * but you may use specialized functions to translate the values.
 * @fn void stack_lexer_32_scan(char*, uint32_t[])
 * @param text Text to be scanned for tokens.
 * @param codex Source of all the symbols to be identified.
 */
extern void stack_lexer_32_scan(char* text,uint32_t codex[256]);

//----<===={64 bit stack lexer}====>----//

#endif
