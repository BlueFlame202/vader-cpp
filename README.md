# VADER Sentiment Analysis in C++

VADER (Valence Aware Dictionary and sEntiment Reasoner) is a lexicon and rule-based tool that is designed to measure sentiments expressed in social media. The original tool was built in Python; this is a port of the original to C++. It is designed to be faster for large data sets. The current version of the program is v1.0.0. Benchmarks and further testing and improvements will continue to improve the speed and accuracy of the code. As the original project was open-sourced under the MIT License, so is this project. Attributions are appreciated, but no authors may be held liable.

## Table of Contents
  * [Table of Contents](#table-of-contents)
  * [Discrepencies Between the Python and Cpp Versions](#discrepencies-between-the-python-and-cpp-versions)
    + [isupper Methods](#isupper-methods)
    + [Spacing](#spacing)
  * [Issues and Future Work](#issues-and-future-work)
    + [Handling of UTF-8 Characters in Cpp](#handling-of-utf-8-characters-in-cpp)
    + [Future Changes](#future-changes)
    + [Time Complexities](#time-complexities)
  * [Cpp Demo and Code Examples](#cpp-demo-and-code-examples)
  * [Other Information](#other-information)
  * [Contact](#contact)

## Discrepencies Between the Python and Cpp Versions

While some differences may still be discovered with further use, there are two primary ways in which the C++ version of VADER differs from the Python version. 

### isupper Methods

Firstly, Python's String isupper() method functions different from C++'s std::isupper(). Specifically, in Python, running

```>>> print(':D'.isupper())```

yields True despite the character ':' not being uppercase. Meanwhile, in C++, as no similar isupper method exists for std::strings, we are forced to use the expression  

```std::all_of(word.begin(), word.end(), [](unsigned char c){ return ::isupper(c); })```

in order to check if the string ```word``` is capitalized. However, this means that ':D'.isupper() yields ```True``` while running the C++ code above on ```word = u8":D"``` yields ```0``` or false. Specifically, this means that when the C++ version of ```polarity_scores``` is run on a sentence such as "Make sure you :) or :D today!", it does not emphasize the ":D" emoji as much since it is seen as not capitalized, whereas in the Python version, ":D" is seen as capitalized and its lexicon score is emphasized by adding an additional amount to the valence. 

This difference was kept because the original sentiment of ":D" is likely more accurate than the inflated version, which is the original valance plus ```C_INCR```.

### Spacing 

Secondly, a difference exists in the ```polarity_scores``` methods. Here, a space is also inserted immediately after an emoji to ensure that emojis are processed alone without `leaking` into other words. For example, this is so that the string "😀hi" does not become "grinning facehi" but rather "grinning face hi". To ensure that two spaces are not inserted, the else condition becomes ```else if (!(prev_space && temp == u8" "))```.

## Issues and Future Work

### Handling of UTF-8 Characters in Cpp

Working with UTF-8 and emojis is quite difficult in C++. Aside from the confusions arising from differences between C++ 17 and C++ 20, emojis also cannot be read in as individual characters easily. As a result, the cppemojihandler.hpp file contains two methods for identifying and matching emojis from the emoji_utf8_lexicon.txt file.

```create_emoji_bank``` creates a ```std::vector``` such that the ith element in the vector is an ```std::unordered_set``` containing all possible ith characters of an emoji when interpreted in plain text. This way, emojis can be identified by parsing characters that could be part of an emoji using ```emoji_chars_from_start```, and finally verifying if the emoji is in the emoji_utf8_lexicon.txt file. This may not be the optimal method of implementing this, either in time or space. The method was designed in this way to avoid use of external libraries. Future optimization can improve this area of the code.

### Future Changes

* C-style file reading can be faster, so this can be used rather than ```std::ifstream``` in ```vader::SentimentIntensityAnalyzer::make_lex_dict``` and ```vader::SentimentIntensityAnalyzer::make_emoji_dict```.
* The ```vader::SentimentIntensityAnalyzer::_sentiment_laden_idioms_check``` has not been implemented yet as it was labeled as future work in the original project.
* Several methods may have more optimal ways to be coded, such as the ```vader::normalize``` method.
* Similar discrepancies in terms of isupper should be investigated.
* Only the first instance of `but` is considered in ```vader::SentimentIntensityAnalyzer::_but_check```. In the future, the method should be modified to analyze texts which contain multiple `but`s in a single sentence or piece of text. Note that this method of analysis also poses problems if the tool is used on paragraphs, which may easily contain multiple instances of the word `but`.
* Pieces of code containing ```break``` can be reimplemented without ```break```, although this is less necessary.

### Time Complexities

Time complexities will be analyzed and posted here soon!

## Cpp Demo and Code Examples

A demo of the C++ VADER tool is as follows (taken from the test.cpp file):

``` #include <iostream>
#include <vector>
#include <string>

#include "vaderSentiment.hpp"
#include "SentiText.hpp"
#include "SentimentIntensityAnalyzer.hpp"

int main()
{
	String sentences[16] = 
	{
		u8"VADER is smart, handsome, and funny.", // positive sentence example
		u8"VADER is smart, handsome, and funny!", // punctuation emphasis handled correctly (sentiment intensity adjusted)
		u8"VADER is very smart, handsome, and funny.", // booster words handled correctly (sentiment intensity adjusted)
		u8"VADER is VERY SMART, handsome, and FUNNY.", // emphasis for ALLCAPS handled
		u8"VADER is VERY SMART, handsome, and FUNNY!!!", // combination of signals - BADER appropriately adjusts intensity
		u8"VADER is VERY SMART, uber handsome, and FRIGGIN FUNNY!!!", // booster words & punctuation make this close to ceiling for score
		u8"VADER is not smart, handsome, nor funny.", // negation sentence example
		u8"The book was good.", // positive sentence
		u8"At least it isn't a horrible book.", // negated negative sentence with contradiction
		u8"The book was only kind of good.", // qualified positive sentence is handled correctly (intensity adjusted)
		u8"The plot was good, but the characters are uncompelling and the dialog is not great.", // mixed negation sentence
		u8"Today SUX!", // negative slang with capitalization emphasis
		u8"Today only kinda sux! But I'll get by, lol", // mixed sentiment example with slang and contrastive conjunction "but"
		u8"Catch utf-8 emoji such as 💘 and 💋 and 😁", // emoticons handled
		u8"Make sure you :) or :D today!", // emoticons handled
		u8"Not bad at all" // Capitalized negation
	};

	vader::SentimentIntensityAnalyzer vader;
	std::cout << "---------------------------------------------------- " << '\n' <<
		" - Analyze typical example cases, including handling of: " << '\n' <<
		"  -- negations " << '\n' <<
		"  -- punctuation emphasis & punctuation flooding " << '\n' <<
		"  -- word-shape as emphasis (capitalization difference) " << '\n' <<
		"  -- degree modifiers (intensifiers such as 'very' and dampeners such as 'kind of') " << '\n' <<
		"  -- slang words as modifiers such as 'uber' or 'friggin' or 'kinda' " << '\n' <<
		"  -- contrastive conjunction 'but' indicating a shift in sentiment; sentiment of later text is dominant " << '\n' <<
		"  -- use of contractions as negations " << '\n' <<
		"  -- sentiment laden emoticons such as :) and :D " << '\n' <<
		"  -- utf-8 encoded emojis such as 💘 and 💋 and 😁 " << '\n' <<
		"  -- sentiment laden slang words (e.g., 'sux') " << '\n' <<
		"  -- sentiment laden initialisms and acronyms (for example: 'lol')" << std::endl;

	for (String sentence : sentences)
	{
		vader::Sentiment vs = vader.polarity_scores(sentence);
		std::cout << vs.compound << ", " << vs.neg << ", " << vs.neu << ", " << vs.pos << std::endl;
	}

	std::cout << "----------------------------------------------------" << std::endl;
	std::cout << " - About the scoring: " << std::endl;
	std::cout << "  -- The 'compound' score is computed by summing the valence scores of each word in the lexicon, adjusted according to the rules, and then normalized to be between - 1 (most extreme negative) and +1 (most extreme positive). This is the most useful metric if you want a single unidimensional measure of sentiment for a given sentence. Calling it a 'normalized, weighted composite score' is accurate." << std::endl;
	std::cout << "  -- The 'pos\", \"neu', and 'neg' scores are ratios for proportions of text that fall in each category (so these should all add up to be 1... or close to it with float operation).These are the most useful metrics if you want multidimensional measures of sentiment for a given sentence." << std::endl;
	std::cout << "----------------------------------------------------" << std::endl;

	String tricky_sentences[13] = 
	{
		"Sentiment analysis has never been good.",
		"Sentiment analysis has never been this good!",
		"Most automated sentiment analysis tools are shit.",
		"With VADER, sentiment analysis is the shit!",
		"Other sentiment analysis tools can be quite bad.",
		"On the other hand, VADER is quite bad ass",
		"VADER is such a badass!",  // slang with punctuation emphasis
		"Without a doubt, excellent idea.",
		"Roger Dodger is one of the most compelling variations on this theme.",
		"Roger Dodger is at least compelling as a variation on the theme.",
		"Roger Dodger is one of the least compelling variations on this theme.",
		"Not such a badass after all.",  // Capitalized negation with slang
		"Without a doubt, an excellent idea."  // "without {any} doubt" as negation
	};

	std::cout << "----------------------------------------------------" << std::endl;
	std::cout << " - Analyze examples of tricky sentences that cause trouble to other sentiment analysis tools." << std::endl;
	std::cout << "  -- special case idioms - e.g., 'never good' vs 'never this good', or 'bad' vs 'bad ass'." << std::endl;
	std::cout << "  -- special uses of 'least' as negation versus comparison \n" << std::endl;
	for (String sentence : tricky_sentences)
	{
		vader::Sentiment vs = vader.polarity_scores(sentence);
		std::cout << vs.compound << ", " << vs.neg << ", " << vs.neu << ", " << vs.pos << std::endl;
	}
	std::cout << "----------------------------------------------------" << std::endl;

	std::cin.get();

	return 0;
}
```

The output for the above example is:

```
----------------------------------------------------
 - Analyze typical example cases, including handling of:
  -- negations
  -- punctuation emphasis & punctuation flooding
  -- word-shape as emphasis (capitalization difference)
  -- degree modifiers (intensifiers such as 'very' and dampeners such as 'kind of')
  -- slang words as modifiers such as 'uber' or 'friggin' or 'kinda'
  -- contrastive conjunction 'but' indicating a shift in sentiment; sentiment of later text is dominant
  -- use of contractions as negations
  -- sentiment laden emoticons such as :) and :D
  -- utf-8 encoded emojis such as ≡ƒÆÿ and ≡ƒÆï and ≡ƒÿü
  -- sentiment laden slang words (e.g., 'sux')
  -- sentiment laden initialisms and acronyms (for example: 'lol')
0.831632, 0, 0.254237, 0.745763
0.843896, 0, 0.248098, 0.751902
0.85451, 0, 0.299147, 0.700853
0.922657, 0, 0.245901, 0.754099
0.934209, 0, 0.233335, 0.766665
0.946938, 0, 0.293968, 0.706032
-0.742418, 0.645767, 0.354233, 0
0.440434, 0, 0.508475, 0.491525
0.43102, 0, 0.677966, 0.322034
0.383245, 0, 0.697107, 0.302893
-0.704169, 0.327419, 0.578564, 0.0940167
-0.546137, 0.779006, 0.220994, 0
0.524914, 0.127307, 0.555754, 0.316939
0.875, 0, 0.583333, 0.416667
0.835634, 0, 0.31027, 0.68973
0.43102, 0, 0.512821, 0.487179
----------------------------------------------------
 - About the scoring:
  -- The 'compound' score is computed by summing the valence scores of each word in the lexicon, adjusted according to the rules, and then normalized to be between - 1 (most extreme negative) and +1 (most extreme positive). This is the most useful metric if you want a single unidimensional measure of sentiment for a given sentence. Calling it a 'normalized, weighted composite score' is accurate.
  -- The 'pos", "neu', and 'neg' scores are ratios for proportions of text that fall in each category (so these should all add up to be 1... or close to it with float operation).These are the most useful metrics if you want multidimensional measures of sentiment for a given sentence.
----------------------------------------------------
----------------------------------------------------
 - Analyze examples of tricky sentences that cause trouble to other sentiment analysis tools.
  -- special case idioms - e.g., 'never good' vs 'never this good', or 'bad' vs 'bad ass'.
  -- special uses of 'least' as negation versus comparison

-0.341238, 0.324872, 0.675128, 0
0.567153, 0, 0.620668, 0.379332
-0.55737, 0.375, 0.625, 0
0.647644, 0, 0.582977, 0.417023
-0.584919, 0.351431, 0.648569, 0
0.801996, 0, 0.422535, 0.577465
0.400336, 0, 0.597729, 0.402271
0.701287, 0, 0.340522, 0.659478
0.294382, 0, 0.833775, 0.166225
0.226348, 0, 0.852713, 0.147287
-0.169473, 0.131533, 0.868467, 0
-0.258409, 0.289369, 0.710631, 0
0.701287, 0, 0.407747, 0.592253
----------------------------------------------------
```

The values here mostly match those of the original VADER tool in Python. Any differences result from those mentioned in the [Discrepencies section above](#Discrepencies-Between-the-Python-and-Cpp-Versions).

## Other Information

For more information on the VADER Sentiment tool or to find the original papers and work, please see [the original Python version](https://github.com/cjhutto/vaderSentiment).

## Contact

To contact me, please email aathreyakadambi@gmail.com.
