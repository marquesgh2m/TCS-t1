#include <hellfire.h>

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

void bist(void) {
    printf("I am bist\n");
    sabotador();

    for(;;);
    //hf_kill(hf_selfid());
}

void sabotador(void) {
    printf("main_routine addr:%x (dentro da sabotador)\n", main_routine);
}

void main_routine(void) {
    int arr[] = {19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    int arr_size = sizeof(arr)/sizeof(arr[0]);

    printf("\n======\nBefore sorting array: \n");
    printArray(arr, arr_size);

    bubbleSort(arr, arr_size);
    printf("Sorted array: \n");
    printArray(arr, arr_size);

    for(;;);
    //hf_kill(hf_selfid());
}

void app_main(void){
    hf_spawn(bist, 0, 0, 0, "bist thread", 2048);
    hf_spawn(main_routine, 0, 0, 0, "main_routine thread", 2048);
}