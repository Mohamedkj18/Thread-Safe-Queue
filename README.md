
# Thread-Safe Queue Implementation

This repository provides an implementation of a thread-safe queue in C, using synchronization primitives such as mutexes and condition variables. The queue is designed to handle concurrent operations safely in a multithreaded environment.

## Features
- **Thread-Safe**: Supports multiple threads accessing the queue simultaneously without race conditions.
- **Blocking Dequeue**: Threads can wait for items to be enqueued if the queue is empty.
- **Non-Blocking Dequeue**: Supports non-blocking dequeue operations.
- **Synchronization**: Utilizes mutexes (`mtx_t`) and condition variables (`cnd_t`) for thread safety.
- **Queue Statistics**: Tracks the total number of dequeued items.

## Files
- **`queue.c`**: Contains the queue implementation.

## API Reference

### Queue Structure
```c
typedef struct QueueNode {
    void *data;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *head;
    QueueNode *tail;
    size_t visited;
    mtx_t mutex;
    cnd_t notEmpty;
} Queue;
```

### Functions

#### `void initQueue()`
Initializes the queue and its synchronization primitives.

#### `void destroyQueue()`
Cleans up the queue, freeing all resources.

#### `void enqueue(void *item)`
Adds a new item to the end of the queue.  
**Parameters**:
- `item`: A pointer to the data to be enqueued.

#### `void *dequeue(void)`
Removes and returns the first item from the queue. Blocks if the queue is empty.  
**Returns**: A pointer to the dequeued data.

#### `bool tryDequeue(void **item)`
Tries to remove an item from the queue without blocking.  
**Parameters**:
- `item`: A pointer to a location where the dequeued data will be stored.  
**Returns**: `true` if an item was dequeued, `false` if the queue was empty.

#### `size_t visited(void)`
Returns the total number of items that have been dequeued.

## Usage

### Initialization
```c
initQueue();
```

### Enqueue
```c
int data = 42;
enqueue(&data);
```

### Dequeue (Blocking)
```c
int *data = (int *)dequeue();
printf("Dequeued: %d
", *data);
```

### Dequeue (Non-Blocking)
```c
void *data;
if (tryDequeue(&data)) {
    printf("Dequeued: %d
", *(int *)data);
} else {
    printf("Queue is empty.
");
}
```

### Cleanup
```c
destroyQueue();
```

## Thread Safety
- Mutex locks ensure safe access to the queue in a multithreaded environment.
- Condition variables allow threads to wait efficiently when the queue is empty.

## Build and Run
1. Compile the code using a C compiler (e.g., `gcc`):
   ```sh
   gcc -o queue queue.c -pthread
   ```
2. Run the compiled executable:
   ```sh
   ./queue
   ```

## License
This project is open-source and available under the MIT License. See `LICENSE` for details.
