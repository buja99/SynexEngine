#pragma once
class Framework {
public:
	virtual void Initialize();

	virtual void Finalize();

	virtual void Update();

	virtual void Draw() = 0;

	virtual bool GetIsEndReqest() { return isEndReqest_; }
protected:
	bool isEndReqest_ = false;
};

