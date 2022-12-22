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
    
    bool SentiText::isCapDiff()
    {
        return m_is_cap_diff;
    }

    String SentiText::_strip_punc_if_word(String &token)
    {
        // Removes all trailing and leading punctuation 
        // If the resulting string has two or fewer characters, then it was likely an emoticon, so return original string (ie ":)" stripped would be "", so just return ":)"
        String punctuation = u8"!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
        String copy = token;
        token.erase(std::remove_if(token.begin(), 
                              token.end(),
                              [](unsigned char x){return std::ispunct(x);}),
                              token.end());
        if (token.length() <= 2)
            return copy;
        return token;
    }

    std::vector<String> SentiText::_words_and_emoticons()
    {
        // Removes leading and trailing puncutation
        // Leaves contractions and most emoticons
        // Does not preserve punc-plus-letter emoticons (e.g. :D)
        std::vector<String> wes = split(m_text);
        for (String &s : wes)
            _strip_punc_if_word(s);
        return wes;
    }
}
