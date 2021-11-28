// https://en.wikipedia.org/wiki/Bitwise_operations_in_C
// https://www.youtube.com/watch?v=5dA-cyiGKAA&ab_channel=HunterJohnson

#include <hellfire.h>

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


void S1(int *addr, int addr_size, int *signature){
    int i, value; 

    printf("S1:\t\t    Ra\t\tWa(not)\t\tWa\t\tWa(not)\n");

    for(i=0;i<addr_size;i++){
        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
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
    printf("Signature1:%08x\n", *signature); 
}


void S2(int *addr, int addr_size, int *signature){
    int i, value;

    printf("\nS2:\t\t    Ra(not)\tWa\t\tRa\t\tWa(not)\n");
    
    for(i=0;i<addr_size;i++){
        //Ra(not)
        value = *addr;
        *signature = LFSR(*signature, value);
        printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        //Wa
        *addr = ~(value); 
        printf("\t%08x", *addr); 

        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
        printf("\t%08x", value); 

        //Wa(not)
        *addr = ~(value); 
        printf("\t%08x\n", *addr); 
        
        addr++; 
    }    
    printf("Signature2:%08x\n", *signature); 
}


void S3(int *addr, int addr_size, int *signature){
    int i, value;

    printf("\nS3:\t\t    Ra(not)\tWa\t\tWa(not)\t\tWa\n");
    
    addr += addr_size-1;
    for(i=addr_size-1;i>=0;i--){
        //Ra(not)
        value = *addr;
        *signature = LFSR(*signature, value);
        printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        //Wa
        *addr = ~(value); 
        printf("\t%08x", *addr); 

        //Wa(not)
        *addr = value; 
        printf("\t%08x", *addr); 

        //Wa
        *addr = ~(value); 
        printf("\t%08x\n", *addr); 
        
        addr--; 
    }
    printf("Signature3:%08x\n", *signature); 
}


void S4(int *addr, int addr_size, int *signature){
    int i, value;

    printf("\nS4:\t\t    Ra\t\tWa(not)\t\tRa(not)\t\tWa\n");
    
    addr += addr_size-1;
    for(i=addr_size-1;i>=0;i--){
        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
        printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        //Wa(not)
        *addr = ~(value); 
        printf("\t%08x", *addr); 

        //Ra(not)
        value = *addr;
        *signature = LFSR(*signature, value);
        printf("\t%08x", value); 

        //Wa
        *addr = ~(value); 
        printf("\t%08x\n", *addr); 
        addr--; 
    }
    printf("Signature4:%08x\n", *signature); 
}

void S1S(int *addr, int addr_size, int *signature){
    int i, value;

    printf("S1S:\t\t    Ra\n");
    for(i=0;i<addr_size;i++){
        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
        printf("i:%2d *adr:%08x: %08x\n", i, addr, value); 

        addr++; 
    }
    printf("SignatureS1:%08x\n", *signature); 
}


void S2S(int *addr, int addr_size, int *signature){
    int i, value;

    printf("\nS2S:\t\t    Ra(not)\tRa\n");
    
    for(i=0;i<addr_size;i++){
        //Ra(not)
        value = ~(*addr);
        *signature = LFSR(*signature, value);
        printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
        printf("\t%08x\n", value); 

        addr++; 
    }    
    printf("SignatureS2:%08x\n", *signature); 
}


void S3S(int *addr, int addr_size, int *signature){
    int i, value;

    printf("\nS3S:\t\t    Ra(not)\n");
    
    addr += addr_size-1;
    for(i=addr_size-1;i>=0;i--){
        //Ra(not)
        value = ~(*addr);
        *signature = LFSR(*signature, value);
        printf("i:%2d *adr:%08x: %08x\n", i, addr, value); 

        addr--; 
    }
    printf("SignatureS3:%08x\n", *signature); 
}


void S4S(int *addr, int addr_size, int *signature){
    int i, value;

    printf("\nS4S:\t\t    Ra\t\tRa(not)\t\t\n");
    
    addr += addr_size-1;
    for(i=addr_size-1;i>=0;i--){
        //Ra
        value = *addr;
        *signature = LFSR(*signature, value);
        printf("i:%2d *adr:%08x: %08x", i, addr, value); 

        //Ra(not)
        value = ~(*addr);
        *signature = LFSR(*signature, value);
        printf("\t%08x\n", value); 

        addr--; 
    }
    printf("SignatureS4:%08x\n", *signature); 
}


int bist_test(int *addr, int addr_size){
    printf("\n----- BIST TEST -----\n");
    int i;
    int value;
    int signature = 0;
    S1(addr, addr_size, &signature);
    S2(addr, addr_size, &signature);
    S3(addr, addr_size, &signature);
    S4(addr, addr_size, &signature);

    printf("bist_test_sum:%08x\n", signature); 
    return signature;
}

int bist_signature(int *addr, int addr_size){
    printf("----- BIST SIGNATURE -----\n");
    int i;
    int value;
    int signature = 0;

    S1S(addr, addr_size, &signature);
    S2S(addr, addr_size, &signature);
    S3S(addr, addr_size, &signature);
    S4S(addr, addr_size, &signature);

    printf("bist_signature_sum:%08x\n", signature); 
    return signature;
}

void bist_th(void) {
    hf_block(hf_id("main_routine_th"));

    int bist_test_sum, bist_signature_sum;

    printf("\n------------ BIST TEST ------------(%d)\n", bist_test_counter++);

    bist_test_sum = bist_test(&bubbleSort, BUBBBLESORT_ADDR_SIZE);
    bist_signature_sum = bist_signature(&bubbleSort, BUBBBLESORT_ADDR_SIZE);

     printf("\nResults:\n");
    printf("bist_test_sum:\t\t%08x\n", bist_test_sum); 
    printf("bist_signature_sum:\t%08x\n", bist_signature_sum);
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