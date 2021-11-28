// https://en.wikipedia.org/wiki/Bitwise_operations_in_C

#include <hellfire.h>

#define READ_ONLY 1;
#define BUBBBLESORT_ADDR_SIZE 18
#define ARR_SIZE 20

//int arr_size = sizeof(arr)/sizeof(arr[0]);
int bist_test_counter = 0;


// A function to implement bubble sort
void bubbleSort(int arr[], int n);
// Function to print an array 
void printArray(int arr[], int size);
// Main routine that call bubble an print out the sorted array
void main_routine(void);


// A function to implement bubble sort
void bubbleSort(int arr[], int n)
{
    int i, j, temp;
    for (i = 0; i < n-1; i++)
        // Last i elements are already in place
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

int bist_test(int *addr, int addr_size){
    int i;
    int value;
    //S1
    printf("S1:\t\t    Ra\t\tWa(not)\t\tWa\t\tWa(not)\n");
    for(i=0;i<addr_size;i++){
        //Ra
        value = *addr;
        printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        //Wa(not)
        *addr = ~(value); 
        printf("\t%08x", *addr); 

        //Wa
        *addr = value; 
        printf("\t%08x", *addr); 

        //Wa(not)
        *addr = ~(value); 
        printf("\t%08x\n", *addr); 
        addr++; 
    }
    addr -= addr_size; // Faz com que o ponteiro volte para o inicio do endereço que está sendo testado
    
    //S2
    printf("\nS2:\t\t    Ra(not)\tWa\t\tRa\t\tWa(not)\n");
    for(i=0;i<addr_size;i++){
        //Ra(not)
        value = ~(*addr);
        printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        //Wa
        *addr = value; 
        printf("\t%08x", *addr); 

        //Ra
        value = *addr;
        printf("\t%08x", value); 

        //Wa(not)
        *addr = ~(value); 
        printf("\t%08x\n", *addr); 
        addr++; 
    }
    addr -= addr_size; // Faz com que o ponteiro volte para o inicio do endereço que está sendo testado
    
    //S3
    printf("\nS3:\t\t    Ra(not)\tWa\t\tWa(not)\t\tWa\n");
    for(i=0;i<addr_size;i++){
        //Ra(not)
        value = ~(*addr);
        printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        //Wa
        *addr = value; 
        printf("\t%08x", *addr); 

        //Wa(not)
        *addr = ~(value); 
        printf("\t%08x", *addr); 

        //Wa
        *addr = value; 
        printf("\t%08x\n", *addr); 
        addr++; 
    }
    addr -= addr_size; // Faz com que o ponteiro volte para o inicio do endereço que está sendo testado

    //S4
    printf("\nS4:\t\t    Ra\tWa(not)\tRa(not)\t\tWa\n");
    for(i=0;i<addr_size;i++){
        //Ra
        value = *addr;
        printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        //Wa(not)
        *addr = ~(value); 
        printf("\t%08x", *addr); 

        //Ra(not)
        value = ~(*addr);
        printf("\t%08x", value); 

        //Wa
        *addr = value; 
        printf("\t%08x\n", *addr); 
        addr++; 
    }
      
}
//printf("i:%2d *adr:%08x: %08x\n", i, addr, *addr);

void bist_th(void) {
    hf_block(hf_id("main_routine_th"));

    int bist_test_sum, bist_signature_sum;

    printf("\n------------ BIST TEST ------------(%d)\n", bist_test_counter++);
    bist_test(&bubbleSort, BUBBBLESORT_ADDR_SIZE);

    //sabotador(arr, arr_size);

    //sabotador(&bubbleSort, 18); // 18 é a quantidade de comandos assembly que implementam o bubblesort
    
    printf("\n---------------------------------\n");

    hf_resume(hf_id("main_routine_th"));
    delay_ms(1000);
    for(;;);
    //hf_kill(hf_selfid());
}


void sabotador(int *aux, int depth){
    int i;
    int random_depth = random() % depth; // Seleciona um valor entre 0 a N
    int random_bit = random() % 32;
    int random_adress = (int)(aux)+random_depth*4;

    printf("RandomAdress(%d): %08x, RandomBit:%d\n", random_depth, random_adress, random_bit);
    printf("FirstAddress: %08x, depth:%d\n", aux, depth);

    for(i=0;i<depth;i++){
        if(i==random_depth){
            printf("i:%2d *adr:%08x: %08x \tdec(%d){Original}\n", i, aux, *aux, *aux);
            *aux ^= 1UL << random_bit;
        }
        printf("i:%2d *adr:%08x: %08x \tdec(%d)\n", i, aux, *aux, *aux); 
        aux++;
    }

}

void main_routine_th(void) {
    int i;
    int arr[ARR_SIZE];

    printf("\nbubbleSort addr: %08x \n", &bubbleSort);
    printf("arr addr: %08x \n", arr);
    printf("main_routine addr:%x \n", main_routine_th);
    printf("main_routine addr:%x \n", &main_routine_th);


    // Inicialização da array
    for(i=0;i<ARR_SIZE;i++){
        arr[i]=ARR_SIZE-i-1;
    }

    printf("\n======\nBefore sorting array: \n");
    printArray(arr, ARR_SIZE);

    bubbleSort(arr, ARR_SIZE);
    printf("After sorting array: \n");
    printArray(arr, ARR_SIZE);

    for(;;);
    //hf_kill(hf_selfid());
}

void app_main(void){
    // Set random seed
    srand(42); //srand(time(NULL)); 

    hf_spawn(bist_th, 0, 0, 0, "bist_th", 6000);
    hf_spawn(main_routine_th, 0, 0, 0, "main_routine_th", 2048);
}