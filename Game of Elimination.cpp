#include <iostream>

using namespace std;

struct Node
{
	int info;
	Node * next;
};

void print_list(Node * head)
{
    Node * current = head;
	while (current != NULL)
	{
		// process the current node, e.g., print the content
		cout << current->info << " -> ";
		current = current->next;
	}
	cout << "NULL\n";
}

void circular(Node * & head, Node * & tail) 
{
	tail->next = head;
}

void count (Node * & head, Node * & tail, int k)
{
	Node *p = head;
	Node *p_prev = NULL;
	Node *p_next = NULL;
	int i;
	while(head!=tail) {
		for (i=1;i<k;i++) {
			p_prev=p;
			p = p->next;
		}
		p_next = p->next;
		if (p==tail) {
			tail = p_prev;
		}
		if (p==head) {
			head = p->next;
		}
		free(p);
		p=p_prev->next = p_next; 
	}
}

void tail_insert(Node * & head,
	Node * & tail, int num)
{
	Node * p = new Node;
	p->info = num;
	p->next = NULL;

	if (head == NULL) {
		head = p;
		tail = p;
	}
	else {
		tail->next = p;
		tail = p;
	}

}



int main()
{
    Node * head = NULL, * tail = NULL;
    int num = 0;
    int i,k;
    // build linked list backward
    //cout << "input integers n, k";
    cin >> num >>k;
    for (i=0;i<num;i++) {
        tail_insert(head, tail, i+1);
    }
    circular (head, tail);

    count(head,tail,k); //eliminate contestants.
    cout<<head->info<<endl;
    free(head);
    return 0;
}

