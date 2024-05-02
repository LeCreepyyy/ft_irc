
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>

	class Channel {
		
		private :
			std::string				channel_name;
			std::string				channel_mode;
			

		public :
			Channel(std::string name);
			~Channel();

		// Accessors

			/*void			setName(std::string name);
			std::string		getName();

			void			setMode(std::string mode);
			std::string		getMode();*/
	};

#endif