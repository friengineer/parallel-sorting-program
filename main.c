#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include "data_manipulation_methods.h"

// Reverses the order of the data in-place, leaving the answer in the same array as before
void reverseOrder_inParallel()
{
  int i;

  // No two threads interact with the same entry so this is thread safe
  #pragma omp parallel for
  for( i=0; i<dataSize/2; i++ )
  {
    swapEntries( i, dataSize-1-i );
  }

  return;
}

// Sort all entries in-place, in order of increasing id
void sortByID_inParallel()
{
  int i, j;
  Entry_t temp[dataSize];

  // Initialise temporary array with the data array's values
  #pragma omp parallel for
  for( i=0; i<dataSize; i++ )
  {
    temp[i].name = (char*) malloc( strlen(orderedData[i].name)*sizeof(char) );
    strcpy(temp[i].name, orderedData[i].name);
    temp[i].id = orderedData[i].id;
  }

  // Count how many items' IDs are smaller than an item's ID and place the latter item
  // into the correct place in the list
  #pragma omp parallel for private(j)
  for( i=0; i<dataSize; i++ )
  {
    int count = 0;

    for( j=0; j<dataSize; j++ )
    {
      // If another item's ID is the same as the current item's ID, the count is
      // incremented to ensure the items are put in different places in the data array
      if( temp[i].id>temp[j].id || ( temp[i].id==temp[j].id && i!=j && j>i ) )
      {
        count++;
      }
    }

    // Place the item in the correct place in the data array
    strcpy(orderedData[count].name, temp[i].name);
    orderedData[count].id = temp[i].id;

    // Free memory allocated for the item's 'name' field in the temporary array
    free( temp[i].name );
  }

  return;
}

// Shuffle all items in parallel. Here, "shuffle" means to randomly select 2 entry indices and swap the corresponding entries
// This is done dataSize*(dataSize-1)/2 times so that each pairing is swapped on average once
void shuffle_inParallel()
{
  int i;
  unsigned int state;

  #pragma omp parallel for private(state)
  for( i=0; i<dataSize*(dataSize-1)/2; i++ )
  {
    // Use a separate state for each thread with seeds unique to each thread. Include
    // i in the bitwise XOR in case a thread has multiple iterations that occur in
    // the same second
    state = omp_get_thread_num() ^ time(NULL) ^ i;

    // Generate two random indices using the state
    int item1 = rand_r(&state) % dataSize;
    int item2 = rand_r(&state) % dataSize;

    // Continuously generate random indices for item 2 until the index is different
    // to item 1
    while( item1==item2 )
    {
      item2 = rand_r(&state) % dataSize;
    }

    #pragma omp critical
    {
      swapEntries( item1, item2 );
    }
  }
}

// Remove the last item from the list in a thread safe manner
void removeLastItem_threadSafe()
{
  // Decrease the size of the data array
  #pragma omp atomic
    dataSize--;

  return;
}

int main( int argc, char **argv )
{
    // Initialise the random number generator to the system clock
    srand( time(NULL) );

    // Make sure we have exactly 1 command line argument after executable name
    if( argc != 2 )
    {
        printf( "Enter a single command line argument for the operation required:\n(1) Reverse the order.\n(2) Sort in order of increasing ID.\n" );
        printf( "(3) Shuffle.\n(4) Remove all items from the end in a parallel loop.\n" );
        return EXIT_FAILURE;
    }

    // Convert argument to an option number, and ensure it is in the valid range. Note argv[0] is the executable name
    int option = atoi( argv[1] );
    if( option<=0 || option>4 )
    {
        printf( "Option number '%s' invalid.\n", argv[1] );
        return EXIT_FAILURE;
    }

    // Display how many threads are being used
    printf( "Performing option '%i' using %i OpenMP thread(s).\n\n", option, omp_get_max_threads() );

    // Loads the data from file
    if( loadOrderedData()<0 ) return EXIT_FAILURE;

    // Print the initial ordered data to screen
    printf( "Before the operation:\n" );
    printData();

    // Perform an operation on the data depending on the option entered on the command line
    int i, initialDataSize = dataSize;

    switch( option )
    {
        case 1:
            reverseOrder_inParallel();
            break;

        case 2:
            sortByID_inParallel();
            break;

        case 3:
            shuffle_inParallel();
            break;

        case 4:
            #pragma omp parallel for
            for( i=0; i<initialDataSize; i++ )
                removeLastItem_threadSafe();
            break;

        default:
            printf( "Option '%i' not implememnted by the switch() statement.\n", option );
            return EXIT_FAILURE;
    }

    // Print the data after the operation, then free up all resources and quit
    printf( "\nAfter the operation:\n" );
    printData();
    deleteOrderedData();

    return EXIT_SUCCESS;
}
