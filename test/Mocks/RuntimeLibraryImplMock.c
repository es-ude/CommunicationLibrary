#include <stdlib.h>
#include "test/Mocks/RuntimeLibraryImplMock.h"


size_t *input_value;

void *returned_pointer;

double last_delay_microseconds_arg = 0;

void * MockAllocate_allocate(size_t bytes_allocated) {
  *input_value = bytes_allocated;
  return returned_pointer;
}

void MockAllocate_configure(MockAllocateConfig *config){
  input_value = &config->size_allocated;
  returned_pointer = config->returned_address;
}

void MockRuntime_delayMicroseconds(double microseconds) {
  last_delay_microseconds_arg = microseconds;
}

double MockRuntime_lastDelayMicrosecondsArg(void) {
  return last_delay_microseconds_arg;
}

typedef struct MockMemoryNode {
  struct MockMemoryNode *next;
  void *allocated_memory;
  size_t size_of_memory;
} MockMemoryNode;

typedef struct MockMemory {
  MemoryManagement interface;
  MockMemoryNode *first;
} MockMemory;



static void *allocate(size_t size);
static void deallocate(void *pointer);

static bool isEmpty(MockMemory *impl);
static MockMemoryNode *createNewNode(size_t size);
static MockMemoryNode *findTail(MockMemoryNode *head);
static MockMemoryNode *findNode(MockMemory *impl, void *pointer);

static MockMemory memory = {
        .interface.allocate = allocate,
        .interface.deallocate = deallocate,
        .first = NULL,
};

void *allocate(size_t size) {
  if (isEmpty(&memory)) {
    memory.first = createNewNode(size);
    return memory.first->allocated_memory;
  }
  else {
    MockMemoryNode *tail = findTail(memory.first);
    tail->next = createNewNode(size);
    return tail->next->allocated_memory;
  }
}

MockMemoryNode *createNewNode(size_t size) {
  MockMemoryNode *node = malloc(sizeof(MockMemoryNode));
  node->allocated_memory = malloc(size);
  node->next = NULL;
  node->size_of_memory = size;
  return node;
}

MockMemoryNode *findTail(MockMemoryNode *head) {
  while (head->next != NULL) {
    head = head->next;
  }
  return head;
}


void deallocate(void *pointer) {
  MockMemoryNode *node = memory.first;
  if (node->allocated_memory == pointer) {
    free(node->allocated_memory);
    memory.first = node->next;
    free(node);
  }
  else {
    while (node->next != NULL && node->next->allocated_memory != pointer) {
      node = node->next;
    }
    if (node->next != NULL) {
      MockMemoryNode *node_to_be_removed = node->next;
      node->next = node_to_be_removed->next;
      free(node_to_be_removed->allocated_memory);
      free(node_to_be_removed);
    }
  }
}

MemoryManagement *MemoryManagement_createMockImpl(void) {
  MemoryManagement *self = (MemoryManagement *) &memory;
  return self;
}

uint16_t MockMemoryManagement_numberOfAllocatedObjects(MemoryManagement *self) {
  MockMemory *impl = (MockMemory *) self;
  MockMemoryNode *current = impl->first;
  uint16_t counter = 0;
  while (current != NULL) {
    counter++;
    current = current->next;
  }
  return counter;
}

size_t MockMemoryManagement_size(MemoryManagement *self, void *pointer) {
  MockMemory *impl = (MockMemory *) self;
  MockMemoryNode *current = findNode(impl, pointer);
  if (current == NULL) {
    return 0;
  }
  else {
    return current->size_of_memory;
  }
}

MockMemoryNode *findNode(MockMemory *impl, void *pointer) {
  MockMemoryNode *current = impl->first;
  while (current != NULL && current->allocated_memory != pointer) {
    current = current->next;
  }
  return current;
}

bool isEmpty(MockMemory *impl) {
  return impl->first == NULL;
}