# Introduction
An assignment given as part of the Software 1 (fall 2023) course - implementing a bigram langauge model.

We build a basic model of a language (the English language) from a given text, using statistics of the appearance of word pairs in the text. For example, if the text is:
"here comes the sun", we have 4 words and the pairs <here, comes>, <comes, the>, <the, sun> each appear one time. The pair <here, sun> does not appear in our text, so we saw it 0 times in total. Our language model will contain the vocabulary and the occurrence count of each word pair.

## Building the language model
The class *BigramModel* represents the language model. The class contains two fields:
- ```String[] mVocabulary```: An array that contains all the words (vocabulary).
- ```int[][] mBigramCounts```: A two-dimensional array the contain the number of occurrences of each word pair in the text. The member in index (i,j) will represent the number of times word *j* appears after word *i*.

We implement the class in the following steps:
1. ```public String[] buildVocabularyIndex(String fileName) throws IOException```:\
The function gets a file name and returns an array of strings that will serve as our vocabulary. The array *vocabulary*:
    - Will contain up to 14,500 legal words from the file.
    - A legal word is a word that meets one of the two definitions:
     (i) A word that contains at least one letter in the English language.
     (ii) An integer (for example, 13 or 1984). In terms of edge cases, an integer is defined in our context as a sequence of digits without any other character. It is allowed for the number to start with 0, but a plus or minus sign at the beginning or a decimal point is not allowed. If there are any characters other than digits in the word, if one of them is an English letter it is a word of the type (i). 
     If there are additional characters such that none of them is an English letter, then it is not a legal word.
    - Every legal word that contains at least one letter in English must be converted to lowercase.
    - Each integer will be converted to the string "some_num". Thus, 55 and 67 are actually the same word: "some_num", and are mapped to a single value in the dictionary. There should not be a repetition of some_num in the dictionary - all the numbers are basically the same word and should be treated as we treat words that appear several times. All illegal words should be ignored.
    - If we find some sign next to a certain word (which has at least one letter in English) such as ",", so for simplicity, we'll consider it a separate word from the word without the punctuation. That is, the word "man" and "man's" are two different words. Same for "java" and "java.".
    
    We will insert the legal words into the vocabulary array, according to the order of their appearance in the file (the first word will be inserted to index 0, and so on). If a word appears twice, it will be saved only once, under the index where it was saved the first time it was seen (for example, if the word "the" is the third word in the file and entered the vocabulary under index 2, the next time we see it, the vocabulary will not change).
    If the file contains more than 14.5K different legal words, we will save only 14.5K words in the vocabulary. If the file contains less than 14.5K different words, the size of the vocabulary array will be adjusted to the number of different words in the file.

2. ```public int[][] buildCountsArray(String fileName, String[] vocabulary) throws IOException```:\
The function gets a file name and an array of strings. It will return a two-dimensional array of integers which will contain the number of occurrences of word pairs in the file according to the following:
    - If vocabulary[x] = word1 and vocabulary[y]=word2, then bigramCounts[x][y] will contain the number of times word2 appeared **immediately after** word1 in the same sentence.
    - Assume that in your input files each sentence will start on a new line. Also, any new line that isn't empty starts a new sentence.
    -  Statistics will only be collected for words found in the vocabulary. If the file contains more than 14.5K words, pairs that contain a word that is not in the vocabulary will not be counted. New words beyond this point are no longer considered valid (but we still count pairs of words until the end of the file).
    - If the word *y* has never appeared after the word *x*, then bigramCounts[x][y] will contain the value 0.


3. ```public void saveModel(String fileName)```:\
The function gets a file name and saves the learned model (vocabulary and pair count)
into two files. The vocabulary will be saved to the file filename.voc whereas
the counts will be saved in the filename.counts file.
In the .voc file the vocabulary will be saved in the following format:
    - The first line will contain the number of words found in mVocabulary.
    - Starting from the second line, each word will appear, along with its index, in   ascending order of the indices.
If there are numbers in the text, then some_num will appear in the index of the first number that appears in the text and will not appear in the dictionary again.

    In the .counts file, the counts of the word pairs will be saved as follows:
    - In each line we will write the number of occurrences of each pair of words, when the pair of words will be represented by their indices in the vocabulary. We will write the number of occurrences in the same row and separate it from the two indices with semicolons.
    - The pair <i1,i2> will be written before the pair <i3,i4> if i1 is smaller than i3, or if i1 and i3 are the same, and i2 is smaller than i4.
    - Since most of the possible pairs of words do not appear in the text (the number of occurrences is 0), we will save only the pairs that did appear (that is, values other than 0).
    
4. ```public void loadModel(String fileName) throws IOException```:\
The function gets the name of a file and loads from the two files fileName.voc and fileName.counts the language model into the fields mVocabulary and mBigramCounts. If these fields are initialized to other values, these values are overridden.

5. ```public int getWordIndex(String word)```:\
The function gets a string and returns its index in mVocabulary. If this string does not appear, the function will return -1.

6. ```public int getBigramCount(String word1, String word2)```:\
The function gets two strings *word1* and *word2* and returns the number of times that *word2* appeared after *word1*. If one of the words does not exist in the vocabulary
the function will return 0.

7. ```public String getMostFrequentProceeding(String word)```:\
The function gets a word and returns the word that appeared after it the most number of times. If there are several words with the same number of occurrences, we will return the word with the lowest index in the vocabulary. If after the word no other word was seen the method return null.

8. ```public boolean isLegalSentence(String sentence)```:\
The function gets a sentence and checks if the language model we built allows the existence of this sentence. Because the language model claims to learn the language's structure, and the connections between words, the sentence will be legal if every pair of adjacent words that appear in the sentence appeared together at least once in the text on which we learned the language model.

## Word Similarity
We would like to see if using our data structure we can learn something about language behavior and the relationship between words. For this goal, we will use a large text file, and try to see if there is a relationship between words that appear in a similar context: that is, in close proximity to the same words.

```public static double calcCosineSim(int[] arr1, int[] arr2)```:\
The function gets two arrays of integers of the same length, and calculates the similarity of the cosines between them according to the following formula:

$$\frac {\sum\limits_{i=1}^n A_i B_i} {\sqrt{\sum\limits_{i=1}^n A_i^2 } \sqrt{\sum\limits_{i=1}^n B_i^2 }}$$

Where *A* and *B* are vectors of dimension *n* who are represented by an int array.
The similiar the vectors are, the greater the value we will obtain.

We will now use this function to find the word which is the most similar to some other word, based on their context. 
**vector v represents word w**: 
- for a word *w* in our vocabulary, the vector *v* that represents it is obtained from the language model and its length is the same as the number of words in the vocabulary (*mVocabulary*).
- the member in index *x* will contain the number of time the pair (*word*, *mVocabulary*) was seen in the language model.
- vector *v* will be represented by a one-dimensional int array.

```public String getClosestWord(String word)```:\
The function gets a word and returns the word which is the most similiar to it, according to the comparison of the representative vectors of all the words to that of word. You must extract the vector representing word from the bigramCounts field, and then, find a word *w* so that the cosineSim between the representative vector of *w* and that of word is the highest among the rest the words in the vocabulary. 
Our expectation is that words with more "similar" vectors will be similar
in the meaning or the way they are used in the text. If there are two words that have the same similarity, we return the whose index is smaller.

## Notes
In order to get significant results with the method describe above we must use a very large amount of text, with a wide vocabulary and many usage examples for each word. Also, should we use additional information: not only compare the words that appear after the word, but also the words that precede it, and use more than one next/previous word. At the same time, even with the very basic method described in section 10 it is possible to reach interesting results for the file *emma.txt* (the book "Emma" by Jane Austen, from the website https://www.gutenberg.org). 
For example, it appears that "good" and "great" have relatively similar vectors, as well as "he", "she" and many others.

What can help us improve the results is a linguistic processing on the text: for example, convert all the inflections of verbs and nouns into the singular form (goes -> go, books -> book).
Our implementation is of course basic, but forms the basis for more advanced techniques that are used to learn information about a language. In particular, in *isLegalSentence* we use a very strict measure: words that don't appear in the training text make the sentence "illegal". This measurement is determined only for the purpose of the exercise. In reality, we give the sentences scores: how much
they are reasonable in light of the model studied.
