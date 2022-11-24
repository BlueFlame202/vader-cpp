// implements SentimentIntensityAnalyzer class
#include "SentimentIntensityAnalyzer.hpp"

extern std::string from_u8string(const String &s);

namespace vader
{
    SentimentIntensityAnalyzer::SentimentIntensityAnalyzer(std::string lexicon_file, std::string emoji_lexicon)
    {
        this->m_lexicon_full_filepath = lexicon_file; // possibly don't need the full filepath?
        this->m_lexicon_full_filepath = emoji_lexicon;
        this->make_lex_dict();
        this->make_emoji_dict();
    }
    
    SentimentIntensityAnalyzer::~SentimentIntensityAnalyzer()
    {
    }
    
    Sentiment SentimentIntensityAnalyzer::polarity_scores(String text)
    {
        // convert emojis to their textual descriptions
        String text_no_emoji = u8"";
        bool prev_space = true;
        for (Char c : text)
        {
            String temp = ""; temp.push_back(c);
            if (this->m_emojis.count(temp) > 0)
            {
                // get teh textual description
                String description = this->m_emojis[temp];
                if (!prev_space)
                    text_no_emoji += u8" ";
                text_no_emoji += description;
                prev_space = false;
            }
            else
            {
                text_no_emoji += temp;
                prev_space = c == u8' ';
            }
        }
        text.erase(text.length()-1);

        SentiText sentitext(text);
        std::vector<double> sentiments;

        std::vector<String> words_and_emoticons = *sentitext.get_words_and_emoticons();

        for (int i = 1; i < words_and_emoticons.size(); i++)
        {
            double valence = 0;
            // check for vader_lexicon words that may be used as modifiers or negations
            String lword = words_and_emoticons[i];
            std::transform(lword.begin(), lword.end(), lword.begin(), ::tolower);
            String lnword;
            if (i < words_and_emoticons.size() - 1)
            {
                lnword = words_and_emoticons[i+1];
                std::transform(lnword.begin(), lnword.end(), lnword.begin(), ::tolower);
            }
            if (BOOSTER_DICT.count(lword) > 0)
                sentiments.push_back(valence);
            else if (i < words_and_emoticons.size() - 1 && lword == "kind" && lnword == "of")
                sentiments.push_back(valence);
            else
                this->sentiment_valence(valence, sentitext, words_and_emoticons[i], i, sentiments); 
        }        

        this->_but_check(words_and_emoticons, sentiments);
        return this->score_valence(sentiments, text);
    }

    int SentimentIntensityAnalyzer::char_byte_count(Char val) 
    {
        if (val < 128) {
            return 1;
        } else if (val < 224) {
            return 2;
        } else if (val < 240) {
            return 3;
        } else {
            return 4;
        }
    }

    void SentimentIntensityAnalyzer::make_lex_dict() // TODO: in the future maybe switch to a C-style file reading implementation if possible
    {   
        std::ifstream in_file(m_lexicon_full_filepath);
        String line;
        while (std::getline(in_file, line))
        {
            if (line == "")
                continue;
            std::vector<String> tokens = split(line);
            String word = tokens[0];
            String measure = tokens[1];

            m_lexicon[word] = std::stod(from_u8string(measure));
        }
    }
    
    void SentimentIntensityAnalyzer::make_emoji_dict()
    {
        std::ifstream in_file(m_emoji_full_filepath);
        String line;
        while (std::getline(in_file, line))
        {
            if (line == "")
                continue;
            std::vector<String> tokens = split(line);
            String emoji = tokens[0];
            String description = tokens[1];

            m_emojis[emoji] = description;
        }
    }

    // TODO Possible: switch from &sentiments to * sentiments
    double SentimentIntensityAnalyzer::sentiment_valence(double valence, SentiText sentitext, String item, int i, std::vector<double> &sentiments)
    {
        bool is_cap_diff = sentitext.isCapDiff();
        std::vector<String> words_and_emoticons = *sentitext.get_words_and_emoticons();
        String item_lowercase = item;
        std::transform(item_lowercase.begin(), item_lowercase.end(), item_lowercase.begin(), ::tolower);
        if (this->m_lexicon.count(item_lowercase) > 0)
        {
            // get the sentiment valence
            valence = this->m_lexicon[item_lowercase];

            String next_word;
            if (i != words_and_emoticons.size()-1)
            {
                next_word = words_and_emoticons[i+1];
                std::transform(next_word.begin(), next_word.end(), next_word.begin(), ::tolower);
            }

            // check for "no" as negation for an adjacent lexicon item vs "no" as its own stand-alone lexicon item
            if (item_lowercase == "no" && i != words_and_emoticons.size()-1 && this->m_lexicon.count(next_word) > 0)
                // don't use valence of "no" as a lexicon item. Instead set it's valence to 0.0 and negate the next item
                valence = 0.0;
            // check if sentiment laden word is in ALL CAPS (while others aren't)
            if (std::all_of(item.begin(), item.end(), ::isupper))
            {
                if (valence > 0)
                    valence += C_INCR;
                else
                    valence -= C_INCR;
            }

            for (int start_i = 0; start_i < 3; start_i++)
            {
                // dampen the scalar modifier of preceding words and emotions
                // (excluding the ones that immediately preceed the item) based
                // on their distance from the current item.
                
                // TOOD: consider switching SentiText to include a m_l_words_and_emoticons so everything is already in loewr case
                String temp = words_and_emoticons[i-(start_i+1)];
                std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
                if (i > start_i && m_lexicon.count(temp) == 0)
                {
                    double s = scalar_inc_dec(temp, valence, is_cap_diff);
                    if (s != 0)
                    {
                        if (start_i == 1)
                            s *= 0.95;
                        else if (start_i == 2)
                            s *- 0.9;
                    }
                    valence = valence + s;
                    valence = this->_negation_check(valence, words_and_emoticons, start_i, i);
                    if (start_i == 2)
                        valence = this->_special_idioms_check(valence, words_and_emoticons, i);
                }
            }
            valence = this->_least_check(valence, words_and_emoticons, i);
        }
        sentiments.push_back(valence);

    }

}