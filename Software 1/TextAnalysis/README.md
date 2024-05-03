# Introduction
An assignment given as part of the Software 1 (fall 2023) course - implmenting classes using collections, for text files analysis.

## Histogram
We first implement a data structure representing a Histogram using generic collections. We define a histogram as a data structure that counts the amount of instances of some type *T*. For example, for the sequence 1, 2, 3, 1, 2 the histogram will contain the members 1, 2, 3 and their respective number of occurences.

We are given the interface *IHistogram* with the following methods:
- **addItem()**: adds one instance of *item* to the histogram.
- **removeItem()**: removes one instance of *item* - returns true if it was removed, and false if it didn't exist (therefore wasn't removed).
- **addAll()**: adds a number of items to the histogram.
- **getCountForItem()**: returns the number of occurences for a given *item*. If it doesn't exist we return 0.
- **clear()**: empties the histogram out of all the members and their count.
- **getItemsSet()**: returns a collection of type *Set* which contains all the members of the histogram with a count that is greater than 0.
- **getCountsSum()**: returns the sum of the counts for all members of the histogram.

Implementation:

1. We implement the class *HashMapHistogram*:
```public class HashMapHistogram<T extends Comparable<T>> implements IHistogram<T>```\
We use aggregation of the type *HashMap*, every instance of *HashMapHistogram* will contain a field of that type, reponsible for counting every object of type *T*.


2. We implement the class *HashMapHistogramIterator*:
The interface we deal with is defined as follows:
```public interface IHistogram<T> extends Iterable<Map.Entry<T, Integer>>```\
Therefore the iterator that the histogram should return iterates over pairs of type *Map.Entry*, which represent <key,value> in the histogram.
The iterator returns the pairs based on the natural order of the keys (of type *T*).
For example, if we deal with strings, it will be in lexicographical order.


## TFIDF
Now, we work with Collections to implement a class that analyses text files, and returns significant words, sorted by their similarity, in the following manner:\
For some folder, we will read the content of each file, and store the relevant metadata for each file (indexing) - this will happen only once. Based on the index, we move on to more perform more actions.

We first implement *indexDirectory()* in the class *FileIndex*. The goal is to read the content of all the files, analyse it, and store it so that the implementation of the rest of the methods in *FileIndex* will be efficient. We use *HashMapHistogram* to count the number of occurences for each token in the file.

For a collection of documents (each document is a file) we define:
- **tf (term frequency)**: calculated for a word and a doc as follows:\
```tf(word, doc)``` = (#repetitions(word) in doc) / (#words in doc)
- **idf (inverse document frequency)**: calculated for a word as follows:\
```idf(word)``` = log((#documents) / (#documents containing word))

**tf-idf** is a measurement that gives a score for each *word* in *doc*, and is calculated by multiplying **tf** with **idf**.\
The more a word appears in a certain *doc*, the more significant it is in *doc* (tf). On the other hand (idf), if the word appears in all the docs, it means it is less "unique" with repsect to *doc*. If the word appears in all the docs, its *idf* is $log(1)=0$.\
So **tf-idf** grows as the word appears in **less** docuemnts.
Further info can be found [here](https://medium.com/analytics-vidhya/tf-idf-term-frequency-technique-easiest-explanation-for-text-classification-in-nlp-with-code-8ca3912e58c3).

In the method:\
```public double getCosineSimilarity(String fileName1, String fileName2) throws FileIndexException```\
we use the formula we presented in BigramModel, no need to build vectors with the size of the whole vocabulary.