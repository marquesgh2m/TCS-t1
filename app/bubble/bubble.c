// https://en.wikipedia.org/wiki/Bitwise_operations_in_C
// https://www.youtube.com/watch?v=5dA-cyiGKAA&ab_channel=HunterJohnson
// https://www.dsprelated.com/showthread/adsp/2785-1.php

#include <hellfire.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEW   "\x1B[33m"
#define CRESET "\x1B[0m"

#define VERBOSE 1 //0 Desativa prints em excesso; 1 Ativa todos os prints

#define BUBBBLESORT_ADDR_SIZE 18 // Quantidade de comandos assembly que implementam o bubblesort
#define ARR_SIZE 20

#define MAINSTOP //for(;;); //hf_kill(hf_selfid());
#define BISTSTOP //for(;;); //hf_kill(hf_selfid());
#define ADRCHECKSTOP //for(;;);
#define LFSRSTOP //for(;;);

#define BT_SABOTADOR_BUBBLE 0 //random() % 3+1 // Sabotador vai do 1 ao 3 (local entre etapas do bist), 0 desativa o sabotador
#define BS_SABOTADOR_BUBBLE 0 //random() % 3+1
#define BT_SABOTADOR_ARR random() % 3+1
#define BS_SABOTADOR_ARR 0 // random() % 3+1

#define REINICIALIZA_ARR // for(i=0;i<ARR_SIZE;i++)arr[i]=ARR_SIZE-i-1;


int bist_test_counter = 0;
int arr[ARR_SIZE] = {20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
int verbose = VERBOSE;


// Função que implementa o Bubblesort que será sabotado
void bubbleSort(int arr[], int n)
{
    int i, j, temp;
    for (i = 0; i < n-1; i++)
        for (j = 0; j < n-i-1; j++)
            if (arr[j] > arr[j+1]){
                temp = arr[j];
                arr[j]   = arr[j+1];
                arr[j+1] = temp;
            }
}

/* Function to print an array */
void printArray(int arr[], int size)
{
    int i;
    for (i=0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int groundtruth_check(int *addr, int groundtruth){
    if(*addr != groundtruth){
        printf(RED"\nFatal Error in memory address %08x:%08x! Expected value:%08x\n"CRESET, addr, *addr, groundtruth);
        ADRCHECKSTOP
    }
}

int LFSR_check(int sig1, int sig2){
    if(sig1 != sig2){
        printf(RED"\nFatal Error in memory! Wrong signature: %08x != %08x!\n"CRESET, sig1, sig2);
        LFSRSTOP
    }
}

int LFSR(int b, int x){
    int i;
    // Usando o polinomio primitivo (x^3 + x^2 + 1)
    //int y4 = (((b >> 0) & 0x1) & ((x >> 0) & 0x1)) ? 1 : 0;
    //int y3 = (((b >> 2) & 0x1) & ((x >> 2) & 0x1)) ? 1 : 0;
    //int y2 = (((b >> 11) & 0x1) & ((x >> 11) & 0x1)) ? 1 : 0;
    //int y1 = (((b >> 19) & 0x1) & ((x >> 19) & 0x1)) ? 1 : 0;
    //x = x << 1 | (y1 ^ y2 ^ y3 ^ y4);
    x = x + b;
    return (x << (32 - 1)) | (x >> 1); // Circular shift
}


void S1(int *addr, int addr_size, int *signature, int *addr_groundtruth){
    int i, value; 

    if(verbose) printf("S1:\t\t    Ra\t\tWa(not)\t\tWa\t\tWa(not)\n");
    for(i=0;i<addr_size;i++){
        //Ra
        value = *addr;
        addr_groundtruth[i] = *addr; // Salva valor para usar como comparacao nas outras etapas
        *signature = LFSR(*signature, value);
        if(verbose) printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        //Wa(not)
        *addr = ~(value); 
        if(verbose) printf("\t%08x", *addr); 

        //Wa
        *addr = value; 
        if(verbose) printf("\t%08x", *addr); 

        //Wa(not)
        *addr = ~(value); 
        if(verbose) printf("\t%08x\n", *addr); 
        
        addr++; 
    }
    printf("Signature S1:%08x\n", *signature); 
}


void S2(int *addr, int addr_size, int *signature, int *addr_groundtruth){
    int i, value;

    if(verbose) printf("\nS2:\t\t    Ra(not)\tWa\t\tRa\t\tWa(not)\n");
    for(i=0;i<addr_size;i++){
        //Ra(not)
        value = *addr;
        *signature = LFSR(*signature, value);
        if(verbose) printf("i:%2d *adr:%08x: %08x", i, addr, value); 
        groundtruth_check(addr, ~(addr_groundtruth[i]));

        //Wa
        *addr = ~(value); 
        if(verbose) printf("\t%08x", *addr); 

        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
        if(verbose) printf("\t%08x", value); 
        groundtruth_check(addr, addr_groundtruth[i]);

        //Wa(not)
        *addr = ~(value); 
        if(verbose) printf("\t%08x\n", *addr); 
        
        addr++; 
    }    
    printf("Signature S2:%08x\n", *signature); 
}


void S3(int *addr, int addr_size, int *signature, int *addr_groundtruth){
    int i, value;

    if(verbose) printf("\nS3:\t\t    Ra(not)\tWa\t\tWa(not)\t\tWa\n");
    
    addr += addr_size-1;
    for(i=addr_size-1;i>=0;i--){
        //Ra(not)
        value = *addr;
        *signature = LFSR(*signature, value);
        if(verbose) printf("i:%2d *adr:%08x: %08x", i, addr, value); 
        groundtruth_check(addr, ~(addr_groundtruth[i]));

        //Wa
        *addr = ~(value); 
        if(verbose) printf("\t%08x", *addr); 

        //Wa(not)
        *addr = value; 
        if(verbose) printf("\t%08x", *addr); 

        //Wa
        *addr = ~(value); 
        if(verbose) printf("\t%08x\n", *addr); 
        
        addr--; 
    }
    printf("Signature S3:%08x\n", *signature); 
}


void S4(int *addr, int addr_size, int *signature, int *addr_groundtruth){
    int i, value;

    if(verbose) printf("\nS4:\t\t    Ra\t\tWa(not)\t\tRa(not)\t\tWa\n");
    
    addr += addr_size-1;
    for(i=addr_size-1;i>=0;i--){
        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
        if(verbose) printf("i:%2d *adr:%08x: %08x", i, addr, value); 
        groundtruth_check(addr, addr_groundtruth[i]);

        //Wa(not)
        *addr = ~(value); 
        if(verbose) printf("\t%08x", *addr); 

        //Ra(not)
        value = *addr;
        *signature = LFSR(*signature, value);
        if(verbose) printf("\t%08x", value); 
        groundtruth_check(addr, ~(addr_groundtruth[i]));

        //Wa
        *addr = ~(value); 
        if(verbose) printf("\t%08x\n", *addr); 
        addr--; 
    }
    printf("Signature S4:%08x -> bist_test_sum\n", *signature); 
}

void S1S(int *addr, int addr_size, int *signature, int *addr_groundtruth){
    int i, value;

    if(verbose) printf("S1S:\t\t    Ra\n");
    for(i=0;i<addr_size;i++){
        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
        if(verbose) printf("i:%2d *adr:%08x: %08x\n", i, addr, value); 

        addr++; 
    }
    printf("Signature S1S:%08x\n", *signature); 
}


void S2S(int *addr, int addr_size, int *signature, int *addr_groundtruth){
    int i, value;

    if(verbose) printf("\nS2S:\t\t    Ra(not)\tRa\n");
    for(i=0;i<addr_size;i++){
        //Ra(not)
        value = ~(*addr);
        *signature = LFSR(*signature, value);
        if(verbose) printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
        if(verbose) printf("\t%08x\n", value); 

        addr++; 
    }    
    printf("Signature S2S:%08x\n", *signature); 
}


void S3S(int *addr, int addr_size, int *signature, int *addr_groundtruth){
    int i, value;

    if(verbose) printf("\nS3S:\t\t    Ra(not)\n");
    
    addr += addr_size-1;
    for(i=addr_size-1;i>=0;i--){
        //Ra(not)
        value = ~(*addr);
        *signature = LFSR(*signature, value);
        if(verbose) printf("i:%2d *adr:%08x: %08x\n", i, addr, value); 

        addr--; 
    }
    printf("Signature S3S:%08x\n", *signature); 
}


void S4S(int *addr, int addr_size, int *signature, int *addr_groundtruth){
    int i, value;

    if(verbose) printf("\nS4S:\t\t    Ra\t\tRa(not)\t\t\n");
    
    addr += addr_size-1;
    for(i=addr_size-1;i>=0;i--){
        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
        if(verbose) printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        //Ra(not)
        value = ~(*addr);
        *signature = LFSR(*signature, value);
        if(verbose) printf("\t%08x\n", value); 

        addr--; 
    }
    printf("Signature S4S:%08x -> bist_signature_sum\n", *signature); 
}


int bist_test(int *addr, int addr_size, char *test_name, int local_sabotador){
    printf("\n----- BIST TEST (%s)-----\n", test_name);
    int i;
    int value;
    int signature = 0;
    int addr_groundtruth[addr_size];

    S1(addr, addr_size, &signature, addr_groundtruth); 
    if(local_sabotador == 1) sabotador(addr, addr_size); // SABOTADOR
    S2(addr, addr_size, &signature, addr_groundtruth);
    if(local_sabotador == 2) sabotador(addr, addr_size); // SABOTADOR
    S3(addr, addr_size, &signature, addr_groundtruth);
    if(local_sabotador == 3) sabotador(addr, addr_size); // SABOTADOR
    S4(addr, addr_size, &signature, addr_groundtruth);

    return signature;
}

int bist_signature(int *addr, int addr_size, char *test_name, int local_sabotador){
    printf("----- BIST SIGNATURE (%s)-----\n", test_name);
    int i;
    int value;
    int signature = 0;
    int addr_groundtruth[addr_size];

    S1S(addr, addr_size, &signature, addr_groundtruth);
    if(local_sabotador == 1) sabotador(addr, addr_size); // SABOTADOR
    S2S(addr, addr_size, &signature, addr_groundtruth);
    if(local_sabotador == 2) sabotador(addr, addr_size); // SABOTADOR
    S3S(addr, addr_size, &signature, addr_groundtruth);
    if(local_sabotador == 3) sabotador(addr, addr_size); // SABOTADOR
    S4S(addr, addr_size, &signature, addr_groundtruth);

    return signature;
}

void bist_th(void) {
    int bist_test_sum, bist_signature_sum;

    // Evita que o sistema operacional pare o BIST na metade para executar a thread main_routine_th
    hf_block(hf_id("main_routine_th")); 

    printf(CRESET"\n------------ BIST TEST ------------(%d)\n", bist_test_counter++);
    
    // Bist na area de memória que hospeda o código do bubblesort
    bist_test_sum = bist_test(&bubbleSort, BUBBBLESORT_ADDR_SIZE, "Bubble", BT_SABOTADOR_BUBBLE);
    bist_signature_sum = bist_signature(&bubbleSort, BUBBBLESORT_ADDR_SIZE, "Bubble", BS_SABOTADOR_BUBBLE);
    printf("\nResults BubbleSort:\n");
    printf("bist_test_sum:\t\t%08x\n", bist_test_sum); 
    printf("bist_signature_sum:\t%08x\n", bist_signature_sum);
    LFSR_check(bist_test_sum, bist_signature_sum);


    // Bist na area de memória que hospeda o vetor de dados (arr)
    bist_test_sum = bist_test(arr, ARR_SIZE, "Arr", BT_SABOTADOR_ARR);
    bist_signature_sum = bist_signature(arr, ARR_SIZE, "Arr", BS_SABOTADOR_ARR);
    printf("\nResults Arr:\n");
    printf("bist_test_sum:\t\t%08x\n", bist_test_sum); 
    printf("bist_signature_sum:\t%08x\n", bist_signature_sum);
    LFSR_check(bist_test_sum, bist_signature_sum);

    printf("\n----------- BIST TEST END ----------\n");

    hf_resume(hf_id("main_routine_th"));
    delay_ms(10);
    BISTSTOP
}


void sabotador(int *addr, int addr_size){
    int i;
    int random_adress = random() % addr_size; // Seleciona um valor entre 0 a N
    int random_bit = random() % 32;

    //printf("FirstAddress: %08x, depth:%d\n", aux, depth);

    for(i=0;i<addr_size;i++){
        if(i==random_adress){
            printf(YEW"i:%2d *adr:%08x: %08x dec(%d){Original}", i, addr, *addr, *addr);
            *addr ^= 1UL << random_bit;
            printf(" -> %08x dec(%d){Sabotado}\n"CRESET, *addr, *addr);
        }
        //if(verbose) printf("i:%2d *adr:%08x: %08x \tdec(%d)\n", i, addr, *addr, *addr); 
        addr++;
    }
}

void main_routine_th(void) {
    int i;

    printf("\n======\nBubbleSort addr: %08x \n", &bubbleSort);
    printf("Arr addr: %08x \n\n", arr);

    // Inicialização da array
    REINICIALIZA_ARR

    printf("Before sorting array: \n");
    printArray(arr, ARR_SIZE);

    bubbleSort(arr, ARR_SIZE);
    printf(GRN"After sorting array: \n"CRESET);
    printArray(arr, ARR_SIZE);

    MAINSTOP
}

void app_main(void){
    // Define uma semente para testes pseudorandomicos controlados
    srand(42); //srand(time(NULL)); 

    hf_spawn(bist_th, 0, 0, 0, "bist_th", 2048);
    hf_spawn(main_routine_th, 0, 0, 0, "main_routine_th", 2048);
}