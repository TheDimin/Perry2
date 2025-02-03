#pragma once

namespace Perry
{

	namespace Networking
	{
		struct Client
		{
			std::string ID;
			void* socket = nullptr;
		};

		class Networking : public ISubSystem
		{
		public:
			void Host();
			void Connect();
			void Disconnect();

			void Update();

			bool IsHost() const;
			bool IsClient() const;

			std::vector<Client> clients;
		};
	}

	struct NetworkedComponent
	{
		//NETID
		//PRIORITY
		unsigned int NetworkID = 0;
		unsigned char Priority = 0;
		unsigned short OwnerID = 0; //ID into clients map
	};
}

