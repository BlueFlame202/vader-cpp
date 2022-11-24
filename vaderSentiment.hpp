// VADER SENTIMENT
// Translated into C++ by Aathreya Kadambi
// Edits made by Aathreya Kadambi
// See README in the repository for more information
// Uses C++ 17 to avoid C++ 20 utf-8 formatting issues?

/*
Original vaderSentiment.py Header:

coding: utf-8
# Author: C.J. Hutto
# Thanks to George Berry for reducing the time complexity from something like O(N^4) to O(N).
# Thanks to Ewan Klein and Pierpaolo Pantone for bringing VADER into NLTK. Those modifications were awesome.
# For license information, see LICENSE.TXT

"""
If you use the VADER sentiment analysis tools, please cite:
Hutto, C.J. & Gilbert, E.E. (2014). VADER: A Parsimonious Rule-based Model for
Sentiment Analysis of Social Media Text. Eighth International Conference on
Weblogs and Social Media (ICWSM-14). Ann Arbor, MI, June 2014.
"""
*/ 

#pragma once

#include <cmath>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

// char8_t backwards compatibility https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1423r2.html
#if defined(__cpp_lib_char8_t)
typedef std::u8string String;
typedef char8_t Char;

inline std::string from_u8string(const String &s) 
{
    return std::string(s.begin(), s.end());
}

inline std::vector<String> split(String s)
{
    std::vector<String> res;
    bool n = true;
    unsigned int i = -1; 
    for (Char c : s)
    {
        if (n)
        {
            String strversion = u8"";
            strversion.push_back(c);
            res.push_back(strversion);
            i++;
            n = false;
        }
        else
        {
            if (c == u8' ' || c == u8'\t' || c == u8'\n' || c == u8'\r' || c == u8'\f' || c == u8'\v')
                n = true;
            else
                res[i].push_back(c); // possibly better to get rid of i and just do res.size() - 1
        }
    }
    return res;
}
#else
typedef std::string String;
typedef unsigned char Char;

inline std::string from_u8string(const String &s) 
{
    return s;
}
#endif

inline std::vector<std::string> split(std::string s)
{
    std::vector<std::string> res;
    bool n = true;
    unsigned int i = -1; 
    for (char c : s)
    {
        if (n)
        {
            std::string strversion = "";
            strversion.push_back(c);
            res.push_back(strversion);
            i++;
            n = false;
        }
        else
        {
            if (c == u8' ' || c == u8'\t' || c == u8'\n' || c == u8'\r' || c == u8'\f' || c == u8'\v')
                n = true;
            else
                res[i].push_back(c); // possibly better to get rid of i and just do res.size() - 1
        }
    }
    return res;
}

namespace vader
{
    // Precomiled Constants

    // (empirically derived mean sentiment intensity rating increase for booster words)
    #define B_INCR 0.293
    #define B_DECR -0.293

    // (empirically derived mean sentiment intensity rating increase for using ALLCAPs to emphasize a word)
    #define C_INCR 0.733
    #define N_SCALAR -0.74

    // Below we use unordered sets/maps to improve speed; vaderSentiment.py uses the list structure which has average O(n) lookups when in is used
    // but here we can use std::unordered_set to get a time complexity of O(1)
    static std::unordered_set<String> NEGATE {u8"aint", u8"arent", u8"cannot", u8"cant", u8"couldnt", u8"darent", u8"didnt", u8"doesnt",
        u8"ain't", u8"aren't", u8"can't", u8"couldn't", u8"daren't", u8"didn't", u8"doesn't",
        u8"dont", u8"hadnt", u8"hasnt", u8"havent", u8"isnt", u8"mightnt", u8"mustnt", u8"neither",
        u8"don't", u8"hadn't", u8"hasn't", u8"haven't", u8"isn't", u8"mightn't", u8"mustn't",
        u8"neednt", u8"needn't", u8"never", u8"none", u8"nope", u8"nor", u8"not", u8"nothing", u8"nowhere",
        u8"oughtnt", u8"shant", u8"shouldnt", u8"uhuh", u8"wasnt", u8"werent",
        u8"oughtn't", u8"shan't", u8"shouldn't", u8"uh-uh", u8"wasn't", u8"weren't",
        u8"without", u8"wont", u8"wouldnt", u8"won't", u8"wouldn't", u8"rarely", u8"seldom", u8"despite"};

    // booster/dampener 'intensifiers' or 'degree adverbs'
    // http://en.wiktionary.org/wiki/Category:English_degree_adverbs

    static std::unordered_map<String, double> BOOSTER_DICT {{u8"absolutely", B_INCR}, {u8"amazingly", B_INCR}, {u8"awfully", B_INCR},
        {u8"completely", B_INCR}, {u8"considerable", B_INCR}, {u8"considerably", B_INCR},
        {u8"decidedly", B_INCR}, {u8"deeply", B_INCR}, {u8"effing", B_INCR}, {u8"enormous", B_INCR}, {u8"enormously", B_INCR},
        {u8"entirely", B_INCR}, {u8"especially", B_INCR}, {u8"exceptional", B_INCR}, {u8"exceptionally", B_INCR},
        {u8"extreme", B_INCR}, {u8"extremely", B_INCR},
        {u8"fabulously", B_INCR}, {u8"flipping", B_INCR}, {u8"flippin", B_INCR}, {u8"frackin", B_INCR}, {u8"fracking", B_INCR},
        {u8"fricking", B_INCR}, {u8"frickin", B_INCR}, {u8"frigging", B_INCR}, {u8"friggin", B_INCR}, {u8"fully", B_INCR},
        {u8"fuckin", B_INCR}, {u8"fucking", B_INCR}, {u8"fuggin", B_INCR}, {u8"fugging", B_INCR},
        {u8"greatly", B_INCR}, {u8"hella", B_INCR}, {u8"highly", B_INCR}, {u8"hugely", B_INCR},
        {u8"incredible", B_INCR}, {u8"incredibly", B_INCR}, {u8"intensely", B_INCR},
        {u8"major", B_INCR}, {u8"majorly", B_INCR}, {u8"more", B_INCR}, {u8"most", B_INCR}, {u8"particularly", B_INCR},
        {u8"purely", B_INCR}, {u8"quite", B_INCR}, {u8"really", B_INCR}, {u8"remarkably", B_INCR},
        {u8"so", B_INCR}, {u8"substantially", B_INCR},
        {u8"thoroughly", B_INCR}, {u8"total", B_INCR}, {u8"totally", B_INCR}, {u8"tremendous", B_INCR}, {u8"tremendously", B_INCR},
        {u8"uber", B_INCR}, {u8"unbelievably", B_INCR}, {u8"unusually", B_INCR}, {u8"utter", B_INCR}, {u8"utterly", B_INCR},
        {u8"very", B_INCR},
        {u8"almost", B_DECR}, {u8"barely", B_DECR}, {u8"hardly", B_DECR}, {u8"just enough", B_DECR},
        {u8"kind of", B_DECR}, {u8"kinda", B_DECR}, {u8"kindof", B_DECR}, {u8"kind-of", B_DECR},
        {u8"less", B_DECR}, {u8"little", B_DECR}, {u8"marginal", B_DECR}, {u8"marginally", B_DECR},
        {u8"occasional", B_DECR}, {u8"occasionally", B_DECR}, {u8"partly", B_DECR},
        {u8"scarce", B_DECR}, {u8"scarcely", B_DECR}, {u8"slight", B_DECR}, {u8"slightly", B_DECR}, {u8"somewhat", B_DECR},
        {u8"sort of", B_DECR}, {u8"sorta", B_DECR}, {u8"sortof", B_DECR}, {u8"sort-of", B_DECR}};
    /*
    // check for sentiment laden idioms that do not contain lexicon words (future work, not yet implemented)
    #define SENTIMENT_LADEN_IDIOMS {u8"cut the mustard": 2, u8"hand to mouth": -2,
                            u8"back handed": -2, u8"blow smoke": -2, u8"blowing smoke": -2,
                            u8"upper hand": 1, u8"break a leg": 2,
                            u8"cooking with gas": 2, u8"in the black": 2, u8"in the red": -2,
                            u8"on the ball": 2, u8"under the weather": -2}

    // check for special case idioms and phrases containing lexicon words
    #define SPECIAL_CASES {u8"the shit": 3, u8"the bomb": 3, u8"bad ass": 1.5, u8"badass": 1.5, u8"bus stop": 0.0,
                    u8"yeah right": -2, u8"kiss of death": -1.5, u8"to die for": 3,
                    u8"beating heart": 3.1, u8"broken heart": -2.9 }

    */

    // Static Methods

    static bool negated(std::vector<String> input_words, bool include_nt=true)
    {
        // Determine if input contains negation words
        for (String &word : input_words)
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);//[](unsigned char c){ return std::tolower(c); });
        for (String word : input_words)
            if (NEGATE.count(word))
                return true;
        if (include_nt)
            for (String word : input_words)
                if (word.find(u8"n't") != -1)
                    return true;
        /* Semi-Untranslated portion that was commented: [Possible TODO]
        '''if u8"least" in input_words:
            i = input_words.index("least")
            if i > 0 and input_words[i - 1] != u8"at":
                return True'''
        */
        return false;
    }

    static double normalize(double score, double alpha=15)
    {
        // Normalize the score to be between -1 and 1 using an alpha that approximates the max expected value
        double norm_score = score / sqrt((score * score) + alpha); // TODO: Look into fastest implementations of ISR
        if (norm_score < -1.0)
            return -1;
        else if (norm_score > 1.0)
            return 1;
        return norm_score;
    }

    static bool allcap_differential(std::vector<String> words)
    {
        /*
        Check whether just some words in the input are ALL CAPS
        :param vector words: The words to inspect
        :returns: `true` if some but not all items in `words` are ALL CAPS
        */
        int allcap_words = 0;
        for (String word : words)
            if (std::all_of(word.begin(), word.end(), ::isupper))//[](unsigned char c){ return std::isupper(c); }))
                allcap_words++;
        int cap_differential = words.size() - allcap_words; // this is actually possibly buggy, behavior on emojis/emoticons are unknown TODO: investigate
        return 0 < cap_differential && cap_differential < words.size(); // more space efficient than storing in a variable
    }

    static double scalar_inc_dec(String word, double valence, bool is_cap_diff)
    {
        // Check if the preceding words increase, decrease, or negate/nullify the valence
        double scalar = 0.0;
        String oword = word;
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        if (BOOSTER_DICT.count(word))
        {
            scalar = BOOSTER_DICT[word];
            if (valence < 0)
                scalar *= -1;
            // check if booster/dampener word is ALLCAPS (while others aren't)
            if (is_cap_diff && std::all_of(oword.begin(), oword.end(), ::isupper))
            {
                if (valence > 0)
                    scalar += C_INCR;
                else
                    scalar -= C_INCR;
            }
        }
        return scalar;
    }

    struct Sentiment
    {
        double neg = 0.0;
        double neu = 0.0;
        double pos = 0.0;
        double compound = 0.0;
    };
}