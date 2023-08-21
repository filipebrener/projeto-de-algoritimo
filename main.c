#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// caminhos dos arquivos
#define INPUT_PATH "arquivodeentrada"
#define OUTPUT_PATH "arquivodesaida"
#define TIME_PATH "tempo"

// enums e structs
typedef enum {
    GROWING,
    DESCENDING,
    RANDOM
}Order;

typedef enum {
    INSERTION_SORT,
    BUBBLE_SORT,
    SELECTION_SORT,
    SHELL_SORT
}Algorithm;

char* get_algorithm_name(Algorithm algorithm){
   switch (algorithm){
      case INSERTION_SORT: return "insertion_sort";
      case BUBBLE_SORT: return "bubble_sort";
      case SELECTION_SORT: return "selection_sort";
      case SHELL_SORT: return "shell_sort";
      default:
        printf("Algoritimo invalido: %s", get_algorithm_name(algorithm));
        abort();
   }
}

char* get_order_name(Order order){
   switch (order){
      case GROWING: return "crescente";
      case DESCENDING: return "decrescente";
      case RANDOM: return "randomico";
      default:
        printf("Ordem invalida: %s", get_order_name(order));
        abort();
   }
}

typedef struct {
    char *input_file_path, *output_file_path, *time_file_path;
    int *input_list;
    double execution_time;
    int input_size;
    Algorithm algorithm;
} structure;

// declaração das funções
char* get_file_path(Algorithm algorithm, char* path, int input_size, Order order);
structure structure_factory(int input_size, Order order, Algorithm algorithm);
int* get_input_list(int input_size, Order order);
void insertion_sort(structure *s);
void bubble_sort(structure *s);
void selection_sort(structure *s);
void shell_sort(structure *s);
void sort_and_save(structure *s);
void create_directory_if_not_exists(const char *dir);
Order select_order();
int select_quantity(int size_list[]);
Algorithm select_algorithm();
void free_structure(structure s);

int main(){
    int size_list[6] = {10, 100, 1000, 10000, 100000, 1000000};
    Algorithm algorithm;
    Order order;
    int quantity_index;
    int is_to_continue;
    do{
        system("clear");
        algorithm = select_algorithm();
        system("clear");
        order = select_order();
        system("clear");
        quantity_index = select_quantity(size_list);
        if(quantity_index < 6 && quantity_index >= 0){
            printf("Executando algoritimo %s em um array inicial em ordem %s\n", get_algorithm_name(algorithm), get_order_name(order));
            printf("Executando para um array de %d elementos: ", size_list[quantity_index]);
            fflush(stdout);
            structure s = structure_factory(size_list[quantity_index], order, algorithm);
            sort_and_save(&s);
            printf("OK!\n");
            printf("Algoritimo executado com sucesso!\n");
        } else if(quantity_index == 6){
            printf("Executando algoritimo %s em um array inicial em ordem %s\n\n", get_algorithm_name(algorithm), get_order_name(order));
            for(int i = 0; i < 6; i++){
                printf("Executando para um array de %d elementos: ", size_list[i]);
                fflush(stdout);
                structure s = structure_factory(size_list[i], order, algorithm);
                sort_and_save(&s);
                printf("OK!\n");
                free_structure(s);
            }
            printf("Algoritimo executado com sucesso!\n");
        } else {
            printf("Opção escolhida é inválida!\n");
        }
        printf("Digite 0 para sair ou qualquer outro numero para voltar ao inicio: ");
        scanf("%d", &is_to_continue);
    }while(is_to_continue);
    return 0;
}

// funções
char* get_file_path(Algorithm algorithm, char* path, int input_size, Order order){
    char* file_path = (char*) malloc(41 * sizeof(char));
    char folder_path[30];
    sprintf(folder_path,"%s/%s",get_algorithm_name(algorithm), path);
    create_directory_if_not_exists(folder_path);
    sprintf(file_path,"%s/%s%d.txt", folder_path, get_order_name(order), input_size);
    return file_path;
}

structure structure_factory(int input_size, Order order, Algorithm algorithm){
    structure s;
    s.input_size = input_size;
    s.input_file_path = get_file_path(algorithm, INPUT_PATH, input_size, order);
    s.output_file_path = get_file_path(algorithm, OUTPUT_PATH, input_size, order);
    s.time_file_path = get_file_path(algorithm, TIME_PATH, input_size, order);
    s.input_list = get_input_list(input_size, order);
    s.algorithm = algorithm;
    return s;
}

int* get_input_list(int input_size, Order order){
    int *input_list = (int*) malloc(input_size * sizeof(int));
    switch(order){
        case GROWING:
            for(int i = 0; i < input_size; i++){
                input_list[i] = i;
            }
            break;
        case DESCENDING:
            for(int i = 0; i < input_size; i++){
                input_list[i] = input_size - i;
            }
            break;
        case RANDOM:
            for(int i = 0; i < input_size; i++){
                input_list[i] = rand() % input_size;
            }
            break;
        default:
            printf("Método não conhecido: %s", get_order_name(order));
        break;
    }
    return input_list;
}

void save_in_file(structure s, char* file_path){
    FILE *file;
    file = fopen(file_path, "w");
    if(file == NULL){
        printf("Erro na abertura do arquivo [%s]\n", file_path);
        return;
    }
    fprintf(file, "%d\n", s.input_size);
    for(int i = 0; i < s.input_size; i++){
        fprintf(file, "%d\n", s.input_list[i]);
    }
    fclose(file);
}

void save_time_in_file(structure s, char* file_path){
    FILE *file;
    file = fopen(file_path, "w");
    if(file == NULL){
        printf("Erro na abertura do arquivo [%s]\n", file_path);
        return;
    }
    fprintf(file, "%d\n%.6f", s.input_size, s.execution_time);
    fclose(file);
}

void sort_and_save(structure *s){
    save_in_file(*s, s->input_file_path);
    switch (s->algorithm){
        case INSERTION_SORT: insertion_sort(s);
        case BUBBLE_SORT: bubble_sort(s);
        case SELECTION_SORT: selection_sort(s);
        case SHELL_SORT: shell_sort(s);
    }
    save_in_file(*s, s->output_file_path);
    save_time_in_file(*s,s->time_file_path);
}

void insertion_sort(structure *s){
    int current_number;
    int current_index;
    s->execution_time = 0.0;
    clock_t begin = clock();
    for(int i = 1; i < s->input_size; i++){
        current_number = s->input_list[i];
        current_index = i - 1;
        while(current_index >= 0 && s->input_list[current_index] > current_number){
            s->input_list[current_index + 1] = s->input_list[current_index];
            current_index--;
        }
        s->input_list[current_index + 1] = current_number;
    }
    clock_t end = clock();
    s->execution_time += (double)(end - begin) / CLOCKS_PER_SEC;
}

void bubble_sort(structure *s){
    s->execution_time = 0.0;
    int aux;
    clock_t begin = clock();
    for(int i = 0; i < s->input_size; i++){
        for(int j = 0; j < s->input_size - i; j ++){
            if(s->input_list[j] > s->input_list[j + 1]){
                aux = s->input_list[j];
                s->input_list[j] = s->input_list[j + 1];
                s->input_list[j + 1] = aux;
            }
        }
    }
    clock_t end = clock();
    s->execution_time += (double)(end - begin) / CLOCKS_PER_SEC;
}

void selection_sort(structure *s){
    s->execution_time = 0.0;
    int current_lower_index;
    int aux;
    clock_t begin = clock();
    for(int i = 0; i < s->input_size; i++){
        current_lower_index = i;
        for(int j = i + 1; j < s->input_size; j ++){
            if(s->input_list[current_lower_index] < s->input_list[j]){
                current_lower_index = j;
            }
        }
        aux = s->input_list[i];
        s->input_list[i] = s->input_list[current_lower_index];
        s->input_list[current_lower_index] = aux;
    }
    clock_t end = clock();
    s->execution_time += (double)(end - begin) / CLOCKS_PER_SEC;
}

void shell_sort(structure *s){
    int j, current_value;
    clock_t begin = clock();
    int h = 1;
    while(h < s->input_size) {
        h = 3*h+1;
    }
    while (h > 0) {
        for(int i = h; i < s->input_size; i++) {
            current_value = s->input_list[i];
            j = i;
            while (j > h-1 && current_value <= s->input_list[j - h]) {
                s->input_list[j] = s->input_list[j - h];
                j = j - h;
            }
            s->input_list[j] = current_value;
        }
        h = h/3;
    }
    clock_t end = clock();
    s->execution_time += (double)(end - begin) / CLOCKS_PER_SEC;
}

void print_list(int* list, int list_size){
    for(int i = 0; i < list_size; i++){
        printf("%d ", list[i]);
        if(i % 10 == 0) printf("\n");
    }
}

void show_algorithm_list(){
    printf("CODE: ALGORITHM\n");
    printf("1   : INSERTION SORT\n");
    printf("2   : BUBBLE SORT\n");
    printf("3   : SELECTION SORT\n");
    printf("4   : SHELL SORT\n");
    printf("\n");
}

void show_order_list(){
    printf("CODE: ORDER\n");
    printf("1   : CRESCENTE\n");
    printf("2   : DECRESCENTE\n");
    printf("3   : RANDOMICO\n");
    printf("\n");
}

Algorithm select_algorithm(){
    int algorithm_code;
    show_algorithm_list();
    printf("Digite o código do algoritimo a ser testado: ");
    do{
        scanf("%d", &algorithm_code);
        switch(algorithm_code){
            case 1: return INSERTION_SORT;
            case 2: return BUBBLE_SORT;
            case 3: return SELECTION_SORT;
            case 4: return SHELL_SORT;
            default:
                printf("Opcao invalida: %d\n", algorithm_code);
                printf("Digite um valor válido para o algoritimo: ");
        }
    }while(1);
}

Order select_order(){
    int algorithm_code;
    show_order_list();
    printf("Digite o código da ordem a ser testada: ");
    do{
        scanf("%d", &algorithm_code);
        switch(algorithm_code){
            case 1: return GROWING;
            case 2: return DESCENDING;
            case 3: return RANDOM;
            default:
                printf("Opcao invalida: %d\n", algorithm_code);
                printf("Digite um valor válido para a ordem: ");

        }
    }while(1);
}

void free_structure(structure s){
    free(s.input_list);
    free(s.input_file_path);
    free(s.output_file_path);
    free(s.time_file_path);
}

int select_quantity(int size_list[]){
    int quantity;
    printf("CODE: TAMANHO\n");
    for(int i = 0; i < 6; i++){
        printf("%d   : %d\n", i, size_list[i]);
    }
    printf("6   : Todos os tamanhos\n");
    printf("Escolha: ");
    scanf("%d",&quantity);
    return quantity;
}

void create_directory_if_not_exists(const char *dir){
    char tmp[256];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);
    if(tmp[len - 1] == '/') tmp[len - 1] = 0;
    for(p = tmp + 1; *p; p++)
        if(*p == '/') {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    mkdir(tmp, S_IRWXU);
}
