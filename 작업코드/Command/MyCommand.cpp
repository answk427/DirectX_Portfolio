#include "MyCommand.h"

int CommandQueue::head = 0;
int CommandQueue::tail = 0;
MyCommand* CommandQueue::queue[QUEUE_SIZE];

CommandQueue::~CommandQueue()
{
	for(int i = 0; i < QUEUE_SIZE; i++)
	{
		if (queue[i] != nullptr)
			delete queue[i];
	}
}

void CommandQueue::AddCommand(MyCommand * command)
{
	//ť�� ������� ������ �˻�
	assert((tail + 1) % QUEUE_SIZE != head);

	//�迭 �� �ڿ� �߰�
	if (queue[tail] != nullptr)
		delete queue[tail];

	queue[tail] = command;
	tail = (tail + 1) % QUEUE_SIZE;
}

void CommandQueue::Undo()
{
	//head(������ ��ġ)�� �ֱٸ������ �ѹ��� ���ƿ� ��� �˻�
	assert((head - 1 + QUEUE_SIZE) % QUEUE_SIZE != tail);
	
	if(queue[(head - 1+QUEUE_SIZE) % QUEUE_SIZE] == nullptr)
		return;

	head = (head - 1)%QUEUE_SIZE;
	
	//Undo�� �������� ���ư� �������� ���ν���
	tail = head;

	queue[head]->Undo();
}

void CommandQueue::AllExecute()
{
	while(head != tail)
	{
		queue[head]->Execute();
		head = (head + 1) % QUEUE_SIZE;
	}
}
