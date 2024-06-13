/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
*/

/*
Code updated at 10/05/24
Dang Viet Hoang - 20215206
*/

#include <stdio.h>
#include <stdlib.h>
// #include <conio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"
#include "scanner.h"


#include <unistd.h>
#include <linux/limits.h>

extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];
int state;
int ln, cn;
char str[MAX_IDENT_LEN + 1];	// Add space for \0
char c;

TokenType tk_temp_keyword = TK_NONE;
int noReadChar = 0;
/***************************************************************/
Token* getToken(void) 
{
	Token *token;
	// printf("\nstate - %d\n", state);
	switch(state)
	{
	case 0:
		if (currentChar == EOF) state = 1;
		else
		switch (charCodes[currentChar]) 
		{
			case CHAR_SPACE: 
				state = 2;
			break;
			case CHAR_LETTER:		state = 3; break;
			case CHAR_DIGIT:		state = 7; break;
			case CHAR_PLUS:			state = 9; break;
			case CHAR_MINUS:		state = 10; break;
			case CHAR_TIMES:		state = 11; break;
			case CHAR_SLASH:		state = 12; break;
			case CHAR_LT:				state = 13; break;
			case CHAR_GT:				state = 16; break;
			case CHAR_EQ:				state = 19; break;
			case CHAR_EXCLAMATION:	state = 20; break;
			case CHAR_COMMA:		state = 23; break;
			case CHAR_PERIOD:		state = 24; break;
			case CHAR_SEMICOLON:	state = 27; break;
			case CHAR_COLON:		state = 28; break;
			case CHAR_SINGLEQUOTE:	state = 31; break;
			case CHAR_LPAR:			state = 35; break;
			case CHAR_RPAR:			state = 42; break;
			default: 
				state = 43;
		}
		return getToken();
	case 1:
		return makeToken(TK_EOF, lineNo, colNo);
	case 2:
		do {
			readChar();
		} while(currentChar != EOF && charCodes[currentChar] == CHAR_SPACE);
		
		state = 0;
		return getToken();

	case 3:
		noReadChar = 1;
		ln = lineNo; cn = colNo;
		{
			int i = 0;
			state = 4;

			while(charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT) {
				if (i < MAX_IDENT_LEN) {
					str[i] = currentChar;
					i++;			
				}
				readChar();
			}
			str[i] = '\0';

			if (i >= MAX_IDENT_LEN) {
				error(ERR_IDENT_TOO_LONG, ln, cn);
				return makeToken(TK_NONE, lineNo, colNo);
			}
		}

		// printf("STR: %s\n", str);
		return getToken();

	case 4:	// And case 5, 6
		tk_temp_keyword = checkKeyword(str);
		if (tk_temp_keyword == TK_NONE) {
			state = 5;
			// printf("IDENTIFIER: %s\n", str);
			return getToken();
		} else {
			state = 6;
			// printf("KEYWORD: %s %ld %d\n", str, strlen(str), colNo);
			return getToken();
		}

	case 5:	// Identifiers
		token = makeToken(TK_IDENT, ln, cn);
		strcpy(token->string, str);
		return token;

	case 6: // Keywords
		return makeToken(tk_temp_keyword, ln, cn);

	case 7:
		noReadChar = 1;
		ln = lineNo; cn = colNo;
		token = makeToken(TK_NUMBER, lineNo, colNo);

		{ 
			int i = 0;
			while (currentChar == '0') { readChar(); } // Skip all leading 0 until 1-9

			while(charCodes[currentChar] == CHAR_DIGIT) {
				if (i < MAX_IDENT_LEN) {
					str[i] = currentChar;
					readChar();
					i++;
				} else {
					readChar();
				}
			}

			if (i == 0) { str[0] = '0'; i++; }	// edgee case where number is just 0;
			str[i] = '\0';
		}


		errno = 0; // Bound checking for strtol (better than atoi)
		char *end;
		long parsed_value = strtol(str, &end, 10);
		
		if (parsed_value > INT_MAX || parsed_value < INT_MIN || errno == ERANGE) {
			// NUMBER TOO LONG
			error(ERR_NUMBER_TOO_LONG, ln, cn);
			return makeToken(TK_NONE, lineNo, colNo);
		}
		
		token->value = parsed_value;
		strcpy(token->string, str);
		return token;

	case 9:		return makeToken(SB_PLUS, lineNo, colNo);
	case 10:	return makeToken(SB_MINUS, lineNo, colNo);
	case 11:	return makeToken(SB_TIMES, lineNo, colNo);
	case 12:	return makeToken(SB_SLASH, lineNo, colNo);

	case 13:
		ln = lineNo; cn = colNo;
		readChar();
		if (charCodes[currentChar] == CHAR_EQ) {
			state = 14; 
		} else { 
			state = 15;
			noReadChar = 1;
		}
		return getToken();

	case 14: return makeToken(SB_LE, ln, cn);
	case 15: return makeToken(SB_LT, ln, cn);

	case 16:
		ln = lineNo; cn = colNo;
		readChar();
		if (charCodes[currentChar] == CHAR_EQ) {
			state = 17; 
		} else { 
			state = 18;
			noReadChar = 1;
		}
		return getToken();

	case 17: return makeToken(SB_GE, ln, cn);
	case 18: return makeToken(SB_GT, ln, cn);

	case 19: return makeToken(SB_EQ, lineNo, colNo);

	case 20:
		ln = lineNo; cn = colNo;
		readChar();
		if (charCodes[currentChar] == CHAR_EQ) {
			state = 21; 
		} else { 
			state = 22;
		}
		return getToken();

	case 21: return makeToken(SB_NEQ, ln, cn);

	case 22:
		error(ERR_INVALID_SYMBOL, lineNo, colNo-1);
		return makeToken(TK_NONE, lineNo, colNo);

	case 23: return makeToken(SB_COMMA, lineNo, colNo);

	case 24:
		ln = lineNo; cn = colNo;
		readChar();
		if (charCodes[currentChar] == CHAR_RPAR) {
			state = 25; 
		} else { 
			state = 26;
			noReadChar = 1;
		}
		return getToken();

	case 25: return makeToken(SB_RSEL, ln, cn);
	case 26: return makeToken(SB_PERIOD, ln, cn);

	case 27:
		return makeToken(SB_SEMICOLON, lineNo, colNo);

	case 28:
		ln = lineNo; cn = colNo;
		readChar();
		if (charCodes[currentChar] == CHAR_EQ) {
			state = 29; 
		} else { 
			state = 30;
			noReadChar = 1;
		}
		return getToken();

	case 29: return makeToken(SB_ASSIGN, ln, cn);
	case 30: return makeToken(SB_COLON, ln, cn);

	case 31: // Single quote
		ln = lineNo; cn = colNo;
		readChar();
		if (currentChar != EOF && isprint(currentChar))
			state = 32;
		else
			state = 34;
		return getToken();

	case 32:
		c = currentChar;
		readChar();
		if (charCodes[currentChar] == CHAR_SINGLEQUOTE) 
			state = 33; 
		else 
			state = 34;
		return getToken();

	case 33:
		token = makeToken(TK_CHAR, lineNo, colNo-2);
		token->string[0] = c;
		token->string[1] ='\0';
		return token;

	case 34:
		error(ERR_INVALID_CONSTANT_CHAR, ln, cn);
		return makeToken(TK_NONE, lineNo, colNo);

	case 35: // tokens begin with lpar, skip comments
		ln = lineNo; cn = colNo;
		readChar();
		if (currentChar == EOF)
			state = 41;
		else 
			switch (charCodes[currentChar]) {
				case CHAR_PERIOD:	state = 36; break;
				case CHAR_TIMES:	state = 37; break;
				default: state = 41; noReadChar = 1;	// Need to process this
			}
		return getToken();
		
	case 36: return makeToken(SB_LSEL, ln, cn);

	case 37:
		// Skip until *
		do {
			readChar();
			// printf("%c", currentChar);
			if (currentChar == EOF) {
				state = 40;
				return getToken();
			}
		} while(charCodes[currentChar] != CHAR_TIMES);

		state = 38;
		return getToken();

	case 38:
		// Check if )
		// Skip *
		do {
			readChar();
		} while(charCodes[currentChar] == CHAR_TIMES);

		// printf("%c", currentChar);
		if (currentChar == EOF) {
			state = 40;
			return getToken();
		}

		if (charCodes[currentChar] != CHAR_RPAR) {
			state = 37;
			return getToken();
		}

		state = 39;
		return getToken();

	case 39:
		// End of comment
		state = 0;
		readChar();
		return getToken();

	case 40:
		error(ERR_END_OF_COMMENT, lineNo, colNo);
		return makeToken(TK_NONE, lineNo, colNo);

	case 41: return makeToken(SB_LPAR, ln, cn);
	case 42: return makeToken(SB_RPAR, lineNo, colNo);

	case 43:
		error(ERR_INVALID_SYMBOL, lineNo, colNo);
		return makeToken(TK_NONE, lineNo, colNo);

}
}

/******************************************************************/

void printToken(Token *token) {

	printf("%d-%d:", token->lineNo, token->colNo);

	switch (token->tokenType) {
	case TK_NONE: printf("TK_NONE\n"); break;
	case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
	case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
	case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
	case TK_EOF: printf("TK_EOF\n"); break;

	case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
	case KW_CONST: printf("KW_CONST\n"); break;
	case KW_TYPE: printf("KW_TYPE\n"); break;
	case KW_VAR: printf("KW_VAR\n"); break;
	case KW_INTEGER: printf("KW_INTEGER\n"); break;
	case KW_CHAR: printf("KW_CHAR\n"); break;
	case KW_ARRAY: printf("KW_ARRAY\n"); break;
	case KW_OF: printf("KW_OF\n"); break;
	case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
	case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
	case KW_BEGIN: printf("KW_BEGIN\n"); break;
	case KW_END: printf("KW_END\n"); break;
	case KW_CALL: printf("KW_CALL\n"); break;
	case KW_IF: printf("KW_IF\n"); break;
	case KW_THEN: printf("KW_THEN\n"); break;
	case KW_ELSE: printf("KW_ELSE\n"); break;
	case KW_WHILE: printf("KW_WHILE\n"); break;
	case KW_DO: printf("KW_DO\n"); break;
	case KW_FOR: printf("KW_FOR\n"); break;
	case KW_TO: printf("KW_TO\n"); break;
	case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
	case SB_COLON: printf("SB_COLON\n"); break;
	case SB_PERIOD: printf("SB_PERIOD\n"); break;
	case SB_COMMA: printf("SB_COMMA\n"); break;
	case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
	case SB_EQ: printf("SB_EQ\n"); break;
	case SB_NEQ: printf("SB_NEQ\n"); break;
	case SB_LT: printf("SB_LT\n"); break;
	case SB_LE: printf("SB_LE\n"); break;
	case SB_GT: printf("SB_GT\n"); break;
	case SB_GE: printf("SB_GE\n"); break;
	case SB_PLUS: printf("SB_PLUS\n"); break;
	case SB_MINUS: printf("SB_MINUS\n"); break;
	case SB_TIMES: printf("SB_TIMES\n"); break;
	case SB_SLASH: printf("SB_SLASH\n"); break;
	case SB_LPAR: printf("SB_LPAR\n"); break;
	case SB_RPAR: printf("SB_RPAR\n"); break;
	case SB_LSEL: printf("SB_LSEL\n"); break;
	case SB_RSEL: printf("SB_RSEL\n"); break;
	}
}


/******************************************************************/
#define TOKEN_LIMIT 200
// Token * tokenList[TOKEN_LIMIT] = { NULL };

int scan_all(char *fileName) {
	Token *token;

	if (openInputStream(fileName) == IO_ERROR)
		return IO_ERROR;

	int i = 0;

	token = getToken();
	while (token->tokenType != TK_EOF) {
		if (token->tokenType != TK_NONE) {
			// Only print token if not error // No storing
			printToken(token);
			// tokenList[i] = token;
		}
		free(token);


		if (!noReadChar) {
			readChar();
		} else {
			noReadChar = 0;
		}


		state = 0;
		token = getToken();


		// break;
		i++;
		if (i >= TOKEN_LIMIT) {
			printf("!!!!!REACHED TOKEN LIMIT!!!!!\n");
			break;
		}
	}

	free(token);
	closeInputStream();

	return IO_SUCCESS;
}



// // One time use
// int currentTokenCount = -1;
// Token* getValidToken(void) {
// 	Token* returnToken;

// 	if (currentTokenCount >= 0 && currentTokenCount < TOKEN_LIMIT) {
// 		returnToken = tokenList[currentTokenCount];
// 		tokenList[currentTokenCount] = NULL;
// 	}

// 	currentTokenCount++;
// 	return returnToken;
// }

Token* getValidToken(void) {
	state = 0;
	Token *token = getToken();

	if (!noReadChar) {
		readChar();
	} else {
		noReadChar = 0;
	}

	if (token->tokenType == TK_NONE) {
		free(token);
		token = getValidToken();
	}

	return token;
}
