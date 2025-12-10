#include <stdio.h>
#include <string.h>

// ============================================================
// PART 1: DFA LEXERS
// ============================================================

#define DFA_STATES 15
#define DEAD -1


// ------------------------------------------------------------
// DFA 1: VARIABLES
// Pattern: _ [letters]+ [digit] [letter]
// ------------------------------------------------------------
// Mapping: _→0, letter→1, digit→2, other→3
int get_input_var(char c) {
    if (c == '_') return 0;
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return 1;
    if (c >= '0' && c <= '9') return 2;
    return 3;
}

// States:
// S0(_)->S1
// S1(letters)->S2
// S2(letters)->S2, digits->S3
// S3(letter)->S4 (accept)
// S4 = accept
int table_var[DFA_STATES][4] = {
    /* S0 */ { 1, DEAD, DEAD, DEAD },
    /* S1 */ { DEAD, 2, DEAD, DEAD },
    /* S2 */ { DEAD, 2,   3, DEAD },
    /* S3 */ { DEAD, 4, DEAD, DEAD },
    /* S4 */ { DEAD, DEAD, DEAD, DEAD }
};


// ------------------------------------------------------------
// DFA 2: FUNCTIONS
// Pattern: [letters]+ F n   (must end in Fn)
// ------------------------------------------------------------
// Mapping: F→0, n→1, other letter→2, other→3
int get_input_func(char c) {
    if (c == 'F') return 0;
    if (c == 'n') return 1;
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return 2;
    return 3;
}

// Corrected transitions:
// S0: only letters → S1
// S1: letters → S1, F → S2
// S2: n → S3 (ACCEPT), F → S2, letters → S1
int table_func[DFA_STATES][4] = {
    /* S0 */ { DEAD, DEAD, 1, DEAD },
    /* S1 */ {    2, DEAD, 1, DEAD },
    /* S2 */ {    2,    3, 1, DEAD },
    /* S3 */ { DEAD, DEAD, DEAD, DEAD }
};


// ------------------------------------------------------------
// DFA 3: LOOP LABELS
// Pattern: loop_ [letters]+ [digit][digit] :
// ------------------------------------------------------------
// char map: l→0, o→1, p→2, _→3, :→4, digit→5, alpha→6, other→7
int get_input_loop(char c) {
    if (c == 'l') return 0;
    if (c == 'o') return 1;
    if (c == 'p') return 2;
    if (c == '_') return 3;
    if (c == ':') return 4;
    if (c >= '0' && c <= '9') return 5;
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return 6;
    return 7;
}

// Fixed transitions
int table_loop[DFA_STATES][8] = {
    /* S0 */ { 1, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD },   // l
    /* S1 */ { DEAD, 2, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD },   // o
    /* S2 */ { DEAD, 3, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD },   // o
    /* S3 */ { DEAD, DEAD, 4, DEAD, DEAD, DEAD, DEAD, DEAD },   // p
    /* S4 */ { DEAD, DEAD, DEAD, 5, DEAD, DEAD, DEAD, DEAD },   // _
    /* S5 */ { DEAD, DEAD, DEAD, DEAD, DEAD, 6, 5, DEAD },      // letters → S5, digits → S6
    /* S6 */ { DEAD, DEAD, DEAD, DEAD, DEAD, 7, DEAD, DEAD },   // 1st digit
    /* S7 */ { DEAD, DEAD, DEAD, DEAD, 8, DEAD, DEAD, DEAD },   // 2nd digit then ':'
    /* S8 */ { DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD } // ACCEPT
};


// ============================================================
// PART 2: LL(1) PARSER
// ============================================================

#define MAXSTACK 200
#define MAXTOK   200
#define MAXSYM   50

char *NT[] = {"E","Eprime","T"};
#define NNT 3

char *TERMINALS[] = {"id","+","$"};
#define NTER 3

char *RHS[] = {
    "T Eprime",     // 1
    "+ T Eprime",   // 2
    "",             // 3 epsilon
    "id"            // 4
};

// LL(1) parse table
int TABLE[NNT][NTER] = {
    {1, 0, 0},   // E
    {0, 2, 3},   // E'
    {4, 0, 0}    // T
};

char stack[MAXSTACK][MAXSYM];
int top = -1;

void push(char *s) { strcpy(stack[++top], s); }
char* pop() { return (top >= 0 ? stack[top--] : NULL); }

void print_stack() {
    printf("[");
    for (int i = top; i >= 0; i--) {
        printf("%s", stack[i]);
        if (i > 0) printf(", ");
    }
    printf("]");
}

int find_nt(char *x){ for(int i=0;i<NNT;i++) if(strcmp(NT[i],x)==0) return i; return -1; }
int find_t(char *x){ for(int i=0;i<NTER;i++) if(strcmp(TERMINALS[i],x)==0) return i; return -1; }

int tokenize(char *line, char tokens[][MAXSYM]) {
    int n = 0;
    char *p = strtok(line, " \t\n");
    while (p) { strcpy(tokens[n++], p); p = strtok(NULL, " \t\n"); }
    if (n == 0 || strcmp(tokens[n-1], "$") != 0) strcpy(tokens[n++], "$");
    return n;
}

void run_parser_demo() {
    printf("\n--- PART 2: LL(1) Parser Demo (showing Stack) ---\n");
    char line[] = "id + id";
    printf("Input string: %s\n", line);

    char input[MAXTOK][MAXSYM];
    int n = tokenize(line, input);

    top = -1;
    push("$");
    push("E");

    int ip = 0;

    printf("%-25s %-10s %-10s %-25s\n", "Stack", "Lookahead", "Top", "Production");
    printf("-------------------------------------------------------------------\n");

    while (top >= 0) {
        char X[MAXSYM];
        strcpy(X, pop());
        char *a = input[ip];

        print_stack();
        printf("%-12s %-10s ", a, X);

        int tindex = find_t(X);

        if (tindex != -1) {
            if (strcmp(X, a) == 0) {
                printf("match\n");
                ip++;
                continue;
            } else {
                printf("REJECTED\n");
                return;
            }
        }

        int ntindex = find_nt(X);
        int aindex = find_t(a);

        if (ntindex == -1 || aindex == -1) { printf("REJECTED\n"); return; }

        int prod = TABLE[ntindex][aindex];
        if (prod == 0) { printf("REJECTED (no rule)\n"); return; }

        if (strlen(RHS[prod-1]) == 0) printf("epsilon\n");
        else printf("%s\n", RHS[prod-1]);

        if (strlen(RHS[prod-1]) > 0) {
            char temp[200];
            strcpy(temp, RHS[prod-1]);
            char *p = strtok(temp, " ");
            char symbols[10][MAXSYM];
            int k = 0;
            while (p) { strcpy(symbols[k++], p); p = strtok(NULL, " "); }
            for (int i = k - 1; i >= 0; i--) push(symbols[i]);
        }
    }

    if (strcmp(input[ip], "$") == 0)
        printf("\nACCEPTED\n");
    else
        printf("\nREJECTED: input not fully consumed\n");
}


// ============================================================
// MAIN
// ============================================================

int main() {

    printf("Enter program code (end with a line containing END):\n");
    char buffer[100];
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
        if (strstr(buffer, "END")) break;
    }

    // VARIABLE TESTS
    printf("\n--- VARIABLE Tests (_letters+[0-9][letter]) ---\n");
    const char *vars[] = {"_temp5x", "_abc9z", "_x1y", "_value", "_val1", "valla"};
    for (int i = 0; i < 6; i++) {
        int state = 0;
        for (int j = 0; vars[i][j]; j++) {
            int inp = get_input_var(vars[i][j]);
            if (state != DEAD) state = table_var[state][inp];
        }
        printf("%-12s -> %s\n", vars[i], (state == 4 ? "VARIABLE" : "Rejected"));
    }


    // FUNCTION TESTS
    printf("\n--- FUNCTION Tests (letters+Fn) ---\n");
    const char *funcs[] = {"computeValueFn", "getFn", "processFn", "xFn", "Fn", "computeFn", "getValue"};
    for (int i = 0; i < 7; i++) {
        int state = 0;
        for (int j = 0; funcs[i][j]; j++) {
            int inp = get_input_func(funcs[i][j]);
            if (state != DEAD) state = table_func[state][inp];
        }
        printf("%-12s -> %s\n", funcs[i], (state == 3 ? "FUNCTION" : "Rejected"));
    }


    // LOOP LABEL TESTS
    printf("\n--- LOOP_LABEL Tests (loop_[letters]+[0-9][0-9]:) ---\n");
    const char *loops[] = {"loop_main01:", "loop_outer99:", "loop_inner00:", "loop_abc1:"};
    for (int i = 0; i < 4; i++) {
        int state = 0;
        for (int j = 0; loops[i][j]; j++) {
            int inp = get_input_loop(loops[i][j]);
            if (state != DEAD) state = table_loop[state][inp];
        }
        printf("%-16s -> %s\n", loops[i], (state == 8 ? "LOOP_LABEL" : "Rejected"));
    }

    // PARSER DEMO
    run_parser_demo();

    return 0;
}
