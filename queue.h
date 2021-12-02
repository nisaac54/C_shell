struct node {				
       int pid;
       char *name;
       struct node *next;	
};


struct queue {				
       struct node *head;
       struct node *tail;
};


// Function to create a new node 
struct node *newNode(int item, char *s){
       struct node* temp = (struct node*)malloc(sizeof(struct node));
       
       temp->pid = item; 
       temp->name = s;
       temp->next = NULL;
       return temp;
       
};


void enqueue(int item, char *s, struct queue *q){		
     // Enqueue a process

       struct node *temp = newNode(item, s);
       if (q->tail == NULL){
              
            q->head = q->tail = temp;

       }
       q->tail->next = temp;
       q->tail = temp;
       return;
}


void delete(struct queue *q, int key) {
	// Deallocate memory of a process after it completes its work

       // check if list is empty
       if(q->head == NULL){
              q->head = NULL;
              q->tail = NULL; // if head is null tail is null
              printf("List is empty\n");
              return;
       }

       struct node *temp = q->head;

       // if node to remove is at head       
       if(q->head->pid == key){
           q->head = q->head->next;
           free(temp);
       }
       else{
   
           struct node *current = q->head->next;
           struct node *previous = q->head;
           struct node *temp = q->head->next;

              // find node to remove during iteration
              while(current!=NULL){

                     // if node to remove is the tail
                     if(current->pid == key && current->next == NULL){ 
                            previous->next = current->next;
                            printf("Freed: %d\n", current->pid);
                            free(current);
                            q->tail = previous;
                            previous->next = NULL;
                            return;
                     }
                     // if node to remove is not tail 
                      if(current->next != NULL && current->pid == key){
                            previous = current;
                            current = current->next;

                            // if node to remove is the middle
                            if(current->next == NULL){
                                   if(temp->next->next == NULL){
                                   q->tail = current;
                                   current->next = NULL;
                                   q->head->next = current;
                                   printf("Freed: %d\n", previous->pid);
                                   free(previous);
                                   }
                                   else{
                                          // if node to remove is the next to tail
                                          q->tail = current;
                                          current->next = NULL;
                                          temp->next = current;
                                          printf("Freed: %d\n", previous->pid);
                                          free(previous);

                                   }
                            }
                            else{
                                   // if node to remove is next to head 
                                   q->head->next = current;
                                   current->next = q->tail;
                                   printf("Freed: %d\n", previous->pid);
                                   free(previous);
                            }
                     }
                     else{
                            temp = previous = current;
                            current = current->next;
                     }
              }
       }
}
