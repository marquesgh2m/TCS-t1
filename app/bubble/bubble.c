// https://en.wikipedia.org/wiki/Bitwise_operations_in_C
// https://www.youtube.com/watch?v=5dA-cyiGKAA&ab_channel=HunterJohnson

#include <hellfire.h>

#define READ_ONLY 1
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


int LFSR(int v1, int v2){
    return (v1+v2)>>1;
}


void S1(int *addr, int addr_size, int *signature, int read_only){
    int i;
    int value;
    *addr = 0; 

    if(!read_only) printf("S1:\t\t    Ra\t\tWa(not)\t\tWa\t\tWa(not)\n");
    else printf("S1:\t\t    Ra\n");

    for(i=0;i<addr_size;i++){
        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
        printf("i:%2d *adr:%08x: %08x", i, addr, value); 
        if(read_only) printf("\n", value); 

        if(!read_only){
            //Wa(not)
            *addr = ~(value); 
            printf("\t%08x", *addr); 

            //Wa
            *addr = value; 
            printf("\t%08x", *addr); 

            //Wa(not)
            *addr = ~(value); 
            printf("\t%08x\n", *addr); 
        }
        addr++; 
    }
    printf("Signature1:%08x\n", *signature); 
}


void S2(int *addr, int addr_size, int *signature, int read_only){
    int i;
    int value;

    if(!read_only) printf("\nS2:\t\t    Ra(not)\tWa\t\tRa\t\tWa(not)\n");
    else printf("\nS2:\t\t    Ra(not)\tRa\n");
    
    for(i=0;i<addr_size;i++){
        //Ra(not)
        if(!read_only) value = *addr;
        else value = ~(*addr);
        *signature = LFSR(*signature, value);
        printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        if(!read_only){
            //Wa
            *addr = ~(value); 
            printf("\t%08x", *addr); 
        }

        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
        printf("\t%08x", value); 
        if(read_only) printf("\n", value); 

        if(!read_only){
            //Wa(not)
            *addr = ~(value); 
            printf("\t%08x\n", *addr); 
        }
        addr++; 
    }    
    printf("Signature2:%08x\n", *signature); 
}


void S3(int *addr, int addr_size, int *signature, int read_only){
    int i;
    int value;

    if(!read_only) printf("\nS3:\t\t    Ra(not)\tWa\t\tWa(not)\t\tWa\n");
    else printf("\nS3:\t\t    Ra(not)\n");
    
    for(i=0;i<addr_size;i++){
        //Ra(not)
        if(!read_only) value = *addr;
        else value = ~(*addr);
        *signature = LFSR(*signature, value);
        printf("i:%2d *adr:%08x: %08x", i, addr, value); 
        if(read_only) printf("\n", value); 

        if(!read_only){
            //Wa
            *addr = ~(value); 
            printf("\t%08x", *addr); 

            //Wa(not)
            *addr = value; 
            printf("\t%08x", *addr); 

            //Wa
            *addr = ~(value); 
            printf("\t%08x\n", *addr); 
        }
        addr++; 
    }
    printf("Signature3:%08x\n", *signature); 
}


void S4(int *addr, int addr_size, int *signature, int read_only){
    int i;
    int value;

    if(!read_only) printf("\nS4:\t\t    Ra\t\tWa(not)\t\tRa(not)\t\tWa\n");
    else printf("\nS4:\t\t    Ra\t\tRa(not)\t\t\n");
    
    for(i=0;i<addr_size;i++){
        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
        printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        if(!read_only){
            //Wa(not)
            *addr = ~(value); 
            printf("\t%08x", *addr); 
        }

        //Ra(not)
        if(!read_only) value = *addr;
        else value = ~(*addr);
        *signature = LFSR(*signature, value);
        printf("\t%08x", value); 
        if(read_only) printf("\n", value); 

        if(!read_only){
            //Wa
            *addr = ~(value); 
            printf("\t%08x\n", *addr); 
        }
        addr++; 
    }
    printf("Signature4:%08x\n", *signature); 
}

int bist_test(int *addr, int addr_size){
    printf("\n----- BIST TEST -----\n");
    int i;
    int value;
    int signature = 0;
    S1(addr, addr_size, &signature, 0);
    S2(addr, addr_size, &signature, 0);
    S3(addr, addr_size, &signature, 0);
    S4(addr, addr_size, &signature, 0);

    printf("bist_test_sum:%08x\n", signature); 
    return signature;
}

int bist_signature(int *addr, int addr_size){
    printf("----- BIST SIGNATURE -----\n");
    int i;
    int value;
    int signature = 0;

    S1(addr, addr_size, &signature, READ_ONLY);
    S2(addr, addr_size, &signature, READ_ONLY);
    S3(addr, addr_size, &signature, READ_ONLY);
    S4(addr, addr_size, &signature, READ_ONLY);

    printf("bist_signature_sum:%08x\n", signature); 
    return signature;
}

void bist_th(void) {
    hf_block(hf_id("main_routine_th"));

    int bist_test_sum, bist_signature_sum;

    printf("\n------------ BIST TEST ------------(%d)\n", bist_test_counter++);

    bist_test_sum = bist_test(&bubbleSort, BUBBBLESORT_ADDR_SIZE);
    bist_signature_sum = bist_signature(&bubbleSort, BUBBBLESORT_ADDR_SIZE);

    printf("bist_test_sum:%08x\n", bist_test_sum); 
    printf("bist_signature_sum:%08x\n", bist_signature_sum);
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