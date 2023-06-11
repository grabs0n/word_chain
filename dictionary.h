//Файл заголовків

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdbool.h>

// Максимальна довжина слова
#define LENGTH 45

// Кількість можливих знаків, відповідно кількість можливих дочірніх елементів
// 26 літер, дефіс та апостроф
#define N 28

// Прототипи функцій
bool load(const char *dictionary);
void available_letters(int *array);
bool check_word(char *word, const char* dictionary);
char select_new_letter(const int *letters, const char *word);
bool unload(void);
int hash(char character);
char de_hash(int hash);
void random_word(char *word, const int *letters);

#endif
