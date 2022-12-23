#include <iostream>
#include <vector>
#include <string>

#include "vaderSentiment.hpp"
#include "SentiText.hpp"
#include "SentimentIntensityAnalyzer.hpp"

int main()
{
	std::cout << std::isupper(':') << std::endl;

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