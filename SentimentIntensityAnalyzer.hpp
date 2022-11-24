// vader::SentimentIntensityAnalyzer class header

#pragma once

#include "SentiText.hpp"

namespace vader
{
    class SentimentIntensityAnalyzer // Give a sentiment intensity score to sentences.
    {
    private: 
        String m_lexicon_full_filepath;
        String m_emoji_full_filepath;

        std::unordered_map<String, double> m_lexicon;
        std::unordered_map<String, String> m_emojis;

    public:
        SentimentIntensityAnalyzer(std::string lexicon_file="vader_lexicon.txt", std::string emoji_lexicon="emoji_utf8_lexicon.txt");
        ~SentimentIntensityAnalyzer();

        Sentiment polarity_scores(String text);
        double sentiment_valence(double valence, SentiText sentitext, String item, int i, std::vector<double> &sentiments);

    private:
        static int char_byte_count(Char val);

        void make_lex_dict();
        void make_emoji_dict();
        
        double _least_check(double valance, const std::vector<String> &words_and_emoticons, int i);
        static void _but_check(const std::vector<String> &words_and_emoticons, std::vector<double> &sentiments);
        static double _special_idioms_check(double valence, const std::vector<String> &words_and_emoticons, int i);
        static double _sentiment_laden_idioms_check(double valence, SentiText senti_text_lower); // future work
        static double _negation_check(double valence, const std::vector<String> &words_and_emoticons, int start_i, int i);
        
        double _punctuation_emphasis(String text);
        static double _amplify_ep(String text);
        static double _amplify_qm(String text);
        
        static double * _sift_sentiment_scores(std::vector<double> &sentiments);
        Sentiment score_valence(std::vector<double> &sentiments, String text);
    };
}