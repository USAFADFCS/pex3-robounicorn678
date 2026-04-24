/** main.c
 * ===========================================================
 * Name: C2C Charles liermann, 23 APR 2026
 * Section: CS483 / M4
 * Project: PEX3 - Page Replacement Simulator
 * Purpose: Reads a BYU binary memory trace file and simulates
 *          LRU page replacement to measure fault rates across
 *          varying frame allocations.
 * Documentation: 
 * used the following sources for help:
 * https://www.geeksforgeeks.org/operating-systems/page-replacement-algorithms-in-operating-systems/
 * https://www.geeksforgeeks.org/computer-organization-architecture/cache-replacement-policies/
 * https://www.geeksforgeeks.org/operating-systems/virtual-memory-in-operating-system/
 * =========================================================== */
#include <stdio.h>
#include <stdlib.h>
#include "byutr.h"
#include "pagequeue.h"

#define PROGRESS_INTERVAL 100000  // print status every N accesses

int main(int argc, char **argv) {
    FILE *ifp = NULL;
    unsigned long numAccesses = 0;
    p2AddrTr traceRecord;

    // Validate command-line arguments: trace_file frame_size
    if (argc != 3) 
    {
        fprintf(stderr, "usage: %s input_byutr_file frame_size\n", argv[0]);
        fprintf(stderr, "\nframe_size:\n\t1: 512 bytes\n\t2: 1KB\n\t3: 2KB\n\t4: 4KB\n");
        exit(1);
    }

    // Open the binary trace file
    ifp = fopen(argv[1], "rb");
    if (ifp == NULL) 
    {
        fprintf(stderr, "cannot open %s for reading\n", argv[1]);
        exit(1);
    }

    // Parse and validate frame size selection
    int menuOption = atoi(argv[2]);
    if (menuOption < 1 || menuOption > 4) 
    {
        fprintf(stderr, "invalid frame size option: %s (must be 1-4)\n", argv[2]);
        fclose(ifp);
        exit(1);
    }

    // Map menu option to page geometry
    int offsetBits = 0;
    int maxFrames = 0;
    switch (menuOption) 
    {
        case 1:
            offsetBits = 9;   // 512-byte pages
            maxFrames = 8192;
            break;
        case 2:
            offsetBits = 10;  // 1KB pages
            maxFrames = 4096;
            break;
        case 3:
            offsetBits = 11;  // 2KB pages
            maxFrames = 2048;
            break;
        case 4:
            offsetBits = 12;  // 4KB pages
            maxFrames = 1024;
            break;
    }

    fprintf(stderr, "Frame size option %d: %d offset bits, %d max frames, algorithm=LRU\n",
            menuOption, offsetBits, maxFrames);

    // Okay, time to actually build the thing and hope malloc doesn't laugh at us, please no memory leaks!!!!
    PageQueue *pq = pqInit(maxFrames);  // MEMORY FITTTTTT
    if (pq == NULL) 
    {
        fprintf(stderr, "Welp, out of memory DUMMY\n");
        fclose(ifp);
        exit(1);
    }
    
    unsigned long *faults = (unsigned long *)calloc(maxFrames + 1, sizeof(unsigned long));
    if (faults == NULL) 
    {
        fprintf(stderr, "Malloc said no...\n");
        pqFree(pq);
        fclose(ifp);
        exit(1);
    }

    // Process each memory access from the trace file
    while (!feof(ifp)) 
    {
        fread(&traceRecord, sizeof(p2AddrTr), 1, ifp);

        // Extract page number by shifting off the offset bits
        unsigned long pageNum = traceRecord.addr >> offsetBits;
        numAccesses++;

        // Pretend this memory access is real and hope it works
        long depth = pqAccess(pq, pageNum);
        
        if (depth == -1) 
        {
            // Fault for literally every frame count.
            for (int f = 1; f <= maxFrames; f++) {
                faults[f]++;
            }

        } 
        
        else 
        {
            // Page be chilling in memory at depth d
            // Time to count faults for any allocation that wasn't big enough to catch it IE with few than d+1 frames
            for (long f = 1; f <= depth; f++) 
            {
                faults[f]++;
            }

        }

        // Print progress indicator to stderr every PROGRESS_INTERVAL accesses
        // (also prints the last page number seen — useful for early debugging)
        if ((numAccesses % PROGRESS_INTERVAL) == 0) 
        {
            fprintf(stderr, "%lu samples read, last page: %lu\r", numAccesses, pageNum);
        }

    }

    fprintf(stderr, "\n%lu total accesses processed\n", numAccesses);

    // Output CSV results to stdout (redirect with > to create a .csv file)
    printf("Total Accesses:,%lu\n", numAccesses);
    printf("Frames,Missees,Miss Rate\n");

    //print spreadsheet....each frame count and its magnificent fault count
    for (int frameCount = 1; frameCount <= maxFrames; frameCount++) 
    {
        printf("%d,%lu,%f\n", frameCount, faults[frameCount], (double)faults[frameCount] / (double)numAccesses);
    }

    // Clean up clean up, everybody do your share, clean up clean up, please let there be no memory leaks ToT
    pqFree(pq);
    free(faults);
    fclose(ifp);

    return 0;
}
