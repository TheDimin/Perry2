#pragma once



class User
{
	//TODO user should have a netID
	std::string GetDisplayName() { return "TODO"; };
};


class Platform : public Perry::ISubSystem
{
public:
	~Platform();

	void Init() override;

	void Update(float delta) override;
	void ShutDown() override {};

private:
	User* localUser = nullptr;
};
