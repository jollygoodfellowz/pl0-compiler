/*
Chris Chaffin
Chris Schilling 
COP 3402
HW 2: Scanner
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variable for errors
int error = 0, printFlag = 0;

// Function that reads in the file with comments
// and removes them
void printClean(FILE *input, FILE *output)
{
	int i;
	char buffer[100];

	// While fgets does not return NULL
	while (fgets(buffer, 100, input))
	{
		i = 0;

		// While not end of line
		while (buffer[i] != '\0')
		{
			// Checks if there is a comment
			if ((buffer[i] == '/') && (buffer[i + 1] == '*'))
			{
				// Step over start comment symbol (/*)
				i += 2;

				// While loop to handle multi-line comments
				while (!(buffer[i] == '*' && buffer[i + 1] == '/'))
				{
					if (buffer[i] == '\n')
					{
						fgets(buffer, 100, input);
						i = 0;
					}
					else
						i++;
				}
				// Step over end comment symbol (*/)
				i += 2;
			}
			else
			{
				// Print the character if it's not inside a comment
				fprintf(output, "%c", buffer[i]);
				i++;
			}
		}
	}
}

// Function to check if a string is a valid
// identifier
int checkValidIdentifier(char *string)
{
	int i = 0;
	int character = 0;
	int firstNum = 0;

	// Checks the whole string
	while (string[i] != '\0')
	{
		// If the string starts with a number
		if (string[0] <= '9' && string[0] >= '0')
			firstNum = 1;

		// If the string contains characters
		if ((string[i] <= 'z' && string[i] >= 'a') || (string[i] <= 'Z' && string[i] >= 'A'))
			character = 1;

		i++;
	}

	// Not valid identifier
	if ((firstNum == 1) && (character == 1))
		return 0;

	// String is a number
	if ((firstNum == 1) && (character == 0))
		return 1;

	// Variable name
	if ((firstNum == 0) && (character == 1))
		return 2;
}

// Function to check if a character is a
// valid symbol
// The switch statement in which this function is called
// already checks through all the reserved symbols
void checkValidSymbol(char *string)
{
	// If and else if statements return if
	// character is a space, new line, tab, or null
	// terminator
	if (string[0] == ' ')
		return;
	else if (string[0] == '\n')
		return;
	else if (string[0] == '\t')
		return;
	else if (string[0] == '\0')
		return;
	else if (string[0] == 13)
		return;
	else
	{
		printf("Invalid symbol. %c, %d\n", string[0], string[0]);
		// printf("%c, %d\n", string[0], string[0]);
		error = 1;
	}

}

// Function to check if string is a reserved word,
// And calls checkValidIdentifier if it is not
int checkStrings(char *buffer, FILE *output)
{
	int i, value = -1, length = 0;
	int didSomething = 0;
	char buffer2[15];

	// Populates buffer with null terminators
	for (i = 0; i < 15; i++)
		buffer2[i] = '\0';

	// Resets i
	i = 0;

	// While still the same word
	while (buffer[i] != ' ')
	{
		// If there's a new line character, break
		if (buffer[i] == '\n')
			break;

		// If there's a tab, break
		if (buffer[i] == '\t')
			break;

		// Only add the character to the buffer if it's A-Z, a-z, or 0-9
		if ((buffer[i] >= 'a' && buffer[i] <= 'z') || (buffer[i] <= '9' && buffer[i] >= '0') || (buffer[i] >= 'A' && buffer[i] <= 'Z'))
		{
			length++;
			buffer2[i] = buffer[i];
		}
		else
			break;
		i++;
	}

	buffer2[i] = '\0';

	// If and else if to check if reserved word and writes
	// to file
	if (!strcmp(buffer2, "const"))
	{
		value = 28;
		fprintf(output, "const\t%d\n", value);
		didSomething = 1;
	}
	else if (!strcmp(buffer2, "var"))
	{
		value = 29;
		fprintf(output, "var\t%d\n", value);
		didSomething = 1;
	}
	else if (!strcmp(buffer2, "procedure"))
	{
		value = 30;
		fprintf(output, "procedure\t%d\n", value);
		didSomething = 1;
	}
	else if (!strcmp(buffer2, "call"))
	{
		value = 27;
		fprintf(output, "call\t%d\n", value);
		didSomething = 1;
	}
	else if (!strcmp(buffer2, "begin"))
	{
		value = 21;
		fprintf(output, "begin\t%d\n", value);
		didSomething = 1;
	}
	else if (!strcmp(buffer2, "end"))
	{
		value = 22;
		fprintf(output, "end\t%d\n", value);
		didSomething = 1;
	}
	else if (!strcmp(buffer2, "if"))
	{
		value = 23;
		fprintf(output, "if\t%d\n", value);
		didSomething = 1;
	}
	else if (!strcmp(buffer2, "then"))
	{
		value = 24;
		fprintf(output, "then\t%d\n", value);
		didSomething = 1;
	}
	else if (!strcmp(buffer2, "else"))
	{
		value = 33;
		fprintf(output, "else\t%d\n", value);
		didSomething = 1;
	}
	else if (!strcmp(buffer2, "while"))
	{
		value = 25;
		fprintf(output, "while\t%d\n", value);
		didSomething = 1;
	}
	else if (!strcmp(buffer2, "do"))
	{
		value = 26;
		fprintf(output, "do\t%d\n", value);
		didSomething = 1;
	}
	else if (!strcmp(buffer2, "read"))
	{
		value = 32;
		fprintf(output, "read\t%d\n", value);
		didSomething = 1;
	}
	else if (!strcmp(buffer2, "write"))
	{
		value = 31;
		fprintf(output, "write\t%d\n", value);
		didSomething = 1;
	}
	else if (!strcmp(buffer2, "odd"))
	{
		value = 8;
		fprintf(output, "odd\t%d\n", value);
		didSomething = 1;
	}
	else
	{
		// If the length is greater than 11
		if (length > 11)
		{
			// Name is too long, sets error to true
			printf("Name too long.");
			error = 1;
			didSomething = 1;
		}
		else if ((checkValidIdentifier(buffer2) == 0) && buffer2[0] != '\0')
		{
			// If the variable does not start with a letter, sets error to true
			printf("Variable does not start with a letter.\n");
			error = 1;
			didSomething = 1;
		}
		else if (checkValidIdentifier(buffer2) == 1)
		{
			// If the string is a number greater than 5 digits
			if (length > 5)
			{
				// Number is too long, sets error to true
				printf("Number too long.\n");
				error = 1;
				didSomething = 1;
			}
			else
			{
				// Prints the number to the file
				value = 3;
				fprintf(output, "%s\t%d\n", buffer2, value);
				didSomething = 1;
			}

		}
		else if (checkValidIdentifier(buffer2) == 2)
		{
			// If it's a valid identifier, print it to the file
			value = 2;
			fprintf(output, "%s\t%d\n", buffer2, value);
			didSomething = 1;
		}
	}

	// Sets length to zero if nothing was done by function
	if (didSomething == 0)
		length = 0;

	// Returns how long the string was so the function
	// that called this can skip the string
	return length;
}

// Function to print the lexemetable
void printLexemetable(FILE *output)
{
	FILE *input = fopen("cleaninput.txt", "r");

	char buffer[100];
	int value = -1;
	int i = 0;

	// Prints the header to the file
	fprintf(output, "lexeme\ttoken type\n");

	// While fgets doesn't return NULL
	while (fgets(buffer, 100, input))
	{
		i = 0;

		// While not new line
		while (buffer[i] != '\n')
		{
			// If buffer at index i is a null terminator,
			// break out of loop
			if (buffer[i] == '\0')
				break;

			// Adds indexes to skip returned from checkStrings
			i += checkStrings(&buffer[i], output);

			// Switch statement that checks for special symbols
			switch (buffer[i])
			{
			case '+':
				value = 4;
				fprintf(output, "+\t%d\n", value);
				break;
			case '-':
				value = 5;
				fprintf(output, "-\t%d\n", value);
				break;
			case '*':
				value = 6;
				fprintf(output, "*\t%d\n", value);
				break;
			case '/':
				value = 7;
				fprintf(output, "/\t%d\n", value);
				break;
			case '(':
				value = 15;
				fprintf(output, "(\t%d\n", value);
				break;
			case ')':
				value = 16;
				fprintf(output, ")\t%d\n", value);
				break;
			case '=':
				value = 9;
				fprintf(output, "=\t%d\n", value);
				break;
			case ',':
				value = 17;
				fprintf(output, ",\t%d\n", value);
				break;
			case '.':
				value = 19;
				fprintf(output, ".\t%d\n", value);
				break;
			case '<':
				if (buffer[i + 1] == '>')
				{
					value = 10;
					fprintf(output, "<>\t%d\n", value);
					i++;
				}
				else if (buffer[i + 1] == '=')
				{
					value = 12;
					fprintf(output, "<=\t%d\n", value);
					i++;
				}
				else
				{
					value = 11;
					fprintf(output, "<\t%d\n", value);
				}
				break;
			case '>':
				if (buffer[i + 1] == '=')
				{
					value = 14;
					fprintf(output, ">=\t%d\n", value);
					i++;
				}
				else
				{
					value = 13;
					fprintf(output, ">\t%d\n", value);
				}
				break;
			case ';':
				value = 18;
				fprintf(output, ";\t%d\n", value);
				break;
			case ':':
				if (buffer[i + 1] == '=')
				{
					value = 20;
					fprintf(output, ":=\t%d\n", value);
					i++;
				}
				break;
			default:
				checkValidSymbol(&buffer[i]);
			}
			i++;
		}
	}
	fclose(input);
}

// Function to print the lexemelist
void printLexemelist(FILE *output)
{
	FILE *input = fopen("lexemetable.txt", "r");
	char buffer1[100];
	char buffer2[10];

	// Scans the header, which is then overwritten by fscanf
	fgets(buffer1, 100, input);

	// While both fscanf's are not null
	while (fscanf(input, "%s", buffer1) == 1 && fscanf(input, "%s", buffer2) == 1)
	{
		// Print the token
		fprintf(output, "%s ", buffer2);

		if (printFlag == 1)
			printf("%s ", buffer2);

		// If the token type is number or variable, print the
		// number or variable
		if ((buffer2[0] == '2' || buffer2[0] == '3') && buffer2[1] == '\0')
		{
			fprintf(output, "%s ", buffer1);

			if (printFlag == 1)
				printf("%s ", buffer1);
		}

	}

	if (printFlag == 1)
		printf("\n");

	fclose(input);
}

int main(int argc, char **argv)
{
	int i;

	for (i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "-l") == 0)
			printFlag = 1;
	}

	// Initializes and opens files
	FILE *input = fopen("input.txt", "r");
	FILE *inputclean = fopen("cleaninput.txt", "w");
	FILE *lexemetable = fopen("lexemetable.txt", "w");
	FILE *lexemelist = fopen("lexemelist.txt", "w");

	// Creates the cleaninput file and closes after it finishes
	printClean(input, inputclean);
	fclose(inputclean);

	// Creates the lexemetable and closes after it finishes
	printLexemetable(lexemetable);
	fclose(lexemetable);

	// If there were any errors, don't print the
	// lexeme list
	if (error == 0)
		printLexemelist(lexemelist);

	// Closes the files that have not alread been closed
	fclose(input);
	fclose(lexemelist);
}
