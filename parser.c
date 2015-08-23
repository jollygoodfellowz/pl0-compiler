/*
	Alexander Kawa
	Chris Chaffin
	PL/0 Parser
	Systems Software
	Fall 2014
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SYMBOL_TABLE_SIZE 100
#define IDENT 11
#define MAX_DIG 5
#define MAX_LEXI_LEVELS 3
#define MAX_CODE_LENGTH 500

#define LIT 1
#define OPR 2
#define LOD 3
#define STO 4
#define CAL 5
#define INC 6
#define JMP 7
#define JPC 8

enum token_type
{
	nulsym = 1,
	identsym = 2,
	numbersym = 3,
	plussym = 4,
	minussym = 5,
	multsym = 6,
	slashsym = 7,
	oddsym = 8,
	eqsym = 9,
	neqsym = 10,
	lessym = 11,
	leqsym = 12,
	gtrsym = 13,
	geqsym = 14,
	lparentsym = 15,
	rparentsym = 16,
	commasym = 17,
	semicolonsym = 18,
	periodsym = 19,
	becomessym = 20,
	beginsym = 21,
	endsym = 22,
	ifsym = 23,
	thensym = 24,
	whilesym = 25,
	dosym = 26,
	callsym = 27,
	constsym = 28,
	varsym = 29,
	procsym = 30,
	writesym = 31,
	readsym = 32,
	elsesym = 33
};

typedef struct
{
	enum token_type kind; 		// const = 1, var = 2, proc = 3
	char name[IDENT];             // name up to 11 chars
	int val;                      // number (ASCII value)
	int level;                    // L level
	int addr;                     // M address
} symbol;

typedef struct
{
	int     op;
	int     l;
	int     m;
} instruct;

FILE* ifp;
FILE* ofp;

int symbolCounter = 0;
int pos = 0;
int codeCounter = 0;
int currentLevel = 0;
int token;

instruct code[MAX_SYMBOL_TABLE_SIZE * 3];
int stackCounter = 0;

symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
char data[MAX_SYMBOL_TABLE_SIZE * 12];

void readData();
int getNextToken();
void block();
void statement();
void condition();
int getConditional(int token);
int isRelation(int token);
void expression();
void term();
void factor();
void error(int errorValue);
void gen(int op, int l, int m);
int isNewVariable(char *name, int pos);
char* copySubstring(char* str, int start, int end);

int main(int argc, char **argv)
{
	int i, printFlag = 0;

	// Checks for flag passed in as argument
	for (i = 0; i < argc; i++)
	{
		if (strcmp(argv[i],"-a") == 0)
			printFlag = 1;
	}

	/* C:\\Users\\Max\\Documents\\Visual Studio 2013\\Projects\\ConsoleApplication1\\ConsoleApplication1\\ */
	ifp = fopen("lexemelist.txt", "r");
	ofp = fopen("mcode.txt", "w");

	readData();
	token = getNextToken();	
	block(token);

	printf("%d\nlex pos %d\n", token, pos);
	if (token != periodsym) // 19
		error(9);

	gen(9, 0, 2);	// SIO 0,3 Halt the machine

	// Writes the code to mcode.txt to be read by vm
	for (i = 0; i < codeCounter; i++)
		fprintf(ofp, "%d %d %d\n", code[i].op, code[i].l, code[i].m);

	// If the print flag is true, prints the code to console
	if (printFlag == 1)
	{
		for (i = 0; i < codeCounter; i++)
			printf("%d %d %d\n", code[i].op, code[i].l, code[i].m);
	}

	printf("No errors, program is syntactically correct.\n");

	fclose(ifp);
	fclose(ofp);

	return 0;
}

void readData()
{
	// j keeps track of the begining of each substring and ptr
	// keeps track of the next element of symbol_table
	int i, j, ptr, varCount = 0;
	char* tmp = NULL;

	// Scan all data into the array
	for (i = 0; !feof(ifp); i++)
		fscanf(ifp, "%c", &data[i]);

	// Process all of the data
	for (i = 0, j = 0, ptr = 0; data[i] != '\0' && ptr != MAX_SYMBOL_TABLE_SIZE; i++, ptr++, j = i)
	{
		// The most common case.
		if (isdigit(data[i]))
		{
			// Continue until the end of the number
			while (isdigit(data[i]))
				i++;

		}

		// When we have a variable name
		else if (isalpha(data[i]))
		{
			// Continue until the end of the var name
			while (isdigit(data[i]) || isalpha(data[i]))
				i++;

			// Decrement the pointer in the table because the previous symbol was a digit
			ptr--;
			symbol_table[ptr].kind = identsym;

		}

		// Save the newly parsed substring
		tmp = copySubstring(data, j, i);

		// If it is a variable we need to copy the name into the symbol table
		if (symbol_table[ptr].kind == identsym)
		{
			strcpy(symbol_table[ptr].name, tmp);
			
			if (isNewVariable(symbol_table[ptr].name, ptr))
			{
				symbol_table[ptr].addr = varCount;
				varCount++;
			}
				
		}
		else
		{
			symbol_table[ptr].kind = atoi(tmp);
			symbolCounter++;
		}
			

		// free up the memory
		memset(tmp, 0, i - j + 1);
		free(tmp);
	}

	if (ptr == MAX_SYMBOL_TABLE_SIZE)
		error(27);

	gen(INC, 0, varCount);
}

// Function to check if variable in symboltable is new
// or a repeat
int isNewVariable(char *name, int pos)
{
	int i;
	for (i = pos - 1; i >= 0; i--)
	{
		if (strcmp(symbol_table[i].name, name) == 0)
			return 0;
	}
	return 1;
}

// Gets the next token
int getNextToken()
{
	return symbol_table[pos++].kind;
}

// Code generation
void gen(int op, int l, int m)
{
	//if the code counter has gone above the maximum allowable code length, error out
	if (codeCounter > MAX_CODE_LENGTH)
		error(27);

	//otherwise add the passed args to the code array
	else
	{
		code[codeCounter].op = op;     //opcode
		code[codeCounter].l = l;       // lexicographical level
		code[codeCounter].m = m;       //modifier
		codeCounter++;

		stackCounter++;
	}

	if (op == STO)
		stackCounter--;
}

void block()
{
	// variable used to hold position of token
	int tempPos;
	if (token == constsym)
	{
		do
		{
			// saves location of constsym
			tempPos = pos;
			token = getNextToken();

			if (token != identsym)	// error if next token isn't identsym
				error(4);

			token = getNextToken();

			if (token != eqsym)	// error if next token isn't eqsym
				error(3);

			token = getNextToken();

			if (token != numbersym)	// error if next token isn't a numbersym
				error(2);

			token = getNextToken();

			symbol_table[tempPos].val = token;	// stores value of constant

			token = getNextToken();
		} while (token == commasym);

		if (token != semicolonsym)	// error if line isn't ended with semicolon
			error(5);

		token = getNextToken();
	}
	if (token == varsym)
	{
		do
		{
			token = getNextToken();

			if (token != identsym)	// error if token after varsym isn't an identifier
				error(4);

			token = getNextToken();
		} while (token == commasym);

		if (token != semicolonsym)	// error if line isn't ended with semicolon
			error(5);

		token = getNextToken();
	}
	if (token == procsym)
		error(28);

	statement();	// call statement
	printf("%d\n", token);
}	

void statement()
{
	int i;
	if (token == identsym)	// if token is identsym
	{
		token = getNextToken();

		if (token != becomessym)	// error if token after identsym isn't becomes
			error(19);

		token = getNextToken();
		expression();	// call expression

		// for loop to store the variable value
		for (i = 0; i < symbolCounter; i++)
		{
			if (strcmp(symbol_table[i].name, symbol_table[pos + 2].name) == 0)
			{
				gen(STO, 0, symbol_table[i].addr);
				break;
			}
		}
	}

	// This was missing from the pseudo code
	else if (token == callsym)
	{
		token = getNextToken();

		if (token != identsym)
			error(4);

		token = getNextToken();
	}
	// Everything between the comments was missing

	else if (token == beginsym)	// if token is beginsym
	{
		printf("%d\n", token);
		token = getNextToken();
		statement();	// call statement

		while (token == semicolonsym)	// while token is semicolon
		{
			token = getNextToken();	// get next token and call statement
			statement();
		}

		//code[25].l = 0;
		//symbol_table[50].val = 0;
		// printf("%d\n", token); // Could have an "elsesym"
		// if (token != endsym)	// error if there isn't an end symbol
		// 	error(26);

		token = getNextToken();
	}

	else if (token == ifsym)	// if token is an ifsym
	{
		token = getNextToken();
		condition();	// get next token and call condition

		if (token != thensym)	// error if next token isn't thensym
			error(16);

		token = getNextToken();
		statement();	// get next token and call statement
	}

	else if (token == whilesym)	// if token is whilesym
	{
		token = getNextToken();
		condition();	// get next token and call condition

		if (token != dosym)	// error if next token isn't dosym
			error(18);

		token = getNextToken();
		statement(); // get next token and call statement
	}
	
	if (token == writesym || token == readsym) // if token is read or write
	{
		if (token == writesym)
			gen(9, 0, 0);	// (SIO, 0, 0)
		else
			gen(9, 0, 1);	// (SIO, 0, 1)

		token = getNextToken();

		if (token != identsym)	// error if the next token isn't identsym
			error(14);

		token = getNextToken();

	}
}

void condition()
{
	int conditional;	// variable used to get conditional value of token

	if (token == oddsym)	// if token is oddsym
	{
		token = getNextToken();
		expression();	// get next token and call expression
	}
	else
	{
		expression();	// call expression

		if (!isRelation(token))	// error if token isn't relational
			error(20);

		conditional = getConditional(token);	// get conditional value of token

		token = getNextToken();
		expression();	// get next token and call expression

		gen(OPR, 0, conditional);	
	}
}

// Function to get the conditional value of a token
// Used to save conditional before getting another token
int getConditional(int token)
{
	if (token == eqsym)
		return eqsym;
	else if (token == neqsym)
		return neqsym;
	else if (token == lessym)
		return lessym;
	else if (token == leqsym)
		return leqsym;
	else if (token == gtrsym)
		return gtrsym;
	else if (token == geqsym)
		return geqsym;
	else
	{
		error(20);
		return -1;
	}
}

// Boolean function that return 1 if the token is a relation
// and returns 0 if it isn't
int isRelation(int token)
{
	if (token == eqsym || token == neqsym || token == lessym ||
		token == leqsym || token == gtrsym || token == geqsym)
		return 1;
	else
		return 0;
}

void expression()
{
	int addop;	// variable to save operator
	if (token == plussym || token == minussym)
	{
		addop = token;	// save the operator
		token = getNextToken();

		if (addop == minussym)
			gen(OPR, 0, 1); // neg
	}

	term();	// call term

	while (token == plussym || token == minussym)
	{
		addop = token;	// save operator
		token = getNextToken();
		term();	// get next token and call term

		if (addop == plussym)
			gen(OPR, 0, 2); // add
		else
			gen(OPR, 0, 3); // subtract
	}
}

void term()
{
	int mulop;	// variable to save operator
	factor();	// call factor

	while (token == multsym || token == slashsym)
	{
		mulop = token;	// save operator
		token = getNextToken();
		factor();	// get next token and call factor

		if (mulop == multsym)
			gen(OPR, 0, 4); // multiply
		else
			gen(OPR, 0, 5); // divide
	}
}

void factor()
{
	int i;	// variable for loop
	if (token == identsym)
	{
		token = getNextToken();

		for (i = 0; i < symbolCounter; i++)
		{
			if (strcmp(symbol_table[i].name, symbol_table[pos-2].name) == 0)	// if it found the symbol in symboltable
			{
				if (symbol_table[i].kind == 1)	// if it's a constant use lit
				{
					gen(LIT, 0, symbol_table[i].val);
					break;
				}
				else
				{
					gen(LOD, 0, symbol_table[i].addr);		// otherwise use lod
					break;
				}
			}
		}
	}
	else if (token == numbersym)	// if token is a numsym
	{
		token = getNextToken();
		gen(LIT, 0, token);			// use lit
		token = getNextToken();
	}
	else if (token == lparentsym)	// checks parenthesis
	{
		token = getNextToken();
		expression();	// get next token and call expression

		if (token != rparentsym)
			error(22);


		token = getNextToken();
	}
	else
		error(23);
}

// Function that contains all the error messages
void error(int errorValue)
{
	code[25].l = 0;
	symbol_table[50].val = 0;
	switch (errorValue)
	{
	case 1:
		printf("Error number 1, Use = instead of :=.\n");
		break;
	case 2:
		printf("Error number 2, = must be followed by a number.\n");
		break;
	case 3:
		printf("Error number 3, Identifier must be followed by =.\n");
		break;
	case 4:
		printf("Error number 4, const, var, procedure must be followed by identifier.\n");
		break;
	case 5:
		printf("Error number 5, Semicolon or comma missing.\n");
		break;
	case 6:
		printf("Error number 6, Incorrect symbol after procedure declaration.\n");
		break;
	case 7:
		printf("Error number 7, Statement expected.\n");
		break;
	case 8:
		printf("Error number 8, Incorrect symbol after statement part in block.\n");
		break;
	case 9:
		printf("Error number 9, Period expected.\n");
		break;
	case 10:
		printf("Error number 10, Semicolon between statements missing.\n");
		break;
	case 11:
		printf("Error number 11, Undeclared identifier.\n");
		break;
	case 12:
		printf("Error number 12, Assignment to constant or procedure is not allowed.\n");
		break;
	case 13:
		printf("Error number 13, Assignment operator expected.\n");
		break;
	case 14:
		printf("Error number 14, call must be followed by an identifier.\n");
		break;
	case 15:
		printf("Error number 15, Call of a constant or variable is meaningless.\n");
		break;
	case 16:
		printf("Error number 16, then expected.\n");
		break;
	case 17:
		printf("Error number 17, Semicolon or } expected.\n");
		break;
	case 18:
		printf("Error number 18, do expected.\n");
		break;
	case 19:
		printf("Error number 19, Incorrect symbol following statement.\n");
		break;
	case 20:
		printf("Error number 20, Relational operator expected.\n");
		break;
	case 21:
		printf("Error number 21, Expression must not contain a procedure identifier.\n");
		break;
	case 22:
		printf("Error number 22, Right parenthesis missing.\n");
		break;
	case 23:
		printf("Error number 23, The preceding factor cannot begin with this symbol.\n");
		break;
	case 24:
		printf("Error number 24, An expression cannot begin with this symbol.\n");
		break;
	case 25:
		printf("Error number 25, This number is too large.\n");
		break;
	case 26:
		printf("Error number 26, end expected.\n");
		break;
	case 27:
		printf("Error number 27, Code generation overflow.\n");
		break;
	case 28:
		printf("Error number 28, Procedures not supported in this assignment.\n");
		break;
	}
	fclose(ifp);
	fclose(ofp);

	exit(EXIT_FAILURE);
}

// Pre condition: The pointer passed in must be NULL
// Post condition: The ptr passed in contains the new substring
// Takes in a string with the start and end specified
// and returns a pointer to the new substring
char* copySubstring(char* str, int start, int end)
{
	char* sub = (char*)malloc(sizeof(char)*(end - start + 1));
	int i, j;

	// Make sure space was found
	if (sub != NULL)
	{
		// Copy
		for (i = start, j = 0; i < end; i++, j++)
			sub[j] = str[i];

		sub[j] = '\0';
		return sub;
	}

	return NULL;
}
