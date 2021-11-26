#include <hellfire.h>

// A function to implement bubble sort
void bubbleSort(int arr[], int n);
/* Function to print an array */
void printArray(int arr[], int size);

void bs(void) {
    int arr[] = {
            7, 42, 53, 35, 21, 35, 31, 56, 90, 15,
            64, 78, 52, 18, 52, 93, 31, 53, 89, 66
    };
    int n = sizeof(arr)/sizeof(arr[0]);

    printf("Before sorting array: \n");
    printArray(arr, n);

    bubbleSort(arr, n);
    printf("\nSorted array: \n");
    printArray(arr, n);

    for(;;);
}

void bist(void) {
    printf("I am bist\n");

    for(;;);
}

void sabota(void) {
    printf("bs addr(dentro da sabota): %x\n", bs);
    hf_spawn(bist, 0, 0, 0, "bist", 2048);

    for(;;);
}

void app_main(void){
    hf_spawn(sabota, 0, 0, 0, "sabota", 2048);
    hf_spawn(bs, 0, 0, 0, "bs", 2048);
}

void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// A function to implement bubble sort
void bubbleSort(int arr[], int n)
{
    int i, j;
    for (i = 0; i < n-1; i++)

        // Last i elements are already in place
        for (j = 0; j < n-i-1; j++)
            if (arr[j] > arr[j+1])
                swap(&arr[j], &arr[j+1]);
}

/* Function to print an array */
void printArray(int arr[], int size)
{
    int i;
    for (i=0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}