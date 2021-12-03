// https://en.wikipedia.org/wiki/Bitwise_operations_in_C
// https://www.youtube.com/watch?v=5dA-cyiGKAA&ab_channel=HunterJohnson
// https://www.dsprelated.com/showthread/adsp/2785-1.php

#include <hellfire.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEW   "\x1B[33m"
#define CRESET "\x1B[0m"

#define VERBOSE 0 //0 Desativa prints em excesso; 1 Ativa todos os prints; -1 Remove todo e qualquer print

#define BUBBBLESORT_ADDR_SIZE 18 // Quantidade de comandos assembly que implementam o bubblesort
#define ARR_SIZE 20

#define MAINSTOP //for(;;); //hf_kill(hf_selfid());
#define BISTSTOP //for(;;); //hf_kill(hf_selfid());
#define ADRCHECKSTOP //for(;;);
#define LFSRSTOP //for(;;);
#define FAULTMASKSTOP for(;;);


#define SABOTADOR_RANDOM random() % 6+6+1 //random() % 12+1 // Sabotador vai do 1 ao 12 Bubble(1,2,3,4,5,6) Arr(7,8,9,10,11,12) (local entre etapas do bist), 0 desativa o sabotador (Se bota 18 que é o valor do bubble quando roda o bist do arr o código vai acessar uma area fora do vetor)


#define REINICIALIZA_ARR  for(i=0;i<ARR_SIZE;i++)arr[i]=ARR_SIZE-i-1;


int bist_test_counter = 0;
int arr[ARR_SIZE] = {20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
int verbose = VERBOSE;
int addr_groundtruth[4][20]; // 4 devido as 4 etapas de bist e 20 por que entre Arr e Bubble o Arr tem a area de memória maior
int fault_mask_flag = 0; // variavel para testar se um erro escapou do bist


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


int groundtruth_check(int sig1, int sig2, int *addr, char *phase){
    if(sig1 != sig2){
        printf(RED"\n%s: Fatal Error in memory address %08x:%08x! Wrong signature:%08x != %08x(Groundtruth)\n"CRESET, phase, addr, *addr, sig2, sig1);
        
        fault_mask_flag = 0; // Conseguiu achar a falha
        ADRCHECKSTOP
    }
}

int LFSR(int signature, int value){
    // Usando o polinomio primitivo (x^31 + x^30 + 1)
    signature ^= value;
    signature ^= (((signature >> 31) ^ (signature >> 30)) & 0x00000001);
    return (signature << 31) | (signature >> 1); // Circular shift
}

void LFSR_DEBUG(){
    int i;
    int signature = 0x01;
    printf("LFSR Debug\n");
    for(i=0; i<32; i++){
        signature = LFSR(signature, 0x00);
        printf("%08x\n", signature);
    }
    //for(;;);
}

void S1(int *addr, int addr_size, int *signature){
    int i, value; 

    if(verbose) printf("S1:\t\t    Ra\t\tWa(not)\t\tWa\t\tWa(not)\t\tSignature\n");
    for(i=0;i<addr_size;i++){
        //Ra
        value = *addr;
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
        if(verbose) printf("\t%08x", *addr); 

        if(verbose) printf("\t%08x\n", *signature); 
        addr_groundtruth[0][i] = *signature; 
        addr++; 
    }
    printf("Signature S1:%08x\n", *signature); 
}


void S2(int *addr, int addr_size, int *signature){
    int i, value;

    if(verbose) printf("\nS2:\t\t    Ra(not)\tWa\t\tRa\t\tWa(not)\t\tSignature\n");
    for(i=0;i<addr_size;i++){
        //Ra(not)
        value = *addr;
        *signature = LFSR(*signature, value);
        if(verbose) printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        //Wa
        *addr = ~(value); 
        if(verbose) printf("\t%08x", *addr); 

        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
        if(verbose) printf("\t%08x", value); 

        //Wa(not)
        *addr = ~(value); 
        if(verbose) printf("\t%08x", *addr); 
        
        if(verbose) printf("\t%08x\n", *signature);
        addr_groundtruth[1][i] = *signature; 
        addr++; 
    }    
    printf("Signature S2:%08x\n", *signature); 
}


void S3(int *addr, int addr_size, int *signature){
    int i, value;

    if(verbose) printf("\nS3:\t\t    Ra(not)\tWa\t\tWa(not)\t\tWa\t\tSignature\n");
    
    addr += addr_size-1;
    for(i=addr_size-1;i>=0;i--){
        //Ra(not)
        value = *addr;
        *signature = LFSR(*signature, value);
        if(verbose) printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        //Wa
        *addr = ~(value); 
        if(verbose) printf("\t%08x", *addr); 

        //Wa(not)
        *addr = value; 
        if(verbose) printf("\t%08x", *addr); 

        //Wa
        *addr = ~(value); 
        if(verbose) printf("\t%08x", *addr); 

        if(verbose) printf("\t%08x\n", *signature);
        addr_groundtruth[2][i] = *signature;         
        addr--; 
    }
    printf("Signature S3:%08x\n", *signature); 
}


void S4(int *addr, int addr_size, int *signature){
    int i, value;

    if(verbose) printf("\nS4:\t\t    Ra\t\tWa(not)\t\tRa(not)\t\tWa\t\tSignature\n");
    
    addr += addr_size-1;
    for(i=addr_size-1;i>=0;i--){
        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
        if(verbose) printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        //Wa(not)
        *addr = ~(value); 
        if(verbose) printf("\t%08x", *addr); 

        //Ra(not)
        value = *addr;
        *signature = LFSR(*signature, value);
        if(verbose) printf("\t%08x", value); 

        //Wa
        *addr = ~(value); 
        if(verbose) printf("\t%08x", *addr); 

        if(verbose) printf("\t%08x\n", *signature);
        addr_groundtruth[3][i] = *signature;  
        addr--; 
    }
    printf("Signature S4:%08x\n", *signature); 
}

void S1S(int *addr, int addr_size, int *signature){
    int i, value;

    if(verbose) printf("S1S:\t\t    Ra\n\t\tSignature");
    for(i=0;i<addr_size;i++){
        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
        if(verbose) printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        if(verbose) printf("\t%08x\n", *signature);
        groundtruth_check(addr_groundtruth[0][i], *signature, addr, "S1S");
        addr++; 
    }
    printf("Signature S1S:%08x\n", *signature); 
}


void S2S(int *addr, int addr_size, int *signature){
    int i, value;

    if(verbose) printf("\nS2S:\t\t    Ra(not)\tRa\t\tSignature\n");
    for(i=0;i<addr_size;i++){
        //Ra(not)
        value = ~(*addr);
        *signature = LFSR(*signature, value);
        if(verbose) printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
        if(verbose) printf("\t%08x", value); 

        if(verbose) printf("\t%08x\n", *signature);
        groundtruth_check(addr_groundtruth[1][i], *signature, addr, "S2S");
        addr++; 
    }    
    printf("Signature S2S:%08x\n", *signature); 
}


void S3S(int *addr, int addr_size, int *signature){
    int i, value;

    if(verbose) printf("\nS3S:\t\t    Ra(not)\t\tSignature\n");
    
    addr += addr_size-1;
    for(i=addr_size-1;i>=0;i--){
        //Ra(not)
        value = ~(*addr);
        *signature = LFSR(*signature, value);
        if(verbose) printf("i:%2d *adr:%08x: %08x", i, addr, value);

        if(verbose) printf("\t%08x\n", *signature);
        groundtruth_check(addr_groundtruth[2][i], *signature, addr, "S3S");
        addr--; 
    }
    printf("Signature S3S:%08x\n", *signature); 
}


void S4S(int *addr, int addr_size, int *signature){
    int i, value;

    if(verbose) printf("\nS4S:\t\t    Ra\t\tRa(not)\t\tSignature\n");
    
    addr += addr_size-1;
    for(i=addr_size-1;i>=0;i--){
        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
        if(verbose) printf("i:%2d *adr:%08x: %08x", i, addr, value);

        //Ra(not)
        value = ~(*addr);
        *signature = LFSR(*signature, value);
        if(verbose) printf("\t%08x", value); 

        if(verbose) printf("\t%08x\n", *signature);
        groundtruth_check(addr_groundtruth[3][i], *signature, addr, "S4S"); 
        addr--; 
    }
    printf("Signature S4S:%08x\n", *signature); 
}


int bist_test(int *addr, int addr_size, char *test_name, int sabotador_id){
    printf("\n----- BIST TEST (%s) SabotadorId=%d-----\n", test_name, sabotador_id);
    int i;
    int value;
    int signature = 0;

    S1(addr, addr_size, &signature); 
    if(sabotador_id == 1) sabotador(addr, addr_size); // SABOTADOR
    S2(addr, addr_size, &signature);
    if(sabotador_id == 2) sabotador(addr, addr_size); // SABOTADOR
    S3(addr, addr_size, &signature);
    if(sabotador_id == 3) sabotador(addr, addr_size); // SABOTADOR
    S4(addr, addr_size, &signature);

    return signature;
}

int bist_signature(int *addr, int addr_size, char *test_name, int sabotador_id){
    printf("\n----- BIST SIGNATURE (%s) SabotadorId=%d-----\n", test_name, sabotador_id);
    int i;
    int value;
    int signature = 0;

    S1S(addr, addr_size, &signature);
    if(sabotador_id == 4) sabotador(addr, addr_size); // SABOTADOR
    S2S(addr, addr_size, &signature);
    if(sabotador_id == 5) sabotador(addr, addr_size); // SABOTADOR
    S3S(addr, addr_size, &signature);
    if(sabotador_id == 6) sabotador(addr, addr_size); // SABOTADOR
    S4S(addr, addr_size, &signature);

    return signature;
}

void bist_th(void) {
    int bist_test_sum, bist_signature_sum;

    // Evita que o sistema operacional pare o BIST na metade para executar a thread main_routine_th
    hf_block(hf_id("main_routine_th")); 

    // Para a execução se encontra um erro de Fault Mask
    if(fault_mask_flag){
        printf(RED"\nBIST Fault Mask Error: BIST lost an error!\n"CRESET);
        printArray(arr, ARR_SIZE);
        bubbleSort(arr, ARR_SIZE);
        printArray(arr, ARR_SIZE);
        FAULTMASKSTOP
    }

    int sabotador_id  = SABOTADOR_RANDOM;
    int sabotador_id_bubble = sabotador_id>6?0:sabotador_id;
    int sabotador_id_arr = sabotador_id>6?sabotador_id-6:0;


    printf(CRESET"\n------------ BIST TEST ------------(%d) SID:%d\n", 1+bist_test_counter++, sabotador_id);
    // Bist na area de memória que hospeda o código do bubblesort
    bist_test_sum = bist_test(&bubbleSort, BUBBBLESORT_ADDR_SIZE, "Bubble", sabotador_id_bubble);
    bist_signature_sum = bist_signature(&bubbleSort, BUBBBLESORT_ADDR_SIZE, "Bubble", sabotador_id_bubble);

    // Bist na area de memória que hospeda o vetor de dados (arr)
    bist_test_sum = bist_test(arr, ARR_SIZE, "Arr", sabotador_id_arr);
    bist_signature_sum = bist_signature(arr, ARR_SIZE, "Arr", sabotador_id_arr);

    printf("\n----------- BIST TEST END ----------\n");

    hf_resume(hf_id("main_routine_th"));
    delay_ms(10);
    BISTSTOP
}


void sabotador(int *addr, int addr_size){
    int i;
    int random_adress = random() % addr_size; // Seleciona um valor entre 0 a N
    int random_bit = random() % 32;

    printf("\n------ SABOTADOR ------\n");
    for(i=0;i<addr_size;i++){
        if(i==random_adress){
            printf(YEW"i:%2d *adr:%08x: %08x dec(%d){Original}", i, addr, *addr, *addr);
            *addr ^= 1UL << random_bit;
            printf(" -> %08x dec(%d){Sabotado}\n"CRESET, *addr, *addr);
        }
        else{
            if(verbose) printf("i:%2d *adr:%08x: %08x \tdec(%d)\n", i, addr, *addr, *addr); 
        }
        addr++;
    }
    fault_mask_flag = 1; // Variavel para ver se o erro escapou do bist 
    printf("------\n");
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