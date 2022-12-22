#include <iostream>
#include <vector>
#include <string>

#include "vaderSentiment.hpp"
#include "SentiText.hpp"
#include "SentimentIntensityAnalyzer.hpp"

int main()
{
	String s = u8"wow! I'm so happy! :D this is amazing;";
    vader::SentiText senti(s);
	/*
	std::vector<String> words_and_emoticons = *s.get_words_and_emoticons();
    std::cout << "[" << std::endl;
    for (String word : words_and_emoticons)
        std::cout << from_u8string(word) << std::endl;
    std::cout << "]" << std::endl;
	*/

	vader::SentimentIntensityAnalyzer vader;
	vader::Sentiment res = vader.polarity_scores(s);
	std::cout << res.compound << ", " << res.neg << ", " << res.neu << ", " << res.pos << std::endl;

    return 0;
}
