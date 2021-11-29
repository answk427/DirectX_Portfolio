#pragma once


class MyCommand
{
public:
	MyCommand() {}
	virtual ~MyCommand() {}
	virtual void Execute() = 0;
	virtual void Undo() = 0;
};
