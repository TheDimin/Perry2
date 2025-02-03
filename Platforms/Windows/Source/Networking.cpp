#include "pch.h"
#include "Enginepch.h"
#pragma hdrstop
using namespace Perry::Networking;

#ifdef NETWORK

#pragma hdrstop
#include "steam/isteamnetworkingsockets.h"
#include <steam/isteamnetworkingutils.h>



void SteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo) {
	if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_Connected) {
		std::cout << "Connected to peer!" << std::endl;
		Perry::GetEngine().GetNetwork()->clients.emplace_back("Client1", reinterpret_cast<void*>(pInfo->m_hConn));
	}
	else if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ClosedByPeer ||
		pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
		std::cout << "Connection closed." << std::endl;
	}
	else if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_Connecting)
	{
		auto resultCode = SteamNetworkingSockets()->AcceptConnection(pInfo->m_hConn);
		if (resultCode == k_EResultInvalidParam)
		{
			WARN(LOG_NETWORKING, "invalid parameter to network connection");
		}
	}
	else
		ERROR(LOG_NETWORKING, "Unhandled connection state: %d", pInfo->m_info.m_eState);
}

void Networking::Host()
{
	// Create a listen socket (server)
	SteamNetworkingIPAddr serverAddress;
	serverAddress.Clear();
	serverAddress.SetIPv6LocalHost(27015);
	HSteamListenSocket listenSocket = SteamNetworkingSockets()->CreateListenSocketIP(serverAddress, 0, nullptr);
	if (listenSocket == k_HSteamListenSocket_Invalid) {
		ERROR(LOG_NETWORKING, "Failed to create listen socket.");
		return;
	}


	clients.clear();
	clients.emplace_back("TODO", &listenSocket);

	// Set up callback
	SteamNetworkingUtils()->SetGlobalCallback_SteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback);
}

void Networking::Connect()
{
	SteamNetworkingIPAddr serverAddress;
	serverAddress.Clear();
	serverAddress.SetIPv6LocalHost(27015);
	SteamNetworkingSockets()->ConnectByIPAddress(serverAddress, 0, nullptr);
}
void Networking::Disconnect()
{
	SteamNetworkingSockets()->CloseListenSocket(*static_cast<HSteamListenSocket*>(clients[0].socket));
}
void Networking::Update()
{
	SteamNetworkingSockets()->RunCallbacks();


	if (IsClient())
	{
		auto message = SteamNetworkingUtils()->AllocateMessage(20);
		//strcpy(message->GetData(),"Hello World");
		SteamNetworkingSockets()->SendMessages(1, &message, 0);
	}
	else
	{
		for (auto& client : clients)
		{
			SteamNetworkingMessage_t* msg = SteamNetworkingUtils()->AllocateMessage(20);
			SteamNetworkingSockets()->ReceiveMessagesOnConnection(reinterpret_cast<HSteamNetConnection>(client.socket), &msg, 1);

			//WARN(LOG_NETWORKING, "Received message: %s", static_cast<const char*>(msg->GetData());

		}
	}
	/*
	for (auto region : World::GetWorld().ActiveRegions())
	{
		auto NetView = region->Registry.view<NetworkedComponent>();

		for (auto&& [e, netComponent] : NetView.each())
		{

		}
	}
	*/
}

#endif

void Networking::Host()
{
}

void Networking::Connect()
{

}
void Networking::Disconnect()
{

}

void Networking::Update()
{
}
bool Networking::IsHost() const
{
	return clients.size() > 0;
}
bool Networking::IsClient() const
{
	return clients.size() == 0;
}