#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variables
typedef struct Entry
{
    char *name;
    int id;
} Entry_t;

// Array for the full list, and the number of entries within it
Entry_t *orderedData;
int dataSize;

// Used for the de-allocation at the end
int initialDataSize;

// Loads the ordered data from the local file "orderedData.txt"
#define MAX_LINE_LENGTH 128
int loadOrderedData()
{
    // Try to open up the file.
    FILE *fileIn = fopen( "orderedData.txt", "rt" );
    if( !fileIn )
    {
        printf( "Could not open the local file 'orderedData.txt' for reading.\n" );
        return -1;
    }

    // Assume all lines are less than MAX_LINE_LENGTH bytes
    char line[MAX_LINE_LENGTH];

    // First line should be a single integer, corresponding to the total number of data items
    fgets( line, MAX_LINE_LENGTH, fileIn );
    dataSize = atoi( line );
    if( dataSize <= 0 )
    {
        printf( "Could not parse first line of 'orderedData.txt' as a positive integer; has the file been corrupted?\n" );
        fclose( fileIn );
        return -1;
    }

    // Store the initial data size for deallocation at the end
    initialDataSize = dataSize;

    // Allocate memory for the full data list
    orderedData = (Entry_t*) malloc( dataSize*sizeof(Entry_t) );
    if( !orderedData )
    {
        printf( "Could not allocate memory for the %i entries expected for the data list.\n", dataSize );
        fclose( fileIn );
        return -1;
    }

    // Read the data in line by line, adding to the ordered data array. This assumes a very specific format
    // for how the data is stored in the file, but should work fine with the example provided
    int lineNum;
    for( lineNum=0; lineNum<dataSize; lineNum++ )
    {
        // Read in the full line
        fgets( line, MAX_LINE_LENGTH, fileIn );

        // Find the first space in the line; assume this is the separator between the name and the ID
        char* separator = strchr( line, ' ' );

        // Copy everything up to the separator into the 'name' field and add the null character to the end
        int length = strlen(line) - strlen(separator);
        orderedData[lineNum].name = (char*) malloc( (length+1)*sizeof(char) );
        strncpy( orderedData[lineNum].name, line, length );
	    orderedData[lineNum].name[length] = '\0';

        // Get the ID from the second part of the line, and store in the 'id' field
        orderedData[lineNum].id = atoi( separator );
   }

    // Close the file and return with zero, denoting 'success'
    fclose( fileIn );
    return 0;
}

// Prints the current data list to stdout
void printData()
{
    // Simple message for an empty data array
    if( !dataSize )
    {
        printf( "The data array is empty.\n" );
        return;
    }

    // Output ach entry in turn, one line each
    int i;
    for( i=0; i<dataSize; i++ )
        printf( "Index %i  \t:\t%s (id=%i)\n", i, orderedData[i].name, orderedData[i].id );
}

// Frees any resources associated with the ordered data
void deleteOrderedData()
{
    int i;

    // Free up all of the memory allocated for the 'name' fields
    for( i=0; i<initialDataSize; i++ )
        if( orderedData[i].name != NULL )
            free( orderedData[i].name );

    // Free up the array itself
    free( orderedData );
}

// Swaps the two entries with the given indices using local temporary variables. Not thread safe
void swapEntries( int i, int j )
{
    // Only continue if the index values are valid
    if( i<0 || j<0 || i>=dataSize || j>=dataSize || i==j )
    {
        printf( "Cannot swap entries %i and %i: One or both indices out of range, or both the same.\n", i, j );
        return;
    }

    // Swap the pointers for the strings; no memory re-allocation required
    char *tempName = orderedData[i].name;
    int   tempID   = orderedData[i].id  ;

    orderedData[i].name = orderedData[j].name;
    orderedData[i].id   = orderedData[j].id  ;

    orderedData[j].name = tempName;
    orderedData[j].id   = tempID  ;
}

// Returns a random number in the range 0 to dataSize-1 inclusive. Not thread safe
int randomEntryIndex()
{
    return rand() % dataSize;
}

