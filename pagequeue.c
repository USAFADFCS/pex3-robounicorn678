/** pagequeue.c
 * ===========================================================
 * Name: C2C Charles liermann, 23 APR 2026
 * Section: CS483 / M4
 * Project: PEX3 - Page Replacement Simulator
 * Purpose: Implementation of the PageQueue ADT — a doubly-linked
 *          list for LRU page replacement.
 *          Head = LRU (eviction end), Tail = MRU end.
 * =========================================================== */
#include <stdio.h>
#include <stdlib.h>

#include "pagequeue.h"

/**
 * @brief Create and initialize a page queue with a given capacity
 */
PageQueue *pqInit(unsigned int maxSize) 
{
    PageQueue *pq = (PageQueue *)malloc(sizeof(PageQueue));

    if (pq == NULL) 
    {
        return NULL;
    }


    pq->head = NULL;
    pq->tail = NULL;
    pq->size = 0;
    pq->maxSize = maxSize;
    return pq;
}

/**
 * @brief Access a page in the queue (simulates a memory reference)
 */
long pqAccess(PageQueue *pq, unsigned long pageNum) {
    // taverse the DLL from the butt, count the depths, find pagenum, move to tail give back depth
    // if go to end without finding, add new node to tail, if size exceeds max, evict head, return -1 for fault
    long depth = 0;
    PqNode *current = pq->tail;
    
    while (current != NULL) 
    {

        if (current->pageNum == pageNum) 
        {
            // FOUND IT! page here! move it to back. rearrange whole mess now.
            if (current->prev != NULL) 
            {
                current->prev->next = current->next;
            } 
            
            else 
            {
                // thing at head. 
                pq->head = current->next;
            }
            
            if (current->next != NULL) 
            {
                current->next->prev = current->prev;
            } 
            
            
            else 
            {
                // thing at tail. bad. no happen physics broken now
                pq->tail = current->prev;
            }
            
            // move popular thing to tail. 
            current->prev = pq->tail;
            current->next = NULL;
            


            if (pq->tail != NULL) 
            {
                pq->tail->next = current;
            }

            pq->tail = current;
            
            // lonely thing? be start and end both. do two thing one time
            if (pq->head == NULL) 
            {
                pq->head = current;
            }
            
            return depth;

        }
        
        current = current->prev;


        depth++;
    }
    
    // page not here, make new thing
    PqNode *newNode = (PqNode *)malloc(sizeof(PqNode));
    
    if (newNode == NULL) 
    {
        return -1;
    }
    
    newNode->pageNum = pageNum;
    newNode->prev = pq->tail;
    newNode->next = NULL;
    
    // put new thing at tail. 
    if (pq->tail != NULL) 
    {
        pq->tail->next = newNode;
    }

    pq->tail = newNode;
    
    // wasteland queue? now start and end both do it yourself, yay.
    if (pq->head == NULL) 
    {
        pq->head = newNode;
    }
    
    pq->size++;
    
    // too many things now kill thing at head
    if (pq->size > pq->maxSize) 
    {
        PqNode *oldHead = pq->head;

        pq->head = oldHead->next;

        if (pq->head != NULL) 
        {
            pq->head->prev = NULL;
        } 
        
        else 
        {
            // Queue empty no thought head empty
            pq->tail = NULL;
        }


        free(oldHead);
        pq->size--;
    
    
    }
    
    return -1;
}

/**
 * @brief Free all nodes in the queue and reset it to empty
 */
void pqFree(PageQueue *pq) {
   
    if (pq == NULL) 
    {
        return;
    }
    
    PqNode *current = pq->head;

    // walk from head to tail FREE ALL THE MEMORY PLEASE NO MEMOERY LEAKS!!!
    while (current != NULL)
    {
        PqNode *temp = current;
        current = current->next;
        free(temp);
    }
    
    free(pq);
}

/**
 * @brief Print queue contents to stderr for debugging
 */
void pqPrint(PageQueue *pq) {
    
    if (pq == NULL) 
    {
        fprintf(stderr, "Queue is NULL\n");
        return;
    }
    
    fprintf(stderr, "Queue (size=%u, maxSize=%u): ", pq->size, pq->maxSize);
    
    if (pq->head == NULL) 
    {
        fprintf(stderr, "[empty]\n");
        return;
    }
    
    fprintf(stderr, "[HEAD] ");
    PqNode *current = pq->head;
    while (current != NULL) 
    {
        fprintf(stderr, "%lu", current->pageNum);

        if (current->next != NULL) 
        {
            fprintf(stderr, " <-> "); 
        }


        current = current->next;
    }



    
    fprintf(stderr, " [TAIL]\n");
}
