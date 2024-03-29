#pragma once
#include "linked_list.h"
#include "disastrOS_pcb.h"
#include "disastrOS_resource.h"
#include "disastrOS_mq.h"


struct DescriptorPtr;

typedef struct Descriptor{
  ListItem list;
  PCB* pcb;
  Resource* resource;
  MessageQueue* mq;
  int fd;
  struct DescriptorPtr* ptr; // pointer to the entry in the resource list
} Descriptor;

typedef struct DescriptorPtr{
  ListItem list;
  Descriptor* descriptor;
} DescriptorPtr;

void Descriptor_init();
Descriptor* Descriptor_alloc(int fd, Resource* res, PCB* pcb);
int Descriptor_free(Descriptor* d);
Descriptor*  DescriptorList_byFd(ListHead* l, int fd);
void DescriptorList_print(ListHead* l);

DescriptorPtr* DescriptorPtr_alloc(Descriptor* descriptor);
int DescriptorPtr_free(DescriptorPtr* d);
void DescriptorPtrList_print(ListHead* l);

Descriptor* Descriptor_alloc_mq(int fd, MessageQueue* res, PCB* pcb);
void DescriptorPtrList_print_mq(ListHead* l);
