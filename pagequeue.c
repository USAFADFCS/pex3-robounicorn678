/** pagequeue.c
 * ===========================================================
 * Name: _______________________, __ ___ 2026
 * Section: CS483 / ____
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
PageQueue *pqInit(unsigned int maxSize) {
    PageQueue *pq = (PageQueue *)malloc(sizeof(PageQueue));
    if (pq == NULL) {
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
    // Search from tail backwards to find the page (count depth from MRU end)
    long depth = 0;
    PqNode *current = pq->tail;
    
    while (current != NULL) {
        if (current->pageNum == pageNum) {
            // FOUND: Page hit at depth 'd' from the MRU end
            // Remove node from its current position
            if (current->prev != NULL) {
                current->prev->next = current->next;
            } else {
                // current is the head
                pq->head = current->next;
            }
            
            if (current->next != NULL) {
                current->next->prev = current->prev;
            } else {
                // current is the tail (shouldn't happen if depth > 0)
                pq->tail = current->prev;
            }
            
            // Re-insert at the tail (most recently used)
            current->prev = pq->tail;
            current->next = NULL;
            
            if (pq->tail != NULL) {
                pq->tail->next = current;
            }
            pq->tail = current;
            
            // If this was the only node, it's both head and tail
            if (pq->head == NULL) {
                pq->head = current;
            }
            
            return depth;
        }
        
        current = current->prev;
        depth++;
    }
    
    // PAGE NOT FOUND: Create a new node for this page
    PqNode *newNode = (PqNode *)malloc(sizeof(PqNode));
    if (newNode == NULL) {
        return -1;
    }
    
    newNode->pageNum = pageNum;
    newNode->prev = pq->tail;
    newNode->next = NULL;
    
    // Insert at the tail (most recently used)
    if (pq->tail != NULL) {
        pq->tail->next = newNode;
    }
    pq->tail = newNode;
    
    // If queue was empty, this is also the head
    if (pq->head == NULL) {
        pq->head = newNode;
    }
    
    pq->size++;
    
    // If we exceed maxSize, evict the head (LRU page)
    if (pq->size > pq->maxSize) {
        PqNode *oldHead = pq->head;
        pq->head = oldHead->next;
        if (pq->head != NULL) {
            pq->head->prev = NULL;
        } else {
            // Queue is now empty (shouldn't happen if maxSize >= 1)
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
    if (pq == NULL) {
        return;
    }
    
    PqNode *current = pq->head;
    while (current != NULL) {
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
    if (pq == NULL) {
        fprintf(stderr, "Queue is NULL\n");
        return;
    }
    
    fprintf(stderr, "Queue (size=%u, maxSize=%u): ", pq->size, pq->maxSize);
    
    if (pq->head == NULL) {
        fprintf(stderr, "[empty]\n");
        return;
    }
    
    fprintf(stderr, "[HEAD] ");
    PqNode *current = pq->head;
    while (current != NULL) {
        fprintf(stderr, "%lu", current->pageNum);
        if (current->next != NULL) {
            fprintf(stderr, " <-> ");
        }
        current = current->next;
    }
    fprintf(stderr, " [TAIL]\n");
}
