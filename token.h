/*Contains all definitions related to tokens and lists of tokens parsed by the parser and then processed*/

#ifndef TOKEN_HEADER
#define TOKEN_HEADER

typedef enum TokenType{
	INVALID_T = -1,	/*marks invalid token which couldn't be parsed*/
	BLANK_T= 0,	/*sequence of white spaces on a single line which is followed by some non-white token*/
	ENDL_T, /*sequence of white spaces which close the line*/
	PUNC_T,	/*punctuation mark, one of: "#" , ",", "." , ":" */
	OPER_T, /*operation name token*/
	DIR_T,	/*directive name token*/
	NAME_T,	/*name token - sequence of characters where the first is a letter, and others are alphanumeric*/
	STRLIT_T,/*string literal - sequence of characters between " quotes*/
	INTLIT_T,/*integer literal with or without preceding sign*/
	REG_T	/*register token, "r" followed by an integer 0-7*/
} TokenType;

typedef struct Token{
	TokenType type;	/*type of the token*/
	int val;	/*token value, depends on the type, see the specification below*/

	char *start;	/*token string start*/
	int len;	/*token string length*/
} Token;

/*Token values specification:
PUNC_T: value contains the character
REG_T: value contains the register number
OPRE_R: value contains the opcode
DIT_T: value contains the dircode
INTLIT_T: value contains the integral value

In other types, the value is undefined
*/

/*Token list definitions*/
typedef struct TokNode{
	Token tok;
	struct TokNode *next;
} TokNode;

typedef TokNode *TokList;

#endif
