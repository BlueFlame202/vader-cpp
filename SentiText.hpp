// vader::SentiText class header

#pragma once
#pragma execution_character_set("utf-8")

#include "vaderSentiment.hpp"

namespace vader
{
    class SentiText // Identify sentiment-relevant string-level properties of input text.
    {
    private:
        String m_text;
        std::vector<String> m_words_and_emoticons;
        bool m_is_cap_diff;

    public:
        SentiText(String text);
        ~SentiText();

        std::vector<String> * get_words_and_emoticons();
        bool isCapDiff();

    private:
        static String _strip_punc_if_word(String &token);
        std::vector<String> _words_and_emoticons();
    };
}