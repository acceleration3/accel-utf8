#ifndef ACCEL_UTF8_HEADER
#define ACCEL_UTF8_HEADER

#include <string>

#include <accel/macros>

namespace accel
{
	namespace utf8
	{
		namespace details
		{
			inline std::size_t get_utf_codepoint_size(unsigned char first)
			{
				std::uint8_t mask = 0x80u;
				unsigned result = 0;
				while (first & mask)
				{
					++result;
					mask >>= 1;
				}
				return (result == 0) ? 1 : ((result > 4) ? 4 : result);
			}
		}

		class string;

		class string_iterator
		{
		public:
			explicit string_iterator(const std::string& str, std::size_t index = 0) :
				m_str(str),
				m_index(index) {}

			string_iterator& operator++()
			{
				advance();
				return *this;
			}

			bool operator==(const string_iterator& other) const
			{
				return &m_str == &other.m_str && m_index == other.m_index;
			}

			bool operator!=(const string_iterator& other) const
			{
				return !(*this == other);
			}

			string operator*() const;

		private:
			const std::string& m_str;
			std::size_t m_index;

			void advance() 
			{
				m_index += details::get_utf_codepoint_size(m_str[m_index]);
			}
		};

		class string
		{
		public:
			using iterator = string_iterator;
			using const_iterator = const string_iterator;

			explicit string(const std::string& str) : m_str(str) {}
			explicit string(const char* str) : m_str(str) {}

			template<std::size_t Size>
			string(const char (&str)[Size]) : m_str(str) {}

			ACC_DEFAULT_COPYABLE(string);
			ACC_DEFAULT_MOVABLE(string);
			
			string operator+(const string& other)
			{
				return string(m_str + other.m_str);
			}

			string operator+(const std::string& other)
			{
				return string(m_str + other);
			}

			string operator[](std::size_t index) const
			{
				std::size_t offset = index_to_offset(index);
				return string(m_str.substr(offset, details::get_utf_codepoint_size(m_str[offset])));
			}

			bool operator==(const string& other) const
			{
				return length() == other.length() && m_str == other.m_str;
			}

			bool operator!=(const string& other) const
			{
				return !operator==(other);
			}

			bool operator==(const char* memory) const
			{
				return m_str == memory;
			}

			bool operator!=(const char* memory) const 
			{
				return m_str != memory;
			}

			bool operator==(const std::string& str) const
			{
				return m_str == str;
			}

			bool operator!=(const std::string& str) const
			{
				return m_str != str;
			}

			iterator begin() { return iterator(m_str); }
			const_iterator cbegin() const { return const_iterator(m_str); }

			iterator end() { return iterator(m_str, data_size()); }
			const_iterator cend() const { return const_iterator(m_str, data_size()); }

			std::size_t data_size() const { return m_str.size(); }

			std::size_t length() const
			{
				std::size_t length = 0;
				std::size_t index = 0;
				while (index < m_str.length())
				{
					length++;
					index += details::get_utf_codepoint_size(m_str[index]);
				}
				return length;
			}

			const char* data() const { return &m_str[0]; }
			char* data() { return &m_str[0]; }

			std::wstring to_wstring() const
			{
				std::wstring wide_string;
				wide_string.reserve(m_str.length());

				for (std::size_t i = 0; i < m_str.length();) 
				{
					char32_t codepoint = 0;
					std::size_t bytes_read = 0;

					if ((m_str[i] & 0b10000000) == 0b00000000) 
					{
						codepoint = m_str[i];
						bytes_read = 1;
					}
					else if ((m_str[i] & 0b11100000) == 0b11000000)
					{
						codepoint = m_str[i] & 0b00011111;
						bytes_read = 2;
					}
					else if ((m_str[i] & 0b11110000) == 0b11100000) 
					{
						codepoint = m_str[i] & 0b00001111;
						bytes_read = 3;
					}
					else if ((m_str[i] & 0b11111000) == 0b11110000)
					{
						codepoint = m_str[i] & 0b00000111;
						bytes_read = 4;
					}
					else 
					{
						throw std::invalid_argument("Invalid UTF-8 sequence");
					}

					// Read the remaining bytes of the UTF-8 sequence
					for (std::size_t j = 1; j < bytes_read; ++j)
					{
						if ((m_str[i + j] & 0b11000000) != 0b10000000)
							throw std::invalid_argument("Invalid UTF-8 sequence");

						codepoint <<= 6;
						codepoint |= m_str[i + j] & 0b00111111;
					}

					if (codepoint <= 0xFFFF) 
					{
						wide_string.push_back(static_cast<wchar_t>(codepoint));
					}
					else if (codepoint <= 0x10FFFF) 
					{
						//To calculate the surrogate pair, we subtract 0x10000 from the Unicode code point and split the remaining bits into the high and low surrogate.
						//To create the high surrogate, we take the bits from the range 0-9 (inclusive) and add 0xD800.	
						//To create the low surrogate, we take the bits from the range 10-15 (inclusive) and add 0xDC00.

						codepoint -= 0x10000;
						wide_string.push_back(static_cast<wchar_t>((codepoint >> 10) + 0xD800));
						wide_string.push_back(static_cast<wchar_t>((codepoint & 0x3FF) + 0xDC00));
					}
					else 
					{
						throw std::invalid_argument("Invalid UTF-8 codepoint");
					}

					i += bytes_read;
				}

				return wide_string;
			}

			string substr(std::size_t start = 0, std::size_t length = std::string::npos) const
			{
				ACC_ASSERT(start < this->length());
				ACC_ASSERT(start + length < this->length());

				std::size_t start_offset = index_to_offset(start);
				std::size_t end_offset = index_to_offset(start + length);
				return string(m_str.substr(start_offset, end_offset - start_offset));
			}

			void clear()
			{
				m_str.clear();
			}

		private:
			std::string m_str;

			std::size_t index_to_offset(std::size_t index) const
			{
				ACC_ASSERT(index < length());

				std::size_t current = 0;
				std::size_t byte_index = 0;
				while (current < index)
				{
					current++;
					byte_index += details::get_utf_codepoint_size(m_str[byte_index]);
				}

				return byte_index;
			}
		};

		string string_iterator::operator*() const
		{
			auto portion = m_str.substr(m_index, details::get_utf_codepoint_size(m_str[m_index]));
			return string(portion);
		}
	}
}

#endif