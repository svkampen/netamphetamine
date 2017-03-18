#ifndef NM_SERVER_HPP
#define NM_SERVER_HPP

#include "ConnectionManager.hpp"
#include <boost/bimap.hpp>
#include <nm/Game.hpp>

namespace std
{
	template<>
	struct hash<nm::message::Player>
	{
		typedef nm::message::Player argument_type;
		std::size_t operator()(argument_type const& c) const
		{
			std::size_t const h1(std::hash<int32_t>()(c.x()));
			std::size_t const h2(std::hash<int32_t>()(c.y()));
			std::size_t const h3(std::hash<int32_t>()(c.id()));

			return (h1 ^ (h2 + h3));
		}
	};
}

namespace nm
{
namespace server
{
	/**
	 * The netmine server, which takes care of all the minesweeper-specific networking.
	 *
	 * Not to be confused with the game implementation or the non-specific networking classes.
	 * @see Game
	 * @see ConnectionManager
	 */
	class Server
	{
		public:
			/**
			 * Construct a new server with a given game instance.
			 *
			 * @param game The game instance.
			 */
			Server(Game &game);

			/**
			 * Construct a new server on a non-standard port.
			 *
			 * @param game The game instance.
			 * @param port The port.
			 */
			Server(Game &game, int port);

			/**
			 * Run the server, starting its connection manager.
			 */
			void start();



			/**
			 * The handler for chunk requests.
			 *
			 * This handler simply sends a chunk update for the requested chunk back to the connection.
			 *
			 */
			void chunk_request_handler(Connection::ptr connection, const message::ChunkRequest& msg);

			/**
			 * The handler for player joins.
			 *
			 * This handler generates a new Player instance, mapping it to the given connection.
			 * It subsequently sends a welcome message to the new client and relays a player join
			 * to all other clients.
			 *
			 */
			void player_join_handler(Connection::ptr connection, const message::Player& msg);

			/**
			 * The handler for cursor moves.
			 * 
			 * Since netmine shows other players' cursors in real time, this handler is necessary
			 * to relay cursor moves to other clients. Additionally, this updates the server-local
			 * statistics for the player.
			 *
			 */
			void cursor_move_handler(Connection::ptr connection, const message::CursorMove& msg);

			/**
			 * The handler for square opens.
			 *
			 * This handler attempts to open the square specified in the SquareOpen message.
			 * If this leads to a non-empty set of updated chunks, these are sent as regular chunk updates
			 * to all clients. When a square cannot be opened (i.e. it is already opened) no chunk updates
			 * are relayed.
			 *
			 */
			void square_open_handler(Connection::ptr connection, const message::SquareOpen& msg);

			/**
			 * The handler for square flags.
			 *
			 * This handler attempts to flag the square specified in the SquareFlag message.
			 * The affected chunk is relayed to all clients. If a square cannot be flagged (i.e. it is already
			 * opened) no chunk updates are relayed.
			 *
			 */
			void square_flag_handler(Connection::ptr connection, const message::SquareFlag& msg);

			/**
			 * The handler for ClearAt messages.
			 *
			 * A ClearAt message is a strange concept in the world of minesweeper. It notifies a server
			 * to clear part of the chunk of mines in the vicinity of the cursor, since this is behavior
			 * seen in other versions of minesweepers (when you first open a square, you never hit a mine).
			 *
			 */
			void clear_at_handler(Connection::ptr connection, const message::ClearAt& clearAt);

			/**
			 * The handler for player quits.
			 *
			 * When a player quits, the quit message is relayed to all clients and the connection
			 * in question is removed from the client set.
			 */
			void player_quit_handler(Connection::ptr connection, const message::Player& player);

		private:
			ConnectionManager connectionManager;
			std::unordered_map<Connection::ptr, message::Player> clients{};
			Game &game;

			void send_chunk_update(int x, int y, const Chunk& chunk);
			void send_chunk_update(int x, int y);
	};
}
}

#endif
