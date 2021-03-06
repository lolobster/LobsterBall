#ifndef BOOK_GAMESERVER_HPP
#define BOOK_GAMESERVER_HPP

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Thread.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <vector>
#include <memory>
#include <map>


class GameServer
{
	public:
		explicit							GameServer(sf::Vector2f battlefieldSize);
											~GameServer();

		void								notifyPlayerSpawn(sf::Int32 playerIdentifier);
		void								notifyPlayerRealtimeChange(sf::Int32 playerIdentifier, sf::Int32 action, bool actionEnabled);
		void								notifyPlayerEvent(sf::Int32 playerIdentifier, sf::Int32 action);


	private:
		// A GameServerRemotePeer refers to one instance of the game, may it be local or from another computer
		struct RemotePeer
		{
									RemotePeer();

			sf::TcpSocket			socket;
			sf::Time				lastPacketTime;
			std::vector<sf::Int32>	playerBatIdentifiers;
			bool					ready;
			bool					timedOut;
		};

		// Structure to store information about current player state
		struct PlayerInfo
		{
			sf::Vector2f				position;
			std::map<sf::Int32, bool>	realtimeActions;
		};

		// Structure to store information about ball state
		struct BallInfo
		{
			sf::Vector2f				position;
		};

		struct GoalInfo
		{
			sf::Int32 goalHP;
		};

		// Unique pointer to remote peers
		typedef std::unique_ptr<RemotePeer> PeerPtr;


	private:
		void								setListening(bool enable);
		void								executionThread();
		void								tick();
		sf::Time							now() const;

		void								handleIncomingPackets();
		void								handleIncomingPacket(sf::Packet& packet, RemotePeer& receivingPeer, bool& detectedTimeout);

		void								handleIncomingConnections();
		void								handleDisconnections();

		void								informWorldState(sf::TcpSocket& socket);
		void								broadcastMessage(const std::string& message);
		void								sendToAll(sf::Packet& packet);
		void								updateClientState();


	private:
		sf::Thread							mThread;
		sf::Clock							mClock;
		sf::TcpListener						mListenerSocket;
		bool								mListeningState;
		sf::Time							mClientTimeoutTime;

		std::size_t							mMaxConnectedPlayers;
		std::size_t							mConnectedPlayers;

		float								mWorldHeight;
		sf::FloatRect						mPlayAreaRect;

		std::size_t							mPlayerBatCount;
		std::map<sf::Int32, PlayerInfo>	mPlayerBatInfo;
		std::map<sf::Int32, GoalInfo>	mGoalInfo;
		BallInfo	mBallInfo;

		std::vector<PeerPtr>				mPeers;
		sf::Int32							mPlayerBatIdentifierCounter;
		bool								mWaitingThreadEnd;
		
		sf::Time							mLastSpawnTime;
		sf::Time							mTimeForNextSpawn;
};

#endif // BOOK_GAMESERVER_HPP
