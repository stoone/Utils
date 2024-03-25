#pragma once

class model
{
public:
	model();
	~model();

	static model* getInstance();

	void print(int n);
};

