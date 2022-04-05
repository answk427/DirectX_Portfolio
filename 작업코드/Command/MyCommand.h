#pragma once
#include <assert.h>


class MyCommand
{
public:
	MyCommand() {}
	virtual ~MyCommand() {}
	virtual void Execute() = 0;
	virtual void Undo() = 0;
};


class CommandQueue
{
public:
	CommandQueue() {}
	~CommandQueue();
	static void init()
	{
		head = 0;
		tail = 0;
	}
	static void AddCommand(MyCommand* command);
	static void Undo();
	static void AllExecute();
private:
	static const int QUEUE_SIZE = 2000;
	static MyCommand* queue[QUEUE_SIZE];
	static int head; //���� �����ؾ� �� ��� ��ġ
	static int tail; //���ο� ����� �߰��� ��ġ
};