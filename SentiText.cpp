// implements SentiText class
#include "SentiText.hpp"
#include <string>

namespace vader
{
   SentiText::SentiText(String text)
        : m_text(text)
    {
        m_words_and_emoticons = this->_words_and_emoticons();
        // doesn't separate words from\
        // adjacent punctuation (keeps emoticons & contractions)
        this->m_is_cap_diff = allcap_differential(this->m_words_and_emoticons);  
    }

    SentiText::~SentiText()
    {
    }

    std::vector<String> * SentiText::get_words_and_emoticons()
    {
        return &m_words_and_emoticons;
    }

	const bool * SentiText::get_is_emoticon()
	{
		return m_is_emoticon;
	}
    
    bool SentiText::isCapDiff()
    {
        return m_is_cap_diff;
    }

    String SentiText::_strip_punc_if_word(String &token, int i) // TODO: modify so it returns a pair<String, bool> so that this method can be static and then modify _words_and_emoticons as necessary
    {
        // Removes all trailing and leading punctuation 
        // If the resulting string has two or fewer characters, then it was likely an emoticon, so return original string (ie ":)" stripped would be "", so just return ":)"
        String punctuation = u8"!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
		String copy = token;
        token.erase(std::remove_if(token.begin(), 
                              token.end(),
                              [](unsigned char x){return std::ispunct(x) && x != '\'';}), // to leave contractions in, this is different than the original.
                              token.end());
		if (token.length() <= 2)
		{
			token = copy;
			this->m_is_emoticon[i] = true;
		}
		else
			this->m_is_emoticon[i] = false;
        return token;
    }

    std::vector<String> SentiText::_words_and_emoticons()
    {
        // Removes leading and trailing puncutation
        // Leaves contractions and most emoticons
        // Does not preserve punc-plus-letter emoticons (e.g. :D)
        std::vector<String> wes = split(m_text);
		m_is_emoticon = new bool[wes.size()];
        for (int i = 0; i < wes.size(); i++)
            _strip_punc_if_word(wes[i], i);
        return wes;
    }
}
