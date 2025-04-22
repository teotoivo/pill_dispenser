# Style Guide


## Important
* DO NOT use magic numbers random numbers in code have to have a constant variable or a comment exlaining it

## Naming Conventions:

* Functions: Use snake_case for function names (e.g., dispense_pill()).

* Variables: Use snake_case for variables (e.g., num_pills).

* Constants: Use UPPER_SNAKE_CASE for constants (e.g., MAX_PILLS).

* Structs: Use CamelCase for struct names (e.g., PillDispenser).

* Files: Use snake_case for file names (e.g., dispenser.c, lora.c).


## Comments
* use block comments for fucntion documentation
```c 
/*
 * Function to dispense a pill
 * This function interacts with the dispenser motor and checks
 * the piezo sensor to confirm a pill was dispensed
 */
void dispense_pill() {
    // Code logic here
}
```
* Use line comments for inline explanations
```c
int num_pills = 7;  // Number of pills to be dispensed
```
