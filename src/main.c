#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include "dictionary.h"


int main(void)
{
//Виведення правил за необхідністю
    //Змінна для відповіді
    char rules;
    do {
        //Запит та зчитування відповіді
        printf("Do you want to see the rules? (y/n) ");
        scanf(" %c", &rules);
        //В залежності від відповіді відображаються або не відображаються правила
        if (rules == 'y')
        {
            printf("Theme: Choose a dictionary.\n"
                   "You are starting, computer is responding\n"
                   "Uniqueness: Each word must be unique.\n"
                   "End of Game: Continue playing until no new words can be formed.\n"
                   "Winner: The player with the highest number of correct words wins!\n"
                   "If you want to surrender, press Ctrl + D\n"
                   "GLHF\n");
        }
        //Зчитування до введення допустимої відповіді
    } while (rules != 'y' && rules != 'n');

    //Змінні для роботи з директорією
    DIR *dir;
    struct dirent *entry;
    char dictionary[100];
    int count = 0;
    int choice;

    //Відкриваємо директорію в якій знаходиться файл програми
    dir = opendir(".");
    if (dir == NULL) {
        printf("Failed to open directory.\n");
        return 1;
    }

    // Відображаємо та рахуємо всі файли з розширенням .txt
    printf("Available files:\n");
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".txt") != NULL) {
            printf("%d. %s\n", ++count, entry->d_name);
        }
    }

    // Перевірка чи є хоч один файл .txt
    if (count == 0) {
        printf("No .txt files found in the current directory.\n");
        closedir(dir);
        return 1;
    }
    closedir(dir);
    //Запит номеру файлу
    printf("Select a file: ");
    scanf("%d", &choice);

    // Перевірка введеного номеру
    if (count < choice || choice < 1) {
        printf("Invalid file selection.\n");
        return 1;
    }

    // Збереження назви файлу в змінну
    dir = opendir(".");
    count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".txt") != NULL) {
            if (++count == choice) {
                strcpy(dictionary, entry->d_name);
                break;
            }
        }
    }
    closedir(dir);

// Завантажуємо словник
    bool loaded = load(dictionary);

    // Якщо щось пішло не так - виходимо з помилкою
    if (!loaded)
    {
        printf("Could not load dictionary.\n");
        return 1;
    }

//Підготовка змінних для гри

    //Змінна для поточного слова
    char *word = malloc(sizeof(char)*(LENGTH+1));
    word[0] = '\0';

    //Масив з кількістю слів, що починаються на кожну букву
    int* letters = (int*)malloc((N+1) * sizeof(int));
    letters[N] = '\0';
    available_letters(letters);

    //Загальна кількість доступних слів
    int available_words = 0;
    for (int i = 0; i < 28; i++)
    {
        available_words += letters[i];
    }
    if (available_words == 0){
        printf("Dictionary is empty\n");
        return 1;
    }

    //Визначаємо першу літеру
    char current_letter;
    current_letter = select_new_letter(letters, word);
//Початок гри
    printf("Enter the word beginning with the letter %c\n", current_letter);
    //Цикл виконується поки гравець не вирішить здатись, чи всередині циклу не виконається умова для закінчення гри
    while (scanf("%s", word) != EOF && available_words > 0)
    {
    //Хід гравця
        //Перевірка першої літери введеного слова
        if (word[0] == tolower(current_letter)) {
            //Перевірка слова
            if (!check_word(word, dictionary)) {
                continue;
            }

            //Перерахунок слів на випадок, якщо було додано нове слово
            available_letters(letters);
            available_words = 0;
            for (int i = 0; i < 28; i++)
            {
                available_words += letters[i];
            }
            //Перевірка кількості слів
            if (available_words == 0) break;

    //Хід комп'ютера
            //Вибір випадкового слова з доступних
            random_word(word, letters);
            available_words -= 1;

            //Перевірка кількості слів
            if (available_words == 0) break;
            //Вибір нової літери
            available_letters(letters);
            current_letter = select_new_letter(letters, word);

            printf("Enter the word beginning with the letter %c\n", current_letter);
        }
        else printf("Wrong first letter\n");
    }
//Підсумки гри
    //Якщо всі слова були використані, вітаємо гравця з перемогою
    if (available_words == 0)
    {
        printf("GG, all word was used!\n");
    }
    //Якщо ні, сповіщаємо про поразку
    else
    {
        printf("There are non-used words, GL next time\n");
    }

    //Звільняємо пам'ять, що була виділена вручну
    free(word);
    free(letters);
    unload();

    return 0;
}

