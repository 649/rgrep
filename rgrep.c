#include <stdio.h>
#define MAXSIZE 4096

int rgrep_matches(char *line, char *pattern) {
	int sizeline = 0, sizepattern = 0,							// SIZE OF BOTH PATTERN AND LINE
		pattc = 0, pluscntr = 0, plusit = 0, questcntr = 0;		// WILDCARD COUNTERS
	char plus = '\0', quest = '\0';								// WILDCARD RESET FLAGS
	for (int i = 0; line[i] != '\0'; i++) {
		if (line[i] != '\n')
			sizeline++;											// HOW BIG THE LINE IS
	}for (int i = 0; pattern[i] != '\0'; i++)
		sizepattern++;											// HOW BIG THE PATTERN IS
	char tline[sizeline], tpatt[sizepattern];
	for(int i = 0; line[i] != '\0'; i++) {
		if (line[i] != '\n')
			tline[i] = line[i];									// STORE LINE POINTER ELEMENTS IN ARRAY
	}
	for(int i = 0; pattern[i] != '\0'; i++)
		tpatt[i] = pattern[i];									// STORE PATTERN POINTER ELEMENTS IN ARRAY
	tpatt[sizepattern] = '\0';
	while (tpatt[pattc + 1] == '?' && tpatt[pattc] != '\\')
		pattc += 2;												// SKIP ALL QUESTION MARKS AT BEGINNING
	for(int i = 0; tline[i] != '\0'; i++) {
		if ((tpatt[pattc + 1] == '+' || tpatt[pattc + 1] == '?') && tpatt[pattc] != '\\') {
			if (tpatt[pattc + 1] == '+') {						 // IF WE ARE LOOKING AT "+" AND NOT "?"
				pluscntr = pattc + 1;
				while (tpatt[pluscntr + 1] == '+')
					pluscntr++;
				int plusdown = pluscntr;
				if (plus == '\0') {
					if (pluscntr != 0) {
						while (tpatt[pluscntr] == '+') {
							plus = tpatt[pluscntr - 1];			 // ELEMENT BEFORE WILDCARD
							pluscntr--;							 // DECREMENT PLUS COUNTER
						}
					}
				}
				if (plus == '.') {
					while (tline[i] != tpatt[plusdown + 1]) {	 // WE STOP AT W/E IS AFTER WILDCARD
						++i; ++plusit;
					}
				} else {
					while (tline[i] == plus) {					 // NOT DOT SO MATCH LETTER BEFORE PLUS
						++i; ++plusit;
					}
				}
				if (tpatt[plusdown - 1] == tpatt[plusdown + 1])
					i--;										// FIX INDEX
				if (plusit > 0) {								// IF THE LOOP EXECUTED, SO WILL THIS
					pattc += plusdown - pattc + 1;				// WE NEGATE ANY EXTRA + AS IF IT WAS ONE
					i--;										// FIX THE INDEX
				} plus = '\0';									// RESET WILDCARD FOR NEXT ENCOUNTER
				if (tpatt[pattc] == '?')
					pattc++;									// QUICKLY CHECK FOR +? MULTI WILDCARD
			}
			else if (tpatt[pattc + 1] == '?') {					// IF WE ARE LOOKING AT "?" AND NOT "+"
				questcntr = pattc + 1;
				while (tpatt[questcntr + 1] == '?')
					questcntr++;
				int questdown = questcntr;
				if (quest == '\0') {
					if (questcntr != 0) {
						while (tpatt[questcntr] == '?') {
							quest = tpatt[questcntr - 1];		// ELEMENT BEFORE WILDCARD
							questcntr--;						// DECREMENT PLUS COUNTER
						}
					}
				}
				if (tpatt[pattc] == '.') {
					if (tpatt[pattc + 2] == '+') {				// QUICKLY CHECK FOR ?+ MULTI WILDCARD
						pattc++;
						while (tline[i] != tpatt[pattc + 3])
							i++;
						i--;
					}
					pattc += 2;
				}else if (tline[i] != tpatt[questdown-1] || tpatt[questdown - 1] == tpatt[questdown + 1]) {
					if (tpatt[pattc + 2] == '+') 				// QUICKLY CHECK FOR ?+ MULTI WILDCARD
						pattc++;
					pattc += 2;									// ELEMENT DOES NOT MATCH WILDCARD
					i--;
				}else if (tline[i] == tpatt[questdown-1]) {		// ELEMENT DOES MATCH WILDCARD
					if (tpatt[pattc + 2] == '+') {				// QUICKLY CHECK FOR ?+ MULTI WILDCARD
						pattc++;
						while (tline[i] != tpatt[pattc + 2])
							i++;
						i--;
					}
					pattc += 2;
				}
				quest = '\0';									// RESET WILDCARD FOR NEXT ENCOUNTER
			}
		}else if ((tpatt[pattc + 1] == '\\' || tpatt[pattc + 1] == '.' || tpatt[pattc + 1] == '+' || tpatt[pattc + 1] == '?') && tpatt[pattc] == '\\') {
			if (tline[i] == '\\') {
				if (tpatt[pattc + 2] == '+') {
					while (tline[i] == '\\')
						i++; 
					i--;
					pattc += 3;
					if (tpatt[pattc] == '?') 
						pattc++;								// "\\+?" RARE CASE
				}else if (tpatt[pattc + 2] == '?') 
					pattc += 3;
				else if (tpatt[pattc + 1] == '\\')
					pattc += 2;
			}else if (tline[i] == tpatt[pattc + 1]) {
				if (tpatt[pattc + 2] == '+') {					// OMITTED WILDCARDS THAT FACE PLUS WILDCARD
					while (tline[i] == tpatt[pattc+1])
						i++;
					i--;
					pattc += 3;
					if (tpatt[pattc] == '?') 
						pattc++;								// "\[WILDCARD]+?" RARE CASE
				}
				else if (tpatt[pattc + 2] == '?')
					pattc += 3;
				else 
					pattc += 2;
			}
		}else if (tline[i] == tpatt[pattc])
			pattc++;											// LINE HAS MATCHED PATTERN (BY ELEMENT)
		else if (tpatt[pattc] == '.'  && tpatt[pattc-1] != '\\')
				pattc++;										// WILDCARD "." HAS BEEN ENCOUNTERED
		else
			pattc = 0;											// NOTHING IN THIS LINE MATCHED PATTERN
		if (tpatt[pattc] == '\0') 
			return 1;											// PATTERN HAS BEEN MATCHED IN LINE
	}
	while (tpatt[pattc + 1] == '?')
		pattc += 2;
	pattc -= 2;
	if (tpatt[sizepattern - 1] == '?' && tpatt[sizepattern - 2] != '\\') {
		pattc += 2;												// RARE CASE WHERE LAST ELEMENT IS A QUESTION MARK
		if (tpatt[pattc] == '\0')
			return 1;
	}
	return 0;													// NO MATCH IN ELEMENT, SO RETURN 0
}
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <PATTERN>\n", argv[0]);
		fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
        return 2;
    }
	char *pattern = argv[1];
	int help = 0;
	if (pattern[0] == '-' && pattern[1] == '-' && pattern[2] == 'h' && pattern[3] == 'e' && pattern[4] == 'l' && pattern[5] == 'p')help++;
	if (help == 1) {
		fprintf(stderr, "EXAMPLE: %s 'St?r.n+g' < 'FILENAME'\n", argv[0]);
		fprintf(stderr, "\n");
		fprintf(stderr, "PATTERN OPTIONS: \n");
		fprintf(stderr, "\t'.' (period): Matches any character.\n");
		fprintf(stderr, "\t'+' (plus sign): The preceding character will appear one or more times.\n");
		fprintf(stderr, "\t'?' (question mark): The preceding character may or may not appear in the line.\n");
		fprintf(stderr, "\t'\\' (backslash): \"Escapes\" the following character, nullifying any special meaning it has.\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "WILDCARD EXAMPLES: \n");
		fprintf(stderr, "\t'a+' : Matches a, aa, aaaaa or really any number of a’s more than one.\n");
		fprintf(stderr, "\t'.+' : Matches any non-empty String.\n");
		fprintf(stderr, "\t'\\\\+' : Matches a string of \\’s.\n");
		fprintf(stderr, "\t'a?b+' : Matches ab, b, abbb or any amount of b following op:onal a.\n");
		fprintf(stderr, "\t'\\?' : A question mark must appear in the line.\n");
		fprintf(stderr, "\t'they?re' : Matches a line that contains either the string \"theyre\" or the string \"there\".\n");
		fprintf(stderr, "\t'h.d..?n' : Matches lines that contain substrings like \"hidden\", \"hidin\", \"hbdwen\", \"hadbn\", etc.\n");
		fprintf(stderr, "\t'cu\\.?t' : Matches lines that either contain the substring \"cut\" or \"cu.t\".\n");
		return 2;
	}
    /* we're not going to worry about long lines */
    char buf[MAXSIZE];

    while (!feof(stdin) && !ferror(stdin)) {
        if (!fgets(buf, sizeof(buf), stdin))
            break;
        if (rgrep_matches(buf, argv[1])) {
            fputs(buf, stdout);
            fflush(stdout);
        }
    }
    if (ferror(stdin)) {
        perror(argv[0]);
        return 1;
    }
    return 0;
}