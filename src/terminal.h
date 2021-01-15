#ifndef TERMINAL_H
#define TERMINAL_H

/**
 * @brief Returns if there is a pending message on the terminal.
 * 
 * @return 1 if there is a pensing message and 0 otherwise.
 */
int keyboard_input();

/**
 * @brief Returns a pending character from the terminal.
 * 
 * @return The pending character.
 */
int get_charactere();

/**
 * @brief Save the pending message.
 * 
 * @param message The pointer that the message should be saved.
 * @param max The max length of the message that should be saved.
 * @return Error code.
 */
int read_inputed_line(char* message, int max);


#endif
