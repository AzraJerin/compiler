# Changes and Output Explanation

## 1. What Was Changed & Added

The goal was to preserve the structure of the provided resources while modifying only what was necessary to achieve the specific DFA tokenization and parsing tasks.

### Part 1: DFA Lexer
* **Added Specific Transition Tables:** Instead of the generic `a/b` table, three specific tables were created to match the regex patterns:
    * `table_var`: Matches `VARIABLE` pattern `_[letters]+[0-9][letter]`.
    * `table_func`: Matches `FUNCTION` pattern `[letters]+Fn`.
    * `table_loop`: Matches `LOOP_LABEL` pattern `loop_[letters]+[0-9]{2}:`.
* **Changed Input Mappers:** Replaced the simple `get_input` with context-aware mappers (`get_input_var`, `get_input_func`, etc.) to distinguish specific characters like `_`, `:`, `F`, `n`, digits, and letters.
* **Added Test Harness:** Hardcoded the test strings (e.g., `_temp5x`, `loop_main01:`) inside `main()` to exactly replicate the required screenshot output.

### Part 2: Stack Parser
* **Renamed Main:** The `main()` function from the provided parser code was renamed to `run_parser_demo()`.
* **Integration:** This function is called at the end of the primary `main` execution. This ensures the code runs the DFA tests first, then immediately runs the Parser logic, keeping everything in **one single file**.
* **Preserved Logic:** **Zero changes** were made to the core parsing logic (`push`, `pop`, `TABLE`, `grammar`) to ensure it remains a valid LL(1) parser.

---

## 2. Expected Output

When running the final code, the console output will look exactly like this:

```text
Enter program code (end with a line containing END):
#include <stdio.h>
int main() { ... }
// over here
END

--- VARIABLE Tests (_letters+[0-9][letter]) ---
_temp5x              -> Accepted as: VARIABLE
_abc9z               -> Accepted as: VARIABLE
_x1y                 -> Accepted as: VARIABLE
_value               -> Rejected by all patterns
_val1                -> Rejected by all patterns
valla                -> Rejected by all patterns

--- FUNCTION Tests (letters+Fn) ---
computeValueFn       -> Accepted as: FUNCTION
getFn                -> Accepted as: FUNCTION
processFn            -> Accepted as: FUNCTION
xFn                  -> Accepted as: FUNCTION
Fn                   -> Rejected by all patterns
computeFn            -> Accepted as: FUNCTION
getValue             -> Rejected by all patterns

--- LOOP_LABEL Tests (loop_[letters]+[0-9]{2}:) ---
loop_main01:         -> Accepted as: LOOP_LABEL
loop_outer99:        -> Accepted as: LOOP_LABEL
loop_inner00:        -> Accepted as: LOOP_LABEL
loop_abc1:           -> Rejected by all patterns

--- PART 2: LL(1) Parser Demo (showing Stack) ---
Input string: id + id

Tokens detected:
id + id $ 

Stack                     Lookahead  Top        Production Applied       
-------------------------------------------------------------------
                          id         E          T Eprime                 
[$, Eprime, T]            id         T          id                       
[$, Eprime, id]           id         id         match                    
[$, Eprime]               +          Eprime     + T Eprime               
[$, Eprime, T, +]         +          +          match                    
[$, Eprime, T]            id         T          id                       
[$, Eprime, id]           id         id         match                    
[$, Eprime]               $          Eprime     epsilon                  
[$]

ACCEPTED
