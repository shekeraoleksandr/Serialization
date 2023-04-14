#include "core.h"


namespace Core
{

	namespace Util
	{

		bool isLittleEndian()
		{
			// 0x00 0x00 0x00 0b0000 0101
			uint8_t a = 5;
			std::string result = std::bitset<8>(a).to_string();
			return (result.back() == '1') ? true : false;
		}

		void save(const char* file, std::vector<uint8_t>& buffer)
		{
			std::ofstream out;
			out.open(file);

			for (unsigned i = 0; i < buffer.size(); i++)
			{
				out << buffer[i];
			}

			out.close();
		}

		void retriveNsave(ObjectModel::Root* r)
		{
			int16_t iterator = 0;
			std::vector<uint8_t> buffer(r->getSize());
			std::string name = r->getName().substr(0, r->getName().length()).append(".abc");
			r->pack(buffer, iterator);
			save(name.c_str(), buffer);
		}
	}
}