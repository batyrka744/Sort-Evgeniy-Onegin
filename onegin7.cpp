#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>


#define BLACK "\x1b[30m"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define WHITE "\x1b[37m"
#define RESET "\x1b[0m"


#define my_assert(exp, ret) if(!(exp)) {\
    printf(YELLOW "%s:%d: [Assertation failed: %s, file %s, line %d]\n" RESET, __FILE__, __LINE__, #exp,  __FILE__, __LINE__);\
    return ret;}    


#define INPUT "Onegin_full.txt"
#define OUTPUT "output.txt"


typedef struct {char* str; size_t len;} Line;

//-----------------------------------------------------FUNCTION DECLATRATIONS---------------------------------------------

char* ReadFromFile(const char* name, size_t* out_size);

ssize_t PrintStrings(const char* reason, FILE* output, Line* index,  size_t nLines);

void ChangeValues(void* value1, void* value2, size_t size_elem);

void Bubble_Sort(void* const data, size_t size, size_t size_elem, int (*CompareFunc)(const void* a, const void* b));

int Compare_by_beginning(const void* elem_a, const void* elem_b);

int Compare_by_end(const void* elem_a, const void* elem_b);

int str_cmp_beginning(const Line* str1, const Line* str2);

int str_cmp_end(const Line* str1, const Line* str2);

Line* SplitLines(char* buffer, size_t file_size, size_t* nLines);

int HasLetters(const char* s);

void clean_memory(Line* index, Line* start_index, char* buffer);

void print_links(ssize_t line_beg, ssize_t line_end, ssize_t line_orig);

//----------------------------------------------------------MAIN---------------------------------------------------------------------
//8
// .h file // -> -1
int main(void) {

    FILE* output = fopen(OUTPUT, "wb");
    assert(output != NULL);

    size_t file_size = 0;

    char* buffer = ReadFromFile(INPUT, &file_size); // argc argv // define  //+
    assert(buffer != NULL);

    size_t nLines = 0;

    Line* index = SplitLines(buffer, file_size, &nLines);
    Line* start_index = (Line*)calloc(nLines, sizeof(Line));  // assert  //+
    assert(start_index != NULL);

    memcpy(start_index, index, nLines * sizeof(Line)); // why bad???  
    //не проверяет соответствие размеров выходного буфера и копируемых байт что может привести к переполнению буфера 
    // можно либо for либо структуру либо мб свою функцию

    ssize_t current_line = 1;


    ssize_t line_beg = current_line;
    Bubble_Sort(index, nLines, sizeof(Line), &Compare_by_beginning);
    current_line += PrintStrings("SORTED BY THE BEGINNING", output, index, nLines);

    ssize_t line_end = current_line;
    qsort(index, nLines, sizeof(Line), &Compare_by_end);
    current_line += PrintStrings("SORTED BY THE END", output, index, nLines);

    ssize_t line_orig = current_line;
    PrintStrings("ORIGINAL", output, start_index, nLines);

    fclose(output);


    clean_memory(index, start_index, buffer);

    print_links(line_beg, line_end, line_orig);


    return 0;
}

//----------------------------------------------------FUNCTIONS---------------------------------------------------------------------

/**
 * @brief reads file and puts text in the buffer
 * 
 * @param[in] name name of file with text
 * @param[out] out_size the real number of bytes
 * @return char* that need to free -- dynamic memory
 */
char* ReadFromFile(const char* name, size_t* out_size) {

    my_assert(out_size != NULL, NULL);
    my_assert(name != NULL, 0);

    FILE* file = fopen(name, "rb");
    my_assert(file != NULL, 0);

    // get sixe of file
    struct stat file_info = {0};
    int fd = fileno(file);
    my_assert(fstat(fd, &file_info) == 0, NULL);
    size_t file_size = (size_t)file_info.st_size;


    char* buffer = (char*)calloc(file_size + 5, sizeof(char));
    my_assert(buffer != NULL, NULL);

    *out_size = fread(buffer, sizeof(char), file_size , file);
    buffer[*out_size] = '\0';
     
    //  ???   // убрал переменную read_bytes +

    fclose(file);

    return buffer;
}

/**
 * @brief splits buffer to strings and puts pointers to them in array
 * 
 * @param[in] buffer array with text
 * @param[in] file_size size of file with text
 * @param[in, out] nLines number of the strings
 * @return Line* array with pointers to the strings and their length(needs to free)
 */
Line* SplitLines(char* buffer, size_t file_size, size_t* nLines) {

    my_assert(file_size != 0, NULL);
    my_assert(buffer != NULL, NULL); 


    Line* index = (Line*)calloc(file_size, sizeof(Line));
    
    size_t str_num = 1;   //new peremennaya //+
    index[0].str = &buffer[0];

    
    for (size_t i = 0; i < file_size; i++) { // standart need sizet  //+

        if (buffer[i] == '\n'  && i < file_size - 1) {
            buffer[i] = '\0';

            index[str_num - 1].len = (size_t)(&buffer[i] - index[str_num - 1].str);
            index[str_num++].str = &buffer[i + 1];
        }
    }
    index[str_num - 1].len = (size_t)(&buffer[file_size] - index[str_num - 1].str);

    *nLines = str_num;

    return index;
}

/**
 * @brief puts text into the file
 * 
 * @param[in] reason reason of print
 * @param[in] name pointer of opened file
 * @param[in] index array with pointers on the strings and their length
 * @param[in] nLines number of the strings
 * @return ssize_t number of the writed strings
 */
ssize_t PrintStrings(const char* reason, FILE* output, Line* index,  size_t nLines) {

    const char* extra_enters = "\n\n\n\n\n\n";
    size_t len_enters = strlen(extra_enters);

    my_assert(reason != NULL, -1);
    assert(index != NULL);

    size_t printed_lines = 0;

     // optimize
    

    fputs(reason, output);
    fputs(extra_enters, output);  // fprintf // hz fputs lutsche
    printed_lines += len_enters; // magic number //+

    for (size_t i = 0; i < nLines; i++) {  // size_t //+
        assert(index[i].str != NULL);

        if (!HasLetters(index[i].str)) continue;

        printed_lines++;
        fputs(index[i].str, output);
        fputc('\n', output);
    }

    fputs(extra_enters, output);
    printed_lines += len_enters;



    return printed_lines;
}

/**
 * @brief sorts array
 * 
 * @param[in, out] data array
 * @param[in] size size of array(number of elements)
 * @param[in] size_elem size of one element
 * @param[in] CompareFunc a pointer to the function that will be used for sorting
 */
void Bubble_Sort(void* const data, size_t size, size_t size_elem, int (*CompareFunc)(const void* a, const void* b)) {
  
    my_assert(data != NULL,);

    for (size_t n = 0; n < size; n++) {

        size_t change_flag = 0; 

        for (size_t i = 0; i < size - 1; i++) {


            void* elem1 = (char*)data + i*size_elem;
            void* elem2 = (char*)data + (i+1)*size_elem; 

            if ((*CompareFunc)(elem1, elem2) > 0) {
                ChangeValues(elem1, elem2, size_elem);
                change_flag++;
            }

            assert(data != NULL);
        }

        if (change_flag == 0) {
            break;
        }
    }
}

//---------------------------------------------------------HELPING FUNCTIONS--------------------------------------------------------

/**
 * @brief comparator for sort by alphabet starting from the beginning of string
 * 
 * @param[in] elem_a first compared string
 * @param[in] elem_b second compared string
 * @return int negative if earlier in alphabetical order else positive
 */
int Compare_by_beginning(const void* elem_a, const void* elem_b) {

    assert(elem_a != NULL);
    assert(elem_b != NULL);

    const Line* a = (const Line*) elem_a;
    const Line* b = (const Line*) elem_b;

    return str_cmp_beginning(a, b);
}

/**
 * @brief comparator for sort by alphabet starting from the end of string
 * 
 * @param[in] elem_a first compared string
 * @param[in] elem_b second compared string
 * @return int negative if earlier in alphabetical order else positive
 */
int Compare_by_end(const void* elem_a, const void* elem_b) {

    assert(elem_a != NULL);
    assert(elem_b != NULL);

    const Line* a = (const Line*) elem_a;
    const Line* b = (const Line*) elem_b;

    return str_cmp_end(a, b);
}

/**
 * @brief compare strings by the beginning
 * 
 * @param[in] str1 first struct with pointer on the string and its length
 * @param[in] str2 second struct with pointer on the string and its length
 * @return int negative if earlier in alphabetical order else positive
 */
int str_cmp_beginning(const Line* str1, const Line* str2) {

    size_t i = 0;
    size_t j = 0;
    
    while(i < str1->len && j < str2->len) {

        int is_a = isalpha(str1->str[i]);
        int is_b = isalpha(str2->str[j]);  /// for

        if (!is_a) {
            i++;
        }
        if (!is_b) {
            j++;
        }

        if (is_a && is_b) {

            char x = tolower(str1->str[i++]);
            char y = tolower(str2->str[j++]);
            
            if (x != y) {
                return x - y;
            }
        }
    }

    while (str1->str[i] != '\0' && !isalpha(str1->str[i])) i++;        //если в конце стояли  не буквы
    while (str2->str[j] != '\0' && !isalpha(str2->str[j])) j++;

    return tolower(str1->str[i]) - tolower(str2->str[j]);
}

/**
 * @brief compare strings by the end
 * 
 * @param[in] str1 first struct with pointer on the string and its length
 * @param[in] str2 second struct with pointer on the string and its length
 * @return int negative if earlier in alphabetical order else positive
 */
int str_cmp_end(const Line* str1, const Line* str2) {

    int i = str1->len-1;       //(*str1).len
    int j = str2->len-1;  //ploho

    size_t char_flag = 0;   //struct s ukazatel i razmer  //+
    
    while(i >= 0 && j >= 0) {

        int is_a = isalpha(str1->str[i]);
        int is_b = isalpha(str2->str[j]);

        if (!is_a) {
            i--;
        }
        if (!is_b) {
            j--;
        }

        if (is_a && is_b) {

            char x = tolower(str1->str[i--]);
            char y = tolower(str2->str[j--]);
            
            if (x != y) {
                return x - y;
            }
        }
    }

    while (i >= 0 && !isalpha(str1->str[i])) i--;        
    while (j >= 0 && !isalpha(str2->str[j])) j--;

    if (i < 0 || j < 0) return i - j; // если одна строка закончилась раньше
    return tolower(str1->str[i]) - tolower(str2->str[j]);

}

/**
 * @brief changes places of two elements
 * 
 * @param[in, out] value1 first element
 * @param[in, out] value2 second element
 * @param[in, out] size_elem size of elements
 */
void ChangeValues(void* value1, void* value2, size_t size_elem) {
    
    char temp[size_elem]= {0};

    memcpy(temp, value1, size_elem);
    memcpy(value1, value2, size_elem);
    memcpy(value2, temp, size_elem);
}

/**
 * @brief checks if string has letter
 * 
 * @param[in] s string 
 * @return int 1 if has letter else 0
 */
int HasLetters(const char* s) {

    for (size_t i = 0; s[i] != '\0'; i++) {
        if (isalpha(s[i])) {
            return 1; 
        }
    }
    return 0; 
}

//---------------------------------------------------------FUNCTIONS FOR MAIN--------------------------------------------

/**
 * @brief cleans dynamic memory of 3 arrays
 * 
 * @param[in] index pointer of first array
 * @param[in] start_index pointer of second array
 * @param[in] buffer pointer of third array
 */
void clean_memory(Line* index, Line* start_index, char* buffer) {
    free(index);
    free(start_index);
    free(buffer);
}

/**
 * @brief prints links on sorted texts
 * 
 * @param[in] line_beg number of first line
 * @param[in] line_end number of second line
 * @param[in] line_orig number of third line
 */
void print_links(ssize_t line_beg, ssize_t line_end, ssize_t line_orig) {

    printf("Alphabetically: " OUTPUT ":%zu\n", line_beg);
    printf("By rhyme: " OUTPUT ":%zu\n", line_end);
    printf("Original: " OUTPUT ":%zu\n", line_orig);
}