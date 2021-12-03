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
	//큐에 빈공간이 없는지 검사
	assert((tail + 1) % QUEUE_SIZE != head);

	//배열 맨 뒤에 추가
	if (queue[tail] != nullptr)
		delete queue[tail];

	queue[tail] = command;
	tail = (tail + 1) % QUEUE_SIZE;
}

void CommandQueue::Undo()
{
	//head(실행할 위치)가 최근명령으로 한바퀴 돌아온 경우 검사
	assert((head - 1 + QUEUE_SIZE) % QUEUE_SIZE != tail);
	
	if(queue[(head - 1+QUEUE_SIZE) % QUEUE_SIZE] == nullptr)
		return;

	head = (head - 1)%QUEUE_SIZE;
	
	//Undo로 이전으로 돌아간 시점부터 새로시작
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
