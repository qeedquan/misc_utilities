#!/usr/bin/env python

"""

Ported from https://go.dev/doc/codewalk/markov/

Generating random text: a Markov chain algorithm

Based on the program presented in the "Design and Implementation" chapter
of The Practice of Programming (Kernighan and Pike, Addison-Wesley 1999).
See also Computer Recreations, Scientific American 260, 122 - 125 (1989).

A Markov chain algorithm generates text by creating a statistical model of
potential textual suffixes for a given prefix. Consider this text:

	I am not a number! I am a free man!

Our Markov chain algorithm would arrange this text into this set of prefixes
and suffixes, or "chain": (This table assumes a prefix length of two words.)

	Prefix       Suffix

	"" ""        I
	"" I         am
	I am         a
	I am         not
	a free       man!
	am a         free
	am not       a
	a number!    I
	number! I    am
	not a        number!

To generate text using this table we select an initial prefix ("I am", for
example), choose one of the suffixes associated with that prefix at random
with probability determined by the input statistics ("a"),
and then create a new prefix by removing the first word from the prefix
and appending the suffix (making the new prefix is "am a"). Repeat this process
until we can't find any suffixes for the current prefix or we exceed the word
limit. (The word limit is necessary as the chain table may contain cycles.)

Our version of this program reads text from standard input, parsing it into a
Markov chain, and writes generated text to standard output.
The prefix and output lengths can be specified using the -prefix and -words
flags on the command-line.


"""

import argparse
import datetime
import sys
import random

def strjoin(strlist):
    return ' '.join(strlist)

def shift(prefix, word):
    prefix.append(word)
    return prefix[1:]

class Chain:
    def __init__(self, num_prefixes):
        self.num_prefixes = num_prefixes
        self.chain = {}
    
    def build(self, reader):
        prefix = [''] * self.num_prefixes
        for line in reader:
            words = line.rstrip('\n').split()
            for word in words:
                key = strjoin(prefix)
                phrase = self.chain.get(key)
                if phrase == None:
                    phrase = []
                phrase.append(word)

                self.chain[key] = phrase
                prefix = shift(prefix, word)

    def generate(self, num_words):
        words = []
        prefix = [''] * self.num_prefixes
        for i in range(0, num_words):
            key = strjoin(prefix)
            choices = self.chain.get(key)
            if choices == None:
                break

            word = random.choice(choices)
            words.append(word)
            prefix = shift(prefix, word)

        return strjoin(words)

def main():
    parser = argparse.ArgumentParser(description='Generate random text using markov chains')
    parser.add_argument('--prefixes', type=int, default=2, help='number of prefixes')
    parser.add_argument('--words', type=int, default=10000, help='number of words to generate')
    args = parser.parse_args()

    random.seed(datetime.datetime.now())
    mc = Chain(args.prefixes)
    mc.build(sys.stdin)
    words = mc.generate(args.words)
    print(words)

main()
