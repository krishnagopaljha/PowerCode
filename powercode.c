#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_SIZE 256
#define MAX_CODE_SIZE 8192

// Define Python keywords and their replacements
const char *custom_keywords[] = { 
    "False", "None", "True", "and", "as", "assert", "async", "await", "break", 
    "class", "continue", "def", "del", "elif", "else", "except", "finally", "for", 
    "from", "global", "if","input", "import", "in", "is", "lambda", "nonlocal", "not", 
    "or", "pass", "raise", "return", "try", "while", "with", "yield", "print" 
};

const char *python_keywords[] = { 
    "nope", "null", "pwned", "combo", "like", "checkmate", "grindmode", "camp", 
    "respawn", "guild", "skip", "skill", "kill", "elseif", "otherwise", "dodge", 
    "laststand", "quest", "origin", "universe", "queststart","chat", "loot", "within", 
    "equals", "fn", "nolocal", "negate", "either", "do_nothing", "throw", "giveback", 
    "attempt", "until", "using", "produce", "showoff" 
};

// Function to check if a character is part of a word
int is_word_char(char c) {
    return isalnum(c) || c == '_';
}

// Function to replace exact keywords in a line
void replace_keywords(const char *line, char *output) {
    char temp[MAX_LINE_SIZE];
    strcpy(temp, line);

    // Iterate through all keywords
    for (size_t i = 0; i < sizeof(python_keywords) / sizeof(python_keywords[0]); i++) {
        const char *keyword = python_keywords[i];
        const char *replacement = custom_keywords[i];
        char *pos;

        while ((pos = strstr(temp, keyword)) != NULL) {
            // Check if it's a full word match
            int before = (pos == temp) || !is_word_char(*(pos - 1));
            int after = !is_word_char(*(pos + strlen(keyword)));
            if (before && after) {
                // Create a temporary buffer for replacement
                char buffer[MAX_LINE_SIZE];
                strncpy(buffer, temp, pos - temp);  // Copy text before the match
                buffer[pos - temp] = '\0';

                // Add the replacement keyword
                strcat(buffer, replacement);

                // Add the rest of the text after the match
                strcat(buffer, pos + strlen(keyword));

                // Copy the result back into temp
                strcpy(temp, buffer);
            } else {
                break;  // Move on if it's not an exact match
            }
        }
    }

    strcpy(output, temp);  // Copy the final result to the output buffer
}

// Function to replace 'python' with 'PowerCode' and '.py' with '.pcd' in the given string
void replace_in_output(char *output) {
    char temp[MAX_CODE_SIZE];
    strcpy(temp, output);

    // Replace 'python' with 'PowerCode'
    char *pos;
    while ((pos = strstr(temp, "python")) != NULL) {
        memmove(pos + strlen("PowerCode"), pos + strlen("python"), strlen(pos) - strlen("python") + 1);
        memcpy(pos, "PowerCode", strlen("PowerCode"));
    }

    // Replace '.py' with '.pcd'
    while ((pos = strstr(temp, ".py")) != NULL) {
        memmove(pos + strlen(".pcd"), pos + strlen(".py"), strlen(pos) - strlen(".py") + 1);
        memcpy(pos, ".pcd", strlen(".pcd"));
    }

    strcpy(output, temp);  // Copy the final result to the output buffer
}


// Main function
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>.mylang\n", argv[0]);
        return 1;
    }

    // Open the input file
    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Read the contents of the file
    char line[MAX_LINE_SIZE];
    char translated_code[MAX_CODE_SIZE] = "";
    while (fgets(line, sizeof(line), input_file)) {
        // Translate the keywords
        char translated_line[MAX_LINE_SIZE];
        replace_keywords(line, translated_line);
        strcat(translated_code, translated_line);
    }
    fclose(input_file);

    // Write the translated code to a temporary Python file
    FILE *temp_file = fopen("temp.py", "w");
    if (temp_file == NULL) {
        perror("Error creating temporary file");
        return 1;
    }
    fputs(translated_code, temp_file);
    fclose(temp_file);

    // Execute the temporary Python file and capture the output
    FILE *fp = popen("python temp.py 2>&1", "r");
    if (fp == NULL) {
        perror("Error executing Python code");
        printf("\033[0;31merror got\n\033[0m"); // Red color for error
        return 1;
    }

    char output[MAX_CODE_SIZE];
    size_t idx = 0;

    // Read the output line by line and store it
    while (fgets(output + idx, sizeof(output) - idx, fp) != NULL) {
        idx = strlen(output);
    }

    // Close the process
    fclose(fp);

    // Replace 'python' with 'mylang' and '.py' with '.mylang' in the output
    replace_in_output(output);

    // Print the final modified output
    printf("%s", output);

    // Check if there was any error during execution
    if (strlen(output) > 0 && strstr(output, "Traceback") == NULL) {
        printf("\n\033[0;32mMission Accomplished! You did it.\n\033[0m"); // Green color for success
    } else {
        printf("\n\033[0;31mGame Over! Better luck next compile.\n\033[0m"); // Red color for error
    }

    // Clean up the temporary Python file
    remove("temp.py");

    return 0;
}
