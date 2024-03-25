#pragma once
class Single
{
public:
	~Single();

	static Single* getInstance();
private:
	Single();
};

