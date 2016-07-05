#ifndef NM_SERVER_HPP
#define NM_SERVER_HPP

#include "ConnectionManager.hpp"
#include <boost/bimap.hpp>
#include <nm/Game.hpp>

namespace nm
{
namespace server
{
	class Server
	{
		typedef boost::bimap<Connection::ptr, int32_t> client_bimap;
		typedef client_bimap::value_type client_pair;
		public:
			Server();
			void start();
			void poll();

			/* event handlers */
			void player_join_handler(Connection::ptr connection, const message::PlayerJoin& msg);
			void cursor_move_handler(Connection::ptr connection, const message::CursorMove& msg);
			void square_open_handler(Connection::ptr connection, const message::SquareOpen& msg);
			void square_flag_handler(Connection::ptr connection, const message::SquareFlag& msg);
			void player_quit_handler(Connection::ptr connection);

		private:
			ConnectionManager connectionManager;
			client_bimap clients;
			Game game;
	};
}
}

#endif