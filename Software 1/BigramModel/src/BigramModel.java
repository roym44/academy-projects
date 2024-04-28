import java.io.*;
import java.io.IOException;
import java.util.Arrays;

public class BigramModel {
	public static final int MAX_VOCABULARY_SIZE = 14500;
	public static final String VOC_FILE_SUFFIX = ".voc";
	public static final String COUNTS_FILE_SUFFIX = ".counts";
	public static final String SOME_NUM = "some_num";
	public static final int ELEMENT_NOT_FOUND = -1;
	
	String[] mVocabulary;
	int[][] mBigramCounts;
	
	/*
	 * @pre: arr1.length = arr2.legnth
	 * post if arr1 or arr2 are only filled with zeros, $ret = -1, otherwise calcluates CosineSim
	 */
	public static double calcCosineSim(int[] arr1, int[] arr2){ //  Q - 9
		double sum = 0;
		double sumArr1 = 0;
		double sumArr2 = 0;
		for (int i = 0; i < arr1.length; i++) {
			sum += (arr1[i] * arr2[i]);
			sumArr1 += Math.pow(arr1[i], 2);
			sumArr2 += Math.pow(arr2[i], 2);
		}
		if (sumArr1 == 0 || sumArr2 == 0)
			return -1;
		return (sum / (Math.sqrt(sumArr1) * Math.sqrt(sumArr2)));
	}
	
	public void initModel(String fileName) throws IOException{
		mVocabulary = buildVocabularyIndex(fileName);
		mBigramCounts = buildCountsArray(fileName, mVocabulary);
	}

	/*
	 * @post: mVocabulary = prev(mVocabulary)
	 * @post: mBigramCounts = prev(mBigramCounts)
	 */
	public String[] buildVocabularyIndex(String fileName) throws IOException{ // Q 1
		String[] vocabulary = new String[MAX_VOCABULARY_SIZE];
		int vocIndex = 0;
		BufferedReader reader = new BufferedReader(new FileReader(fileName));
		String line = reader.readLine();
		while (line != null) {
			String[] currentLine = line.split(" ");
			for (String word : currentLine) {
				// Check that the word is valid
				if (word.isEmpty())
					continue;
				boolean isEnglish = hasEnglishLetter(word);
				boolean isInteger = isInteger(word);
				if (!isEnglish && !isInteger)
					continue;

				// Prepare word for the vocabulary
				word = isInteger ? SOME_NUM : word.toLowerCase();
				// Don't save the same word more than once
				if (indexOf(vocabulary, word, vocIndex) != ELEMENT_NOT_FOUND)
					continue;

				// Save the word and increment the index by 1
				vocabulary[vocIndex++] = word;
				// Reached max size - stop looking for more words
				if (vocIndex == MAX_VOCABULARY_SIZE)
					break;
			}
			// Also here there is a chance that we need to break
			if (vocIndex == MAX_VOCABULARY_SIZE)
				break;
			line = reader.readLine();
		}
		reader.close();
		return Arrays.copyOfRange(vocabulary, 0, vocIndex);
	}

	// Helper function to return the index of the word in the vocabulary (-1 if not found)
	private int indexOf(String[] vocabulary, String word, int length)
	{
		for (int i = 0; i < length; i++) {
			if (vocabulary[i].equals(word))
				return i;
		}
		return ELEMENT_NOT_FOUND;
	}

	// Helper function to check if the word is an Integer
	private boolean isInteger(String word)
	{
		for (int i = 0 ; i < word.length(); i++) {
			// Found a character which is not a digit - not an Integer
			if (!Character.isDigit(word.charAt(i)))
				return false;
		}
		return true;
	}

	// Helper function to check if the word contains at least one letter in English
	private boolean hasEnglishLetter(String word)
	{
		for (int i = 0 ; i < word.length(); i++) {
			if (Character.isLetter(word.charAt(i)))
				return true;
		}
		return false;
	}
	
	/*
	 * @post: mVocabulary = prev(mVocabulary)
	 * @post: mBigramCounts = prev(mBigramCounts)
	 */
	public int[][] buildCountsArray(String fileName, String[] vocabulary) throws IOException{ // Q - 2
		int[][] countsArray = new int[vocabulary.length][vocabulary.length];
		BufferedReader reader = new BufferedReader(new FileReader(fileName));
		String line = reader.readLine();
		while (line != null) {
			String[] currentLine = line.split(" ");
			if (currentLine.length >= 2) {
				// Go over the words in the line, search for two vocabulary words
				for (int i = 0; i < currentLine.length - 1; i++) {
					String firstWord = currentLine[i].toLowerCase();
					String secondWord = currentLine[i + 1].toLowerCase();

					// TODO: REMOVE: If one of the words is a number - not sure if necessary
					firstWord = isInteger(firstWord) ? SOME_NUM : firstWord;
					secondWord = isInteger(secondWord) ? SOME_NUM : secondWord;

					int firstIndex = indexOf(vocabulary, firstWord, vocabulary.length);
					int secondIndex = indexOf(vocabulary, secondWord, vocabulary.length);

					// If they do appear in the vocabulary
					if (firstIndex != ELEMENT_NOT_FOUND && secondIndex != ELEMENT_NOT_FOUND) {
						countsArray[firstIndex][secondIndex] += 1;
					}
				}
			}
			line = reader.readLine();
		}
		reader.close();
		return countsArray;
	}
	
	/*
	 * @pre: the method initModel was called (the language model is initialized)
	 * @pre: fileName is a legal file path
	 */
	public void saveModel(String fileName) throws IOException{ // Q-3
		// voc file
		File vocFile = new File(fileName + VOC_FILE_SUFFIX);
		BufferedWriter vocWriter = new BufferedWriter(new FileWriter(vocFile));
		vocWriter.write(mVocabulary.length + " words" + System.lineSeparator());
		for (int i = 0; i < mVocabulary.length; i++) {
			vocWriter.write(i + "," + mVocabulary[i] + System.lineSeparator());
		}
		vocWriter.close();

		// counts file
		File countsFile = new File(fileName + COUNTS_FILE_SUFFIX);
		BufferedWriter countsWriter = new BufferedWriter(new FileWriter(countsFile));
		for (int x = 0; x < mBigramCounts.length; x++) {
			for (int y = 0; y < mBigramCounts.length; y++) {
				// if the pair has been seen
				int bigramCount = mBigramCounts[x][y];
				if (bigramCount != 0) {
					String pair = x + "," + y + ":" + bigramCount;
					countsWriter.write(pair + System.lineSeparator());
				}
			}
		}
		countsWriter.close();
	}

	/*
	 * @pre: fileName is a legal file path
	 */
	public void loadModel(String fileName) throws IOException{ // Q - 4
		// voc file
		File vocFile = new File(fileName + VOC_FILE_SUFFIX);
		BufferedReader vocReader = new BufferedReader(new FileReader(vocFile));
		int vocSize = Integer.parseInt(vocReader.readLine().split(" ")[0]);
		String[] vocabulary = new String[vocSize];
		int vocIndex = 0;
		for (String line = vocReader.readLine(); line != null; line = vocReader.readLine()) {
			// split with limit of 2, in case the word itself contains ',' like "a,"
			vocabulary[vocIndex++] = line.split(",", 2)[1];
		}
		this.mVocabulary = vocabulary;
		vocReader.close();

		// counts file
		File countsFile = new File(fileName + COUNTS_FILE_SUFFIX);
		BufferedReader countsReader = new BufferedReader(new FileReader(countsFile));
		int[][] bigramCounts = new int[vocSize][vocSize];
		for (String line = countsReader.readLine(); line != null; line = countsReader.readLine()) {
			String[] lineSplit = line.split("[:,]");
			int firstIndex = Integer.parseInt(lineSplit[0]);
			int secondIndex = Integer.parseInt(lineSplit[1]);
			bigramCounts[firstIndex][secondIndex] = Integer.parseInt(lineSplit[2]);
		}
		this.mBigramCounts = bigramCounts;
		countsReader.close();
	}
	
	/*
	 * @pre: word is in lowercase
	 * @pre: the method initModel was called (the language model is initialized)
	 * @post: $ret = -1 if word is not in vocabulary, otherwise $ret = the index of word in vocabulary
	 */
	public int getWordIndex(String word){  // Q - 5
		return indexOf(this.mVocabulary, word, this.mVocabulary.length);
	}
	
	/*
	 * @pre: word1, word2 are in lowercase
	 * @pre: the method initModel was called (the language model is initialized)
	 * @post: $ret = the count for the bigram <word1, word2>. if one of the words does not
	 * exist in the vocabulary, $ret = 0
	 */
	public int getBigramCount(String word1, String word2){ //  Q - 6
		int firstIndex = this.getWordIndex(word1);
		int secondIndex = this.getWordIndex(word2);
		if (firstIndex == ELEMENT_NOT_FOUND || secondIndex == ELEMENT_NOT_FOUND)
			return 0;
		return this.mBigramCounts[firstIndex][secondIndex];
	}
	
	/*
	 * @pre word in lowercase, and is in mVocabulary
	 * @pre: the method initModel was called (the language model is initialized)
	 * @post $ret = the word with the lowest vocabulary index that appears most fequently after word (if a bigram starting with
	 * word was never seen, $ret will be null
	 */
	public String getMostFrequentProceeding(String word){ //  Q - 7
		int wordIndex = getWordIndex(word);
		// If the word is not in the vocabulary
		if (wordIndex == ELEMENT_NOT_FOUND)
			return null;

		int max = 0;
		String mostFrequent= null;
		for (int i = 0; i < this.mBigramCounts[wordIndex].length; i++) {
			if (this.mBigramCounts[wordIndex][i] > max) {
				max = this.mBigramCounts[wordIndex][i];
				mostFrequent = this.mVocabulary[i];
			}
		}
		return mostFrequent;
	}
	
	/* @pre: sentence is in lowercase
	 * @pre: the method initModel was called (the language model is initialized)
	 * @pre: each two words in the sentence are are separated with a single space
	 * @post: if sentence is is probable, according to the model, $ret = true, else, $ret = false
	 */
	public boolean isLegalSentence(String sentence){  //  Q - 8
		// Empty sentence
		if (sentence.length() == 0)
			return true;

		String[] words = sentence.split(" ");
		// Contains just one word
		if (words.length == 1) {
			return this.getWordIndex(words[0]) != ELEMENT_NOT_FOUND;
		}

		// Check all the full sentence
		for (int i = 0; i < words.length - 1; i++) {
			if (this.getBigramCount(words[i], words[i + 1]) == 0)
				return false;
		}
		return true;
	}

	/*
	 * @pre: word is in vocabulary
	 * @pre: the method initModel was called (the language model is initialized), 
	 * @post: $ret = w implies that w is the word with the largest cosineSimilarity(vector for word, vector for w) among all the
	 * other words in vocabulary
	 */
	public String getClosestWord(String word){ //  Q - 10
		// Vocabulary contains only one word
		if (this.mVocabulary.length == 1) {
			return this.mVocabulary[0];
		}
		int wordIndex = getWordIndex(word);
		int[] wordVector = this.mBigramCounts[wordIndex];
		String maxWord = this.mVocabulary[wordIndex];
		// minimum value of calcCosine is -1, and we start from -2 to catch the first one
		double maxCosine = -2;
		// Search for the maximum cosine with another word
		for (int i = 0; i < this.mBigramCounts.length; i++) {
			if (i != wordIndex) {
				double currentCosine = calcCosineSim(wordVector, this.mBigramCounts[i]);
				if (currentCosine > maxCosine) {
					maxCosine = currentCosine;
					maxWord = this.mVocabulary[i];
				}
			}
		}
		return maxWord;
	}
}
