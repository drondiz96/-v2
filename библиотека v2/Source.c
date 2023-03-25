#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define SUCCESS 0
#define NOT_SUCCESS 1

#define SIZE(ARRAY) (sizeof(ARRAY)/sizeof(ARRAY[0]))
#define N 200

#define true 1

#define INSERT_BOOK(NAME) \
    struct tree_book* tree_##NAME = tree_search(tree_root->##NAME, field_##NAME, new_book);\
    if (tree_##NAME == NULL )/*если не найдено в дереве*/\
        tree_root->##NAME = insert_node(tree_root->##NAME, field_##NAME, new_book);/*не нашли в дереве, добавляем в дерево узел*/\
    else\
        if(comparison(tree_##NAME->ptr_to_list, field_##NAME, new_book) != 0)\
            tree_root->##NAME = insert_node(tree_root->##NAME, field_##NAME, new_book);/*не нашли в дереве, добавляем в дерево узел*/\
        else\
        {/*нашли в дереве, добавлям в список*/\
            struct list_book* new_node_##NAME = (struct list_book*)calloc(1, sizeof(struct list_book)); \
            if (new_node_##NAME == NULL)\
                return NOT_SUCCESS; \
            new_node_##NAME->ptr_to_book = new_book;\
            new_node_##NAME->next_ptr_to_list_node = tree_##NAME->ptr_to_list;\
            tree_##NAME->ptr_to_list = new_node_##NAME;\
        }

#define BOOK_DELETE(NAME)\
    struct tree_book* remove_node_##NAME = tree_search(tree_root->##NAME, field_##NAME, remove_book);\
    if (remove_node_##NAME == NULL)\
        return NOT_SUCCESS;\
    cur_ptr_list = remove_node_##NAME->ptr_to_list;\
    for (struct list_book* parent_cur_node_ptr_list = NULL; cur_ptr_list != NULL; cur_ptr_list = cur_ptr_list->next_ptr_to_list_node)\
    {\
        if (comparison(cur_ptr_list, all_book, remove_book) == 0)\
        {\
            remove_book_in_tree = cur_ptr_list->ptr_to_book;/*сохраняем указатель на книгу*/\
            if (parent_cur_node_ptr_list == NULL && cur_ptr_list->next_ptr_to_list_node == NULL)/*если список состоит из одного элемента*/\
                tree_root->##NAME = delete_node(tree_root->##NAME, field_##NAME, remove_book);/*удаление узла*/\
            else if (parent_cur_node_ptr_list != NULL && cur_ptr_list->next_ptr_to_list_node == NULL)/*если элемент списка является последним элементом*/\
                    parent_cur_node_ptr_list->next_ptr_to_list_node = NULL;\
                else if (parent_cur_node_ptr_list == NULL && cur_ptr_list->next_ptr_to_list_node != NULL)/*если элемент списка является первым элементом*/\
                        remove_node_##NAME->ptr_to_list = cur_ptr_list->next_ptr_to_list_node;\
                    else\
                        parent_cur_node_ptr_list->next_ptr_to_list_node = cur_ptr_list->next_ptr_to_list_node;/*если элемент списка посередине списка*/\
            free(cur_ptr_list);\
            break;/*удаление узла элемента списка*/\
        }\
        parent_cur_node_ptr_list = cur_ptr_list;\
    }

#define CREATE_ROOT_TREE(NAME) \
    tree_root->NAME = (struct tree_book*)calloc(1,sizeof(struct tree_book));\
        if (tree_root->NAME == NULL)\
            return NOT_SUCCESS;\
    struct list_book* new_node_##NAME = (struct list_book*)calloc(1, sizeof(struct list_book));\
    if (new_node_##NAME == NULL)\
        return NOT_SUCCESS;\
    new_node_##NAME->ptr_to_book = new_book;\
    tree_root->NAME->ptr_to_list = new_node_##NAME;

enum field             
{
    field_year = 0,
    field_ozenka,
    field_price,
    field_name_book,
    field_writer,
    field_genre,
    field_publishing_house,
    all_book
};

struct Book
{
    int year;
    float ozenka;
    float price;
    char* name_book;
    char* writer;
    char* genre;
    char* publishing_house;
    char* description;
};

struct Top_tree
{
    struct tree_book* year;
    struct tree_book* ozenka;
    struct tree_book* price;
    struct tree_book* name_book;
    struct tree_book* writer;
    struct tree_book* genre;
    struct tree_book* publishing_house;
};

struct tree_book
{
    struct list_book* ptr_to_list;
    struct tree_book* left;
    struct tree_book* right;
    int height;
};
struct list_book
{
    struct Book* ptr_to_book;
    struct list_book* next_ptr_to_list_node;
};

//основные функции
int book_remove(struct Top_tree*, struct Book*);
int book_add(struct Top_tree*, struct Book*);
int book_edit(struct Top_tree*, struct Book*);
int comparison(struct list_book*, int, struct Book*);
void usage(void);


//функции работающие непосредственно с деревьями
struct tree_book* tree_search(struct tree_book*, int, struct Book*);
struct tree_book* delete_node(struct tree_book*, int, struct Book*);
struct tree_book* insert_node(struct tree_book*, int, struct Book*);
struct tree_book* rotate_right(struct tree_book*);
struct tree_book* rotate_left(struct tree_book*);
struct tree_book* balance(struct tree_book*);
int new_height(struct tree_book*);
int cur_height(struct tree_book*);


//служебные функции(вспомогательные)
int fenter(struct Book*, FILE*);
void enter(struct Book*);
void save_list(struct list_book* , FILE* );
void prnt_list(struct list_book*);
void save_obhod(struct tree_book*, FILE*);
void prnt_obhod(struct tree_book*);


int main(int argc, char* argv[])
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    //setlocale(LC_ALL, "Rus");

    struct Top_tree tree_root = { NULL };
    struct Book* book = NULL;
    
    int i = 1;

    char array[200] = { 0 };
    int sort_field_book = 0;
    int find_field_book = 0;

    int check_EOF = 0;
    int result = 0;

    struct file
    {
        char output[N];
        char input[N];
    };
    struct file name = { 0 };

    while (i < argc)
    {
        check_EOF = scanf("%s", argv[i]);

        if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--print"))
        {
            prnt_obhod(*(&tree_root.year + sort_field_book));
            puts("");
            i++;
            continue;
        }

        if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--add"))
        {
            book = (struct Book*)calloc(1, sizeof(struct Book));
            enter(book);
            result = book_add(&tree_root, book);
            book = NULL;
            puts((result == SUCCESS) ? "Книга успешно добавлена" : "Книга не добавлена");
            i++;
            continue;
        }

        if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--remove "))
        {
            book = (struct Book*)calloc(1, sizeof(struct Book));
            enter(book);
            result = book_remove(&tree_root, book);
            free(book);
            puts((result == SUCCESS) ? "Книга успешно удалена" : "Книга не удалена");
            i++;
            continue;
        }

        if (!strcmp(argv[i], "-e") || !strcmp(argv[i], "--edit "))
        {
            puts("Изменение книги:");
            book = (struct Book*)calloc(1, sizeof(struct Book));
            enter(book);
            result = book_edit(&tree_root, book);
            book = NULL;
            puts((result == SUCCESS) ? "Книга успешно изменена" : "Книга не изменена");
            i++;
            continue;
        }

        if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--find"))
        {
            book = (struct Book*)calloc(1, sizeof(struct Book));
            printf("По какому полю в хотите отсортировать?\n"
                "0 - по году издания\n"
                "1 - по оценке\n"
                "2 - по цене\n"
                "3 - по названию книги\n"
                "4 - по автору книги\n"
                "5 - по жанру\n"
                "6 - по издательству\n"
                "Введите номер поля: ");
            check_EOF = scanf("%d", &find_field_book);
            printf("Введите содержимое поля, по которому хотите найти: ");
            struct tree_book* node = NULL;
            if (find_field_book > field_ozenka)
            {
                char string[N] = { 0 };
                gets_s(string, N);
                *(&(book->name_book) + find_field_book - field_name_book) = (char*)calloc(strlen(string), sizeof(char));
                strcpy(*(&(book->name_book) + find_field_book - field_name_book), string);
                node = tree_search(*(&tree_root.year + find_field_book), find_field_book, book);
            }
            else
            {
                float number = 0.0;
                check_EOF = scanf("%f", &number);
                (*(&(book->year) + find_field_book)) = number;
                node = tree_search(*(&tree_root.year + find_field_book), find_field_book, book);
            }
            free(book);
            if (node == NULL)
                puts("Книги/книги не найдены.");
            else
                prnt_list(node->ptr_to_list);
            i++;
            continue;
        }

        if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--sort"))
        {
            printf("По какому полю в хотите отсортировать?\n"
                "0 - по году издания\n"
                "1 - по оценке\n"
                "2 - по цене\n"
                "3 - по названию книги\n"
                "4 - по автору книги\n"
                "5 - по жанру\n"
                "6 - по издательству\n"
                "Введите номер поля: ");
            check_EOF = scanf("%d", &sort_field_book);
            i++;
            continue;
        }

        if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--save"))
        {
            FILE* fp_input = fopen(name.output, "w");
            save_obhod(tree_root.year, fp_input);
            fclose(fp_input);
            i++;
            continue;
        }

        if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output"))
        {
            printf("Введите имя выходного файла: ");
            check_EOF = scanf("%s", name.output);
            i++;
            continue;
        }

        if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--input"))
        {
            printf("Введите имя входного файла: ");
            check_EOF = scanf("%s", name.input);
            i++;
            continue;
        }

        if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--download"))
        {
            if (name.output[0] = '\0')
                strcpy(name.output, name.input);
            FILE* fp_output = fopen(name.output, "r");
            while (true)
            {
                book = (struct Book*)calloc(1, sizeof(struct Book));
                result = fenter(book, fp_output);
                if (result == EOF)
                {
                    free(book);
                    break;
                }
                book_add(&tree_root, book);
            }
            fclose(fp_output);
            i++;
            continue;
        }

        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
        {
            usage();
            i++;
            continue;
        }

        usage();
    }

    while (true)
    {
        printf("введите команду: ");
        check_EOF = scanf("%s", array);

        if (!strcmp(array, "-p") || !strcmp(array, "--print"))
        {
            prnt_obhod(*(&tree_root.year + sort_field_book));
            puts("");
            return;
        }

        if (!strcmp(array, "-a") || !strcmp(array, "--add"))
        {
            book = (struct Book*)calloc(1, sizeof(struct Book));
            enter(book);
            result = book_add(&tree_root, book);
            book = NULL;
            puts((result == SUCCESS) ? "Книга успешно добавлена" : "Книга не добавлена");
            continue;
        }

        if (!strcmp(array, "-r") || !strcmp(array, "--remove"))
        {
            book = (struct Book*)calloc(1, sizeof(struct Book));
            enter(book);
            result = book_remove(&tree_root, book);
            free(book);
            puts((result == SUCCESS) ? "Книга успешно удалена" : "Книга не удалена");
            continue;
        }

        if (!strcmp(array, "-e") || !strcmp(array, "--edit"))
        {
            puts("Изменение книги:");
            book = (struct Book*)calloc(1, sizeof(struct Book));
            enter(book);
            result = book_edit(&tree_root, book);
            book = NULL;
            puts((result == SUCCESS) ? "Книга успешно изменена" : "Книга не изменена");
            continue;
        }

        if (!strcmp(array, "-f") || !strcmp(array, "--find"))
        {
            book = (struct Book*)calloc(1, sizeof(struct Book));
            if (book == NULL)
                exit(-1);
            printf("По какому полю хотите начать поиск?\n"
                "0 - по году издания\n"
                "1 - по оценке\n"
                "2 - по цене\n"
                "3 - по названию книги\n"
                "4 - по автору книги\n"
                "5 - по жанру\n"
                "6 - по издательству\n"
                "Введите номер поля: ");
            check_EOF = scanf("%d", &find_field_book);
            printf("Введите содержимое поля, по которому хотите найти: ");
            struct tree_book* node = NULL;
            if (find_field_book > field_ozenka)
            {
                char string[N] = { 0 };
                gets_s(string, N);
                *(&(book->name_book) + find_field_book - field_name_book) = (char*)calloc(strlen(string), sizeof(char));
                strcpy(*(&(book->name_book) + find_field_book - field_name_book), string);
                node = tree_search(*(&tree_root.year + find_field_book), find_field_book, book);
            }
            else
            {
                float number = 0.0;
                check_EOF = scanf("%f", &number);
                (*(&(book->year) + find_field_book)) = number;
                node = tree_search(*(&tree_root.year + find_field_book), find_field_book, book);
            }
            free(book);
            if (node == NULL)
                printf("Книги/книги не найдены.");
            else
                prnt_list(node->ptr_to_list);
            puts("");
            continue;
        }

        if (!strcmp(array, "-s") || !strcmp(array, "--sort"))
        {
            printf( "По какому полю в хотите отсортировать?\n"
                    "0 - по году издания\n"
                    "1 - по оценке\n"
                    "2 - по цене\n"
                    "3 - по названию книги\n"
                    "4 - по автору книги\n"
                    "5 - по жанру\n"
                    "6 - по издательству\n"
                    "Введите номер поля: ");
            check_EOF = scanf("%d", &sort_field_book);
            continue;
        }

        if (!strcmp(array, "-v") || !strcmp(array, "--save"))
        {
            if (name.output[0] == '\0')
                strcpy(name.output, name.input);
            FILE* fp_input = fopen(name.output, "w");
            save_obhod(tree_root.year, fp_input);
            fclose(fp_input);
            continue;
        }

        if (!strcmp(array, "-o") || !strcmp(array, "--output"))
        {
            printf("Введите имя выходного файла: ");
            check_EOF = scanf("%s", name.output);
            continue;
        }

        if (!strcmp(array, "-i") || !strcmp(array, "--input"))
        {
            printf("Введите имя входного файла: ");
            check_EOF = scanf("%s", name.input);
            continue;
        }

        if (!strcmp(array, "-d") || !strcmp(array, "--download"))
        {
            FILE* fp_output = fopen(name.input, "r");
            while (true)
            {
                book = (struct Book*)calloc(1, sizeof(struct Book));
                result = fenter(book, fp_output);
                if (result == EOF)
                {
                    free(book);
                    break;
                }
                else
                    book_add(&tree_root, book);
                
            }
            fclose(fp_output);
            continue;
        }

        if (!strcmp(array, "-h") || !strcmp(array, "--help"))
        {
            usage();
            continue;
        }

        usage();
    }

    
    return 0;
}

void usage(void)
{
    printf( "-h/--help — ключ для вывода справки.\n"
            "-p/--print — ключ для вывода информации книгах, после вывода программа завершает работу.\n"
            "-a/--add — ключ для добавления книги.\n"
            "-r/--remove — ключ для удаления книги.\n"
            "-e/--edit — ключ редактирования книги.\n"
            "-f/--find — ключ для поиска книги по заданному критерию.\n"
            "-s/--sort — ключ для сортировки по заданному полю.\n"

            "-i/--input — ключ для передачи пути к входному файлу базы данных.\n"
            "-o/--output — ключ для передачи пути к выходному файлу базы данных.\n"
            "-v/--save — ключ для сохранения в файл\n"
            "-d/--download — ключ для импорта данных в программу из файла.\n");
}

int fenter(struct Book* new_book, FILE* fp_output)
{
    int check_EOF = 0;
    char buffer[N] = { 0 };
    check_EOF = fscanf(fp_output, "%d%f%f", &new_book->year, &new_book->ozenka, &new_book->price);
    if (check_EOF == EOF)
        return EOF;
    check_EOF = fgetc(fp_output);//поглощение \n
    fgets(buffer, N, fp_output);
    new_book->name_book = (char*)calloc(strlen(buffer),sizeof(char));
    if (new_book->name_book == NULL)
        return NOT_SUCCESS;
    memcpy(new_book->name_book, buffer, strlen(buffer) - 1);
    
    fgets(buffer, N, fp_output);
    new_book->writer = (char*)calloc(strlen(buffer), sizeof(char));
    if (new_book->writer == NULL)
        return NOT_SUCCESS;
    memcpy(new_book->writer, buffer, strlen(buffer) - 1);

    fgets(buffer, N, fp_output);
    new_book->publishing_house = (char*)calloc(strlen(buffer), sizeof(char));
    if (new_book->publishing_house == NULL)
        return NOT_SUCCESS;
    memcpy(new_book->publishing_house, buffer, strlen(buffer) - 1);

    fgets(buffer, N, fp_output);
    new_book->genre = (char*)calloc(strlen(buffer), sizeof(char));
    if (new_book->genre == NULL)
        return NOT_SUCCESS;
    memcpy(new_book->genre, buffer, strlen(buffer) - 1);

    fgets(buffer, N, fp_output);
    new_book->description = (char*)calloc(strlen(buffer), sizeof(char));
    if (new_book->description == NULL)
        return NOT_SUCCESS;
    memcpy(new_book->description, buffer, strlen(buffer) - 1);
    return SUCCESS;
}

void enter(struct Book* new_book)
{
    
    if (new_book == NULL)
        exit(-1);
    //заполнение данных о книге
    int check = 0;
    char string[N] = { 0 };
    check = getchar();
    printf("Введите название книги: ");
    gets_s(string, N);
    new_book->name_book = (char*)calloc(strlen(string) + 1, sizeof(char));
    if (new_book->name_book == NULL)
        exit(-1);
    strcpy(new_book->name_book, string);

    printf("Введите автора книги: ");
    gets_s(string, N);
    new_book->writer = (char*)calloc(strlen(string) + 1, sizeof(char));
    if (new_book->writer == NULL)
        exit(-1);
    strcpy(new_book->writer, string);

    printf("Введите год издания книги: ");
    check = scanf("%d", &new_book->year);
    check = getchar();//чтобы поглотить enter или space

    printf("Введите издательство книги: ");
    gets_s(string, N);
    new_book->publishing_house = (char*)calloc(strlen(string) + 1, sizeof(char));
    if (new_book->publishing_house == NULL)
        exit(-1);
    strcpy(new_book->publishing_house, string);

    printf("Введите оценку книги: ");
    check = scanf("%f", &new_book->ozenka);
    check = getchar();//чтобы поглотить enter или space

    printf("Введите цену книги: ");
    check = scanf("%f", &new_book->price);
    check = getchar();//чтобы поглотить enter или space

    printf("Введите жанр книги: ");
    gets_s(string, N);
    new_book->genre = (char*)calloc(strlen(string) + 1, sizeof(char));
    if (new_book->genre == NULL)
        exit(-1);
    strcpy(new_book->genre, string);

    printf("Введите описание книги: ");
    
    gets_s(string, N);
    new_book->description = (char*)calloc(strlen(string) + 1, sizeof(char));
    if (new_book->description == NULL)
        exit(-1);
    strcpy(new_book->description, string);
    puts("");
}

void prnt_obhod(struct tree_book* top_node)
{
    if (top_node != NULL)
    {
        prnt_obhod(top_node->left);
        prnt_list(top_node->ptr_to_list);
        prnt_obhod(top_node->right);
    }
}

void prnt_list(struct list_book* start_ptr_list)
{
    puts("");
    for (; start_ptr_list; start_ptr_list = start_ptr_list->next_ptr_to_list_node)
    {
        printf("Название книги: "); puts(start_ptr_list->ptr_to_book->name_book);
        printf("Автор/Авторы книги: "); puts(start_ptr_list->ptr_to_book->writer);
        printf("Год выпуска книги: %d\n", start_ptr_list->ptr_to_book->year);
        printf("Издательство книги: "); puts(start_ptr_list->ptr_to_book->publishing_house);
        printf("Оценка читателей книги: %.2f\n", start_ptr_list->ptr_to_book->ozenka);
        printf("Цена книги: %.2f\n", start_ptr_list->ptr_to_book->price);
        printf("Жанр книги: "); puts(start_ptr_list->ptr_to_book->genre);
        printf("Краткое описание книги: "); puts(start_ptr_list->ptr_to_book->description);
    }
}

void save_obhod(struct tree_book* top_node, FILE* fp_input)
{
    if (top_node != NULL)
    {
        save_obhod(top_node->left, fp_input);
        save_list(top_node->ptr_to_list, fp_input);
        save_obhod(top_node->right, fp_input);
    }
}

void save_list(struct list_book* top_list_node, FILE* fp_input)
{
    int check_EOF = 0;
    for (; top_list_node; top_list_node = top_list_node->next_ptr_to_list_node)
    {
        check_EOF = fprintf(fp_input, "%d %f %f\n", top_list_node->ptr_to_book->year, top_list_node->ptr_to_book->ozenka, top_list_node->ptr_to_book->price);

        fputs(top_list_node->ptr_to_book->name_book, fp_input); check_EOF = fprintf(fp_input, "\n");
        fputs(top_list_node->ptr_to_book->writer, fp_input); check_EOF = fprintf(fp_input, "\n");
        fputs(top_list_node->ptr_to_book->publishing_house, fp_input); check_EOF = fprintf(fp_input, "\n");
        fputs(top_list_node->ptr_to_book->genre, fp_input); check_EOF = fprintf(fp_input, "\n");
        fputs(top_list_node->ptr_to_book->description, fp_input); check_EOF = fprintf(fp_input, "\n");
    }
}

int book_edit(struct Top_tree* tree_root, struct Book* change_node)
{
    int result = 0;

    result = book_remove(tree_root, change_node);
    if (result == NOT_SUCCESS)
        return NOT_SUCCESS;

    char string[N] = { 0 };
    
    puts("Изменение полей книги");

    puts("Хотите изменить название книги? [y/n]");
    if (getchar() == 'y')
    {
        result = getchar();
        printf("Введите новое имя книги: ");
        gets_s(string, N);
        free(change_node->name_book);
        change_node->name_book = (char*)calloc(strlen(string) + 1, sizeof(char));
        if (change_node->name_book == NULL)
            exit(-1);
        strcpy(change_node->name_book, string);
    }
    else
        result = getchar();

    puts("Хотите изменить имя автора/авторов книги? [y/n]");
    if (getchar() == 'y')
    {
        result = getchar();
        printf("Введите новое имя автора/авторов книги: ");
        gets_s(string, N);
        free(change_node->writer);
        change_node->writer = (char*)calloc(strlen(string) + 1, sizeof(char));
        if (change_node->writer == NULL)
            exit(-1);
        strcpy(change_node->writer, string);
    }
    else
        result = getchar();

    puts("Хотите изменить год издания книги? [y/n]");
    if (getchar() == 'y')
    {
        result = getchar();

        printf("Введите новый год издания книги: ");
        result = scanf("%d", &change_node->year);
    }
    else
        result = getchar();

    puts("Хотите изменить издательство книги? [y/n]");
    if (getchar() == 'y')
    {
        result = getchar();
        printf("Введите новое издательство книги: ");
        gets_s(string, N);
        free(change_node->publishing_house);
        change_node->publishing_house = (char*)calloc(strlen(string) + 1, sizeof(char));
        if (change_node->publishing_house == NULL)
            exit(-1);
        strcpy(change_node->publishing_house, string);
    }
    else 
        result = getchar();

    puts("Хотите изменить жанр книги? [y/n]");
    if (getchar() == 'y')
    {
        result = getchar();
        printf("Введите новый жанр книги: ");
        gets_s(string, N);
        free(change_node->genre);
        change_node->genre = (char*)calloc(strlen(string) + 1, sizeof(char));
        if (change_node->genre == NULL)
            exit(-1);
        strcpy(change_node->genre, string);
    }
    else 
        result = getchar();

    puts("Хотите изменить описание книги? [y/n]");
    if (getchar() == 'y')
    {
        result = getchar();
        printf("Введите новое имя книги: ");
        gets_s(string, N);
        free(change_node->description);
        change_node->description = (char*)calloc(strlen(string) + 1, sizeof(char));
        if (change_node->description == NULL)
            exit(-1);
        strcpy(change_node->description, string);
    }
    else
        result = getchar();

    puts("Хотите изменить цену книги? [y/n]");
    if (getchar() == 'y')
    {
        result = getchar();
        printf("Введите новую цену книги: ");
        result = scanf("%f", &change_node->price);
    }
    else
        result = getchar();

    puts("Хотите изменить оценку книги? [y/n]");
    if (getchar() == 'y')
    {
        result = getchar();
        printf("Введите новую оценку книги: ");
        result = scanf("%f", &change_node->ozenka);
    }
    else
        result = getchar();


    result = book_add(tree_root, change_node);
    if (result == NOT_SUCCESS)
        return NOT_SUCCESS;
    return SUCCESS;
}

int book_add(struct Top_tree* tree_root, struct Book* new_book)
{
    if (tree_root->year == NULL)
    {
        CREATE_ROOT_TREE(year)
        CREATE_ROOT_TREE(ozenka)
        CREATE_ROOT_TREE(price)
        CREATE_ROOT_TREE(name_book)
        CREATE_ROOT_TREE(writer)
        CREATE_ROOT_TREE(genre)
        CREATE_ROOT_TREE(publishing_house)
        return SUCCESS;
    }
    INSERT_BOOK(year)
    INSERT_BOOK(ozenka)
    INSERT_BOOK(price)
    INSERT_BOOK(name_book)
    INSERT_BOOK(writer)
    INSERT_BOOK(genre)
    INSERT_BOOK(publishing_house)
    return SUCCESS;
}

int book_remove(struct Top_tree* tree_root, struct Book* remove_book)
{
    struct list_book* cur_ptr_list = NULL;
    struct Book* remove_book_in_tree = NULL;
    BOOK_DELETE(year)
    BOOK_DELETE(ozenka)
    BOOK_DELETE(price)
    BOOK_DELETE(name_book)
    BOOK_DELETE(writer)
    BOOK_DELETE(genre)
    BOOK_DELETE(publishing_house)
    free(remove_book_in_tree);
    return SUCCESS;
}

struct tree_book* delete_node(struct tree_book* remove_node, int field, struct Book* book)
{
    if (remove_node == NULL)
        return remove_node;
    if (comparison(remove_node->ptr_to_list, field, book) == 0)
    {
        if (remove_node->left == NULL && remove_node->right == NULL)
        {
            free(remove_node);
            return NULL;
        }
        struct tree_book* temp = NULL;
        if (remove_node->left == NULL || remove_node->right == NULL)
        {
            temp = (remove_node->left == NULL) ? remove_node->right : remove_node->left;
            free(remove_node);
            return balance(temp);
        }

        for (temp = remove_node->right; temp && temp->left; temp = temp->left);
        remove_node->ptr_to_list = temp->ptr_to_list;
        remove_node->right = delete_node(remove_node->right, field, temp->ptr_to_list->ptr_to_book);
    }
    else if (comparison(remove_node->ptr_to_list, field, book) > 0)
            remove_node->right = delete_node(remove_node->right, field, book);
        else 
            remove_node->left = delete_node(remove_node->left, field, book);
    return balance(remove_node);
}

struct tree_book* insert_node(struct tree_book* top_node, int field, struct Book* book)
{
    if (top_node == NULL)
    {
        top_node = (struct tree_book*)calloc(1, sizeof(struct tree_book));
        if (top_node == NULL)
            exit(-1);
        top_node->ptr_to_list = (struct list_book*)calloc(1, sizeof(struct list_book));
        if (top_node->ptr_to_list == NULL)
            exit(-1);
        top_node->ptr_to_list->ptr_to_book = book;
        top_node->height = 1;
        return top_node;
    }

    if (comparison(top_node->ptr_to_list, field, book) > 0)
        top_node->right = insert_node(top_node->right, field, book);
    else
        top_node->left = insert_node(top_node->left, field, book);
    return balance(top_node);
}

struct tree_book* balance(struct tree_book* top_node)
{
    top_node->height = new_height(top_node);
    int balance = cur_height(top_node->right) - cur_height(top_node->left);
    if (balance == 2)//если правый выше левого
    {
        int balance_right = cur_height(top_node->right->right) - cur_height(top_node->right->left);
        if (balance_right == -1)
            top_node->right = rotate_right(top_node->right);//правый поворот
        return rotate_left(top_node);//левый поворт
    }
    if (balance == -2)
    {
        int balance_left = cur_height(top_node->left->right) - cur_height(top_node->left->left);
        if (balance_left == 1)
            top_node->left = rotate_left(top_node->left);//левый поворот
        return rotate_right(top_node);//правый поворт
    }
    return top_node;
}

struct tree_book* rotate_right(struct tree_book* tree_top)
{
    struct tree_book* new_tree_top = tree_top->left;
    tree_top->left = new_tree_top->right;
    new_tree_top->right = tree_top;
    tree_top->height = new_height(tree_top);
    new_tree_top->height = new_height(new_tree_top);
    return new_tree_top;
}

struct tree_book* rotate_left(struct tree_book* tree_top)
{
    struct tree_book* new_tree_top = tree_top->right;
    tree_top->right = new_tree_top->left;
    new_tree_top->left = tree_top;
    tree_top->height = new_height(tree_top);
    new_tree_top->height = new_height(new_tree_top);
    return new_tree_top;
}

int new_height(struct tree_book* tree_top)
{
    int height_right = cur_height(tree_top->right);
    int height_left = cur_height(tree_top->left);
    return 1 + max(height_right, height_left);
}

int cur_height(struct tree_book* tree_top)
{
    return (tree_top) ? tree_top->height : 0;
}

struct tree_book* tree_search(struct tree_book* tree_top, int field, struct Book* book)
{
    if (tree_top == NULL)
        return tree_top;

    if(comparison(tree_top->ptr_to_list, field, book) == 0)
        return tree_top;

    if (comparison(tree_top->ptr_to_list, field, book) > 0)
        return tree_search(tree_top->right, field, book);
    else 
        return tree_search(tree_top->left, field, book);
}

int comparison(struct list_book* node_list, int field, struct Book* book)
{
    if (field < all_book)
        if (field > field_price)
            return strcmp(*(&(book->name_book) + field - field_name_book), *(&(node_list->ptr_to_book->name_book) + field - field_name_book));
        else if (*(&(book->year) + field) == *(&(node_list->ptr_to_book->year) + field))
                return 0;
            else if (*(&(book->year) + field) > *(&(node_list->ptr_to_book->year) + field))
                    return 1;
                else
                    return -1;
    else if (node_list->ptr_to_book->price == book->price &&
            node_list->ptr_to_book->ozenka == book->ozenka &&
            node_list->ptr_to_book->year == book->year &&
            strcmp(node_list->ptr_to_book->name_book, book->name_book) == 0 &&
            strcmp(node_list->ptr_to_book->writer, book->writer) == 0 &&
            strcmp(node_list->ptr_to_book->publishing_house, book->publishing_house) == 0 &&
            strcmp(node_list->ptr_to_book->description, book->description) == 0)
            return 0;
        else
            return 1;
}

/*
-a
Мастер и Маргарита
Михаил Афанасьевич Булгаков
1929
Школьная библиотека
10
1999
роман
Мастер знает пять языков и написал роман о Понтии Пилате - том самом, который отправил на смерть Иешуа
-e
Мастер и Маргарита
Михаил Афанасьевич Булгаков
1929
Школьная библиотека
10
1999
роман
Мастер знает пять языков и написал роман о Понтии Пилате - том самом, который отправил на смерть Иешуа
n
n
n
y

-o
output.txt
-i
output.txt
*/