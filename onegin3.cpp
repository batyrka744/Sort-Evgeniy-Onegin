#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>


#define my_assert(exp, ret) if(!(exp)) {\
    printf("%s:%d: [Assertation failed: %s, file %s, line %d]\n", __FILE__, __LINE__, #exp,  __FILE__, __LINE__);\
    return ret;}    


//-----------------------------------------------------FUNCTION DECLATRATIONS---------------------------------------------

char* ReadFromFile(const char* name, size_t* out_size);

ssize_t PrintStrings(const char* reason, const char* name, char** index,  size_t nLines);

void ChangeValues(void* value1, void* value2, size_t size_elem);

void Bubble_Sort(void* const data, size_t size, size_t size_elem, int (*CompareFunc)(const void* a, const void* b));

int my_strlen(const char* s);

int Compare_by_beginning(const void* elem_a, const void* elem_b);

int Compare_by_end(const void* elem_a, const void* elem_b);

int str_cmp_beginning(const char* str1, const char* str2);

int str_cmp_end(const char* str1, const char* str2);

char** SplitLines(char* buffer, size_t file_size, size_t* nLines);

int HasLetters(const char* s);

//----------------------------------------------------------MAIN---------------------------------------------------------------------

int main(void) {

    fclose(fopen("output.txt", "wb"));

    size_t file_size = 0;

    char* buffer = ReadFromFile("Onegin_full.txt", &file_size);
    assert(buffer != NULL);

    size_t nLines = 0;

    char** index = SplitLines(buffer, file_size, &nLines);
    char** start_index = (char**)calloc(nLines, sizeof(char*));
    memcpy(start_index, index, nLines * sizeof(char*));


    ssize_t current_line = 1;

    ssize_t line_beg = current_line;
    Bubble_Sort(index, nLines, sizeof(char*), &Compare_by_beginning);
    current_line += PrintStrings("SORTED BY THE BEGINNING", "output.txt",index, nLines);

    ssize_t line_end = current_line;
    qsort(index, nLines, sizeof(char*), &Compare_by_end);
    current_line += PrintStrings("SORTED BY THE END", "output.txt",index, nLines);

    ssize_t line_orig = current_line;
    PrintStrings("ORIGINAL", "output.txt", start_index, nLines);

    free(index);
    free(start_index);
    free(buffer);


    printf("Alphabetically: output.txt:%zu\n", line_beg);
    printf("By rhyme: output.txt:%zu\n", line_end);
    printf("By rhyme: output.txt:%zu\n", line_orig);


    return 0;
}

//----------------------------------------------------FUNCTIONS---------------------------------------------------------------------

/**
 * @brief reads file and puts text in the buffer
 * 
 * @param[in] name name of file with text
 * @param[out] out_size the real number of bytes
 * @return char* buffer with full text
 */
char* ReadFromFile(const char* name, size_t* out_size) {

    my_assert(out_size != NULL, NULL);

    my_assert(name != NULL, 0);
    FILE* file = fopen(name, "rb");
    my_assert(file != NULL, 0);

    struct stat file_info;
    int fd = fileno(file);
    my_assert(fstat(fd, &file_info) == 0, NULL);
    size_t file_size = (size_t)file_info.st_size;


    char* buffer = (char*)calloc(file_size + 5, sizeof(char));
    
    my_assert(buffer != NULL, NULL);

    size_t read_bytes = fread(buffer, sizeof(char), file_size , file);
    buffer[read_bytes] = '\0';
    
    *out_size = read_bytes;

    fclose(file);

    return buffer;
}

/**
 * @brief splits buffer to strings and puts pointers to them in array
 * 
 * @param[in] buffer array with text
 * @param[in] file_size size of file with text
 * @param[in, out] nLines number of the strings
 * @return char** array with pointers to the strings
 */
char** SplitLines(char* buffer, size_t file_size, size_t* nLines) {

    my_assert(file_size != 0, NULL);
    my_assert(file_size != 0, NULL); 


    char** index = (char**)calloc(file_size, sizeof(char*));
    

    *nLines = 1;
    index[0] = &buffer[0];

    
    for (int i = 0; i < file_size; i++) {

        if (buffer[i] == '\n'  && i < file_size - 1) {
            buffer[i] = '\0';
            index[(*nLines)++] = &buffer[i + 1];
        }
    }

    return index;
}

/**
 * @brief puts text into the file
 * 
 * @param[in] reason reason of print
 * @param[in] name name of file
 * @param[in] index array with pointers on the strings
 * @param[in] nLines number of the strings
 */
ssize_t PrintStrings(const char* reason, const char* name, char** index,  size_t nLines) {

    my_assert(reason != NULL, -1);
    my_assert(name != NULL, -1);
    assert(index != NULL);

    size_t printed_lines = 0;

    FILE* output = fopen(name, "ab");
    assert(output != NULL);

    fputs(reason, output);
    fputs("\n\n\n\n\n\n", output);
    printed_lines += 6;

    for (int i = 0; i < nLines; i++) { 
        assert(index[i] != NULL);

        if (!HasLetters(index[i])) continue;

        printed_lines++;
        fputs(index[i], output);
        fputc('\n', output);
    }

    fputs("\n\n\n\n\n\n", output);
    printed_lines += 6;

    fclose(output);

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

    for (int n = 0; n < size; n++) {

        int change_flag = 0; 

        for (int i = 0; i < size - 1; i++) {


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
 * @brief counts number of characters in the string without enter
 * 
 * @param[in] s string
 * @return int number of characters
 */
int my_strlen(const char* s) {

    assert(s != NULL);

    int i = 0;

    for (; s[i] != '\0'; i++);

    return i - 1;
}


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

    const char* a = *(const char**) elem_a;
    const char* b = *(const char**) elem_b;

    assert(a != NULL);
    assert(b != NULL);


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

    const char* a = *(const char**) elem_a;
    const char* b = *(const char**) elem_b;

    assert(a != NULL);
    assert(b != NULL);


    return str_cmp_end(a, b);
}

/**
 * @brief compare strings by the beginning
 * 
 * @param[in] str1 first string
 * @param[in] str2 second string
 * @return int negative if earlier in alphabetical order else positive
 */
int str_cmp_beginning(const char* str1, const char* str2) {

    int i = 0;
    int j = 0;
    
    while(str1[i] != '\0' && str2[j] != '\0') {

        int is_a = isalpha(str1[i]);
        int is_b = isalpha(str2[j]);

        if (!is_a) {
            i++;
        }
        if (!is_b) {
            j++;
        }

        if (is_a && is_b) {

            char x = tolower(str1[i++]);
            char y = tolower(str2[j++]);
            
            if (x != y) {
                return x - y;
            }
        }
    }

    while (str1[i] != '\0' && !isalpha((unsigned char)str1[i])) i++;        //если в конце стояли  не буквы
    while (str2[j] != '\0' && !isalpha((unsigned char)str2[j])) j++;

    return tolower(str1[i]) - tolower(str2[j]);
}

/**
 * @brief compare strings by the end
 * 
 * @param[in] str1 first string
 * @param[in] str2 second string
 * @return int negative if earlier in alphabetical order else positive
 */
int str_cmp_end(const char* str1, const char* str2) {

    int i = my_strlen(str1);
    int j = my_strlen(str2);
    size_t char_flag = 0;
    
    
    while(i > 0 && j > 0) {

        int is_a = isalpha(str1[i]);
        int is_b = isalpha(str2[j]);

        if (!is_a) {
            i--;
        }
        if (!is_b) {
            j--;
        }

        if (is_a && is_b) {

            char x = tolower(str1[i--]);
            char y = tolower(str2[j--]);
            
            if (x != y) {
                return x - y;
            }
        }
    }

    while (i >= 0 && !isalpha(str1[i])) i--;        
    while (j >= 0 && !isalpha(str2[j])) j--;

    if (i < 0 || j < 0) return i - j; // если одна строка закончилась раньше
    return tolower(str1[i]) - tolower(str2[j]);

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
    for (int i = 0; s[i] != '\0'; i++) {
        if (isalpha(s[i])) {
            return 1; 
        }
    }
    return 0; 
}