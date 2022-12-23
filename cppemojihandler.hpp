// Deal with the fact that C++ can't work with emojis and read them in as one character

#pragma once
#pragma execution_character_set("utf-8")

#include <vector>
#include <unordered_set>
#include <unordered_map>

typedef std::string String;
typedef unsigned char Char;

static std::vector<std::unordered_set<Char>> create_emoji_bank(std::unordered_map<String, String> emojis)
{
	std::vector<std::unordered_set<Char>> res;
	for (std::unordered_map<String, String>::iterator iter = emojis.begin(); iter != emojis.end(); ++iter)
	{
		String k = iter->first;
		int i = 0;
		for (Char c : k)
		{
			if (i == res.size())
			{
				res.push_back(std::unordered_set<Char>());
				res[i].insert(c);
			}
			else
				res[i].insert(c);
			i++;
		}
	}
	return res;
}

static inline String emoji_chars_from_start(String s, std::vector<std::unordered_set<Char>> bank)
{
	String res = u8"";
	for (int i = 0; i < s.length() && bank[i].count(s[i]); i++)
		res += s[i];
	return res;
}