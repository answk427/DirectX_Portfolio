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
	static int head; //현재 실행해야 할 명령 위치
	static int tail; //새로운 명령이 추가될 위치
};