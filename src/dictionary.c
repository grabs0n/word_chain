// Функції завантаження словника

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dictionary.h"


// Елемент дерева
typedef struct node
{
    bool is_word;
    bool is_used;
    int correct_child_count;
    struct node *children[N];
}
node;

// Визначення першого елемента дерева
node *root;

// Функція для перетворення літер на їх цифрове значення по-порядку в абетці
int hash(const char character)
{
    switch (character) {
        // Числа 26 та 27 використовуються для одинарної дужки та дефісу
        case '\'':
            return 26;
        case '-':
            return 27;
        default:
            if (isalpha(character))
                return (int)tolower(character) - 97;
            // В разі отримання недопустимого значення повертаємо -1
            else return -1;

    }
}

// Функція для перетворення цифрового значення на символ
char de_hash(int hash)
{
    switch (hash) {
        // Числа 26 та 27 використовуються для одинарної дужки та дефісу
        case 26:
            return '\'';
        case 27:
            return '-';
        default:
        {
            // В разі отримання недопустимого значення повертаємо -1
            if (hash < 0 || hash > 27) return -1;
            else return (char) (hash + 97);
        }
    }
}

//Додавання нового слова до словнику
bool add_word(char word[45]) {
    //Змінна для поточного елементу. Перше значення - корінний елемент
    node *current_node = root;
    //Змінні для перебору слова по літерам
    unsigned int pos = 0;
    unsigned int word_len = strlen(word);
    int cur_hash;
    // Перебір кожної літери
    while (pos < word_len) {
        cur_hash = hash(word[pos]);
        //Перевірка допустимості символу
        if (cur_hash == -1){
            printf("There is a wrong symbol in dictionary, word: \"%s\"\n", word);
            unload();
            return false;
        }
        current_node->correct_child_count++;
        //Перевірка, чи існує вже такий елемент
        if (!current_node->children[cur_hash]) {
            //Якщо ні - ініціалізуємо
            node *child_node = malloc(sizeof(node));
            if (child_node == NULL) {
                unload();
                return false;
            }

            // Заповнюємо нові дочірні елементи значеннями NULL
            for (int i = 0; i < N; i++) {
                child_node->children[i] = NULL;
            }
            //Визначаємо кількість дочірніх елементів, щоб уникнути сміттєвих значень
            child_node->correct_child_count = 0;
            // Переходимо до наступної комірки
            current_node->children[cur_hash] = child_node;
        }
        current_node = current_node->children[cur_hash];
            // Якщо це остання літера, зазначаємо що в цій позиції знаходиться кінець слова
            if (pos == word_len - 1) current_node->is_word = true;
            //Інакше збільшуємо кількість доступних елементів
            else
            {
                current_node->is_word = false;
            }
            current_node->is_used = false;

        //Оновлюємо лічильник позиції
        pos++;
    }
    //Повідомляємо про успішне виконання
    return true;
}

//Функція, що зазначає слово як використане
void markAsUsed (char* word)
{
    // Початкова позиція
    int pos = 0;
    // Довжина слова
    size_t len = strlen(word);
    // Початковий вузол - кореневий вузол
    node* current_node = root;
    // Змінна для поточного хешу
    int current_hash;

    while (pos < len) {
        // Зменшуємо лічильник правильних нащадків поточного вузла
        current_node->correct_child_count--;
        // Обчислюємо хеш поточного символу
        current_hash = hash(word[pos]);
        // Переходимо до наступного вузла згідно хешу
        current_node = current_node->children[current_hash];
        // Переходимо до наступного символу
        ++pos;
    }

    // Позначаємо останній вузол як використаний
    current_node->is_used = true;
}

bool unknownWord(char* word, const char* dictionary) {
    // Виводимо повідомлення про невідоме слово і запитуємо, чи додати його до словника
    printf("Unknown word '%s'. Add it to the dictionary? ", word);
    char answer;
    while (true) {
        // Зчитуємо відповідь користувача
        scanf("%c", &answer);
        if (answer == 'y')
        {
            // Додаємо слово до словника (префіксного дерева)
            add_word(word);
            // Позначаємо слово як використане
            markAsUsed(word);
            // Відкриваємо файл словника для додавання нового слова
            FILE *file = fopen(dictionary, "a");
            // Записуємо слово у файл словника
            fprintf(file, "\n%s", word);
            // Закриваємо файл словника
            fclose(file);
            return true;
        } else if (answer == 'n')
        {
            return false;
        }
        else {
            // Виводимо повідомлення про невірний ввід і очікуємо правильної відповіді
            printf("(y/n)\n");
        }
    }
}

//  Завантажує до пам'яті словник та повертає результат у вигляді True/False
bool load(const char *dictionary)
{
    //Визначення корінного елементу
    root = malloc(sizeof(node));
    if (root == NULL)
    {
        return false;
    }
    root->is_word = false;
    root->is_used = false;
    root->correct_child_count = 0;

    //Заповнюємо всі дочірні елементи пустими значеннями для уникнення сміттєвих значень
    for (int i = 0; i < N; i++)
    {
        root->children[i] = NULL;
    }

    // Відкриття словника
    FILE *file = fopen(dictionary, "r");
    if (file == NULL)
    {
        unload();
        return false;
    }

    // Змінна для поточного слова
    char word[LENGTH];

    // Зчитуємо дані з файлу порядково
    while (fscanf(file, "%s", word) != EOF)
    {
        if (!add_word(word))
            return false;
    }

    // Закриваємо файл словника
    fclose(file);

    // Повертаємо true, щоб підтвердити успіх завантаження
    return true;
}

// Перевіряє слово на його наявність у словнику
bool check_word(char *word, const char* dictionary){
    //Змінні для перебору слова по літерам
    int pos = 0;
    size_t len = strlen(word);
    node *current_node = root;
    int current_hash = hash(word[pos]);

    while (pos < len)
    {
        if (current_hash == -1){
            printf("There is/are unavailable symbol(s)\n");
            return false;
        }
        // Якщо поточний вузол має ініціалізованого нащадка з хешем наступної літери, переходимо до наступного вузла
        if (current_node->children[current_hash]){
            current_node = current_node->children[current_hash];
        }
        // Якщо нащадок такого нащадка не ініціалізовано, викликаємо функцію для невідомого слова
        else
        {
            return unknownWord(word, dictionary);
        }
        //Переходимо до наступної літери
        ++pos;
        current_hash = hash(word[pos]);
    }
    if (pos == len) {
        if (current_node->is_word) {
            if (!current_node->is_used) {
                // Якщо поточний вузол є словом і не використаний, позначаємо його як використаний і повертаємо true
                markAsUsed(word);
                return true;
            } else {
                // Якщо поточне слово вже використане, виводимо повідомлення
                printf("The word is used\n");
                return false;
            }
        } else {
            // Якщо поточний вузол не є словом, викликаємо функцію для невідомого слова
            return unknownWord(word, dictionary);
        }
    }
}


// Вибирає нову літеру для генерації наступного слова
char select_new_letter(const int *letters, const char *word)
{
    char result;
    int r;

    // Якщо слово не порожнє
    if (word[0] != '\0') {
        int len = (int)strlen(word) - 1;
        // Пошук останньої літери у слові, на яку починаються слова
        for (; len >= 0 && letters[hash(word[len])] < 1; len--);
        // Якщо така літера є в слові, повертаємо її
        if (letters[hash(word[len])] > 0) {
            result = word[len];
            return result;
        }
    }

    // Якщо в слові немає правильної літери, або слово порожнє (функція викликається перший раз) обираємо будь-яку
    // доступну літеру
    do {
        r = rand() % 28;
    } while (letters[r] == 0);

    // Повертаємо підходящу літеру, декодуючи її з хешу
    result = de_hash(r);
    return result;
}


// Випадкове слово на основі наявних літер
void random_word(char *word, const int *letters)
{
    // Обираємо доступну першу літеру
    char current_letter = select_new_letter(letters, word);
    int current_hash = hash(current_letter);
    node* current_node = root->children[current_hash];
    int pos = 0;
    //Змінна для зміщення
    int offset = 0;
    while(true)
    {
        word[pos] = current_letter;
        if (current_node->is_word && !current_node->is_used)
        {
            // Якщо в поточному елементі закінчується слово і воно невикористане, закінчуємо формування слова
            word[pos + 1] = '\0';
            break;
        }
        // Генеруємо випадковий хеш
        current_hash = rand() % 28;

        while (true) {
            //Перевіряємо випадковий індекс та якщо він не підходить, збільшуємо зміщення
            if (current_node->children[(current_hash + offset) % 28] != NULL &&
                (current_node->children[(current_hash + offset) % 28]->correct_child_count > 0 ||
                 (current_node->children[(current_hash + offset) % 28]->is_word &&
                  !current_node->children[(current_hash + offset) % 28]->is_used))) {
                current_hash = (current_hash + offset) % 28;
                offset = 0;
                break;
            }

            if (current_node->children[(abs(current_hash - offset)) % 28] != NULL &&
                (current_node->children[(abs(current_hash - offset)) % 28]->correct_child_count > 0 ||
                 (current_node->children[(abs(current_hash - offset)) % 28]->is_word &&
                  !current_node->children[(abs(current_hash - offset)) % 28]->is_used))) {
                current_hash = abs(current_hash - offset) % 28;
                offset = 0;
                break;
            }

            offset++;
        }

        // Переходимо до визначеного елементу
        current_node = current_node->children[current_hash];
        current_letter = de_hash(current_hash);
        pos++;
    }
    // Коли ціле слово знайдено, позначаємо його використаним та виводимо в консоль
    markAsUsed(word);
    printf("Computer move: %s\n", word);
}

void available_letters(int *array)
{
    for (int i = 0; i < N; i++)
    {
        // Якщо дочірній вузол існує, отримуємо кількість правильних дочірніх вузлів
        if (root->children[i]) {
            array[i] = root->children[i]->correct_child_count;
        } else {
            // Якщо дочірній вузол не існує, кількість доступних літер - 0
            array[i] = 0;
        }
    }
}

// Вивантажує з пам'яті вхідний елемент
void unload_current_node(node *current_node)
{
    // Перевіряємо кожен дочірній елемент
    for (int i = 0; i < N; i++)
    {
        // Якщо під дочірній елемент виділено пам'ять, переходимо у нього
        if (current_node->children[i])
        {
            unload_current_node(current_node->children[i]);
        }
    }
    // Коли дійшли до елементу, у якого немає дочірніх елементів, звільняємо пам'ять виділену під нього
    free(current_node);
}

// Вивантажує словник з пам'ять починаючи з корінного елементу
bool unload(void)
{
    unload_current_node(root);
    return true;
}