
#ifndef CHANNEL_HPP
# define CHANEL_HPP

# include "irc.hpp"

	class Channel {
		
		private :
			std::string			channel_name;
			std::string			channel_mode;
			

		public :
			Channel(std::string name);
			~Channel();
	};

#endif