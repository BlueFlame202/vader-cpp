#include <iostream>
#include <vector>
#include <string>

#include "vaderSentiment.hpp"
#include "SentiText.hpp"

int main()
{
    vader::SentiText s(u8"wow! I'm so happy! :D this is amazing;");
    std::vector<String> words_and_emoticons = *s.get_words_and_emoticons();
    std::cout << "[" << std::endl;
    for (String word : words_and_emoticons)
        std::cout << from_u8string(word) << std::endl;
    std::cout << "]" << std::endl;
    
    return 0;
}