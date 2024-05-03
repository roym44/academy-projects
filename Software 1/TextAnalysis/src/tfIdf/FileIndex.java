package tfIdf;

import java.io.File;
import java.io.IOException;
import java.util.*;

import histogram.IHistogram;
import histogram.HashMapHistogram;

public class FileIndex {
	
	private boolean isInitialized = false;
	private int numFiles; // number of indexed files
	private Map<String, IHistogram<String>> index; // maps indexed files to their histogram

	// counts for each file how many unique words it has by keeping T as the file name
	private IHistogram<String> fileUniqueHistogram; // a histogram of a file's unique words

	private IHistogram<String> wordContained; // a histogram of number of files a word is contained in

	private Map<String,List<Map.Entry<String, Double>>> fileToWordsTFIDF; // maps indexed files to a list of word-tfidf

	public FileIndex() {
		this.numFiles = 0;
		this.index = new HashMap<>();
		this.fileUniqueHistogram = new HashMapHistogram<>();
		this.wordContained = new HashMapHistogram<>();
		this.fileToWordsTFIDF = new HashMap<>();
	}

	/*
	 * @pre: the directory is no empty, and contains only readable text files
	 * @pre: isInitialized() == false;
	 */
  	public void indexDirectory(String folderPath) { //Q1
		File folder = new File(folderPath);
		File[] listFiles = folder.listFiles();
		// for every file in the folder
		for (File file : listFiles) {
			if (file.isFile()) {
				this.numFiles++;
				// Initialize a new histogram for the file
				this.index.put(file.getName(), new HashMapHistogram<>());
				try {
					List<String> tokens = FileUtils.readAllTokens(file);
					IHistogram<String> fileHistogram = this.index.get(file.getName());
					for (String token : tokens) {
						if (fileHistogram.getCountForItem(token) == 0) {
							// Add the file to the unique histogram, we found a new word
							this.fileUniqueHistogram.addItem(file.getName());
							// Add the word to the files containing histogram
							this.wordContained.addItem(token);
						}
						// Add token to the file's histogram
						fileHistogram.addItem(token);
					}
				}
				catch (IOException ex) {
					System.err.println("An error occurred while trying to read from: " + file.getName() + ex);
				}
			}
		}
		this.isInitialized = true;
	}

	// Q2

	/* @pre: isInitialized() */
	public int getCountInFile(String word, String fileName) throws FileIndexException {
		IHistogram<String> fileHistogram = this.index.get(fileName);
		if (fileHistogram == null)
			throw new FileIndexException(String.format("File %s not found", fileName));
		return fileHistogram.getCountForItem(word.toLowerCase());
	}

	/* @pre: isInitialized() */
	public int getNumOfUniqueWordsInFile(String fileName) throws FileIndexException {
		IHistogram<String> fileHistogram = this.index.get(fileName);
		if (fileHistogram == null) {
			throw new FileIndexException(String.format("File %s not found", fileName));
		}
		return this.fileUniqueHistogram.getCountForItem(fileName);
	}
	
	/* @pre: isInitialized() */
	public int getNumOfFilesInIndex(){
		return this.numFiles;
	}
	
	/* @pre: isInitialized() */
	public double getTF(String word, String fileName) throws FileIndexException{ // Q3
		IHistogram<String> fileHistogram = this.index.get(fileName);
		if (fileHistogram == null) {
			throw new FileIndexException(String.format("File %s not found", fileName));
		}
		return calcTF(fileHistogram.getCountForItem(word.toLowerCase()), fileHistogram.getCountsSum());
	}
	
	/* @pre: isInitialized() 
	 * @pre: exist fileName such that getCountInFile(word) > 0*/
	public double getIDF(String word){ //Q4
		return calcIDF(this.numFiles, this.wordContained.getCountForItem(word.toLowerCase()));
	}

	/*
	 * @pre: isInitialized()
	 * @pre: 0 < k <= getNumOfUniqueWordsInFile(fileName)
	 * @post: $ret.size() = k
	 * @post for i in (0,k-2):
	 * 		$ret[i].value >= $ret[i+1].value
	 */
	public List<Map.Entry<String, Double>> getTopKMostSignificantWords(String fileName, int k) 
													throws FileIndexException{ //Q5
		IHistogram<String> fileHistogram = this.index.get(fileName);
		if (fileHistogram == null) {
			throw new FileIndexException(String.format("File %s not found", fileName));
		}
		List<Map.Entry<String, Double>> wordsTFIDF = this.fileToWordsTFIDF.getOrDefault(fileName, null);
		// The list exist - return k pairs
		if (wordsTFIDF != null)
			return wordsTFIDF.subList(0, k);

		// The list doesn't exist, create it and sort it
		wordsTFIDF = this.createWordsTFIDF(fileName);
		wordsTFIDF.sort(new WordTFIDFComparator());
		this.fileToWordsTFIDF.put(fileName, wordsTFIDF);
		return wordsTFIDF.subList(0, k);
	}
	
	/* @pre: isInitialized() */
	public double getCosineSimilarity(String fileName1, String fileName2) throws FileIndexException{ //Q6
		if (this.index.get(fileName1) == null)
			throw new FileIndexException(String.format("File %s not found", fileName1));
		if (this.index.get(fileName1) == null)
			throw new FileIndexException(String.format("File %s not found", fileName2));

		double sumMult = 0;
		double sumSquared1 = 0;
		double sumSquared2 = 0;

		// Go over the words that are contained in some file
		for (String word : this.wordContained.getItemsSet()) {
			double TFIDF1 = getTFIDF(word, fileName1);
			double TFIDF2 = getTFIDF(word, fileName2);
			sumMult += TFIDF1 * TFIDF2;
			sumSquared1 += TFIDF1 * TFIDF1;
			sumSquared2 += TFIDF2 * TFIDF2;
		}
		return sumMult / Math.sqrt(sumSquared1 * sumSquared2);
	}
	
	/*
	 * @pre: isInitialized()
	 * @pre: 0 < k <= getNumOfFilesInIndex()-1
	 * @post: $ret.size() = k
	 * @post for i in (0,k-2):
	 * 		$ret[i].value >= $ret[i+1].value
	 */
	public List<Map.Entry<String, Double>> getTopKClosestDocuments(String fileName, int k)
			throws FileIndexException { //Q7
		if (this.index.get(fileName) == null)
			throw new FileIndexException(String.format("File %s not found", fileName));
		// Create a new list
		List<Map.Entry<String, Double>> topClosest = new ArrayList<>();
		// Go over all the files
		Set<String> set = this.fileUniqueHistogram.getItemsSet();
		for (String fileToCompare : set) {
			Map.Entry<String, Double> fileToSimilarity = new AbstractMap.SimpleEntry<>
					(fileToCompare, this.getCosineSimilarity(fileName, fileToCompare));
			topClosest.add(fileToSimilarity);
		}
		// Sort the list
		topClosest.sort(new WordTFIDFComparator());
		return topClosest.subList(1, k + 1);
	}

	
	
	// add private methods here, if needed
	private List<Map.Entry<String, Double>> createWordsTFIDF(String fileName) throws FileIndexException {
		List<Map.Entry<String, Double>> wordsTFIDF = new ArrayList<>();
		IHistogram<String> fileHistogram = this.index.get(fileName);
		// Add entries to the list
		for (Map.Entry<String, Integer> wordCount : fileHistogram) {
			String word = wordCount.getKey();
			// Get the word's TFIDF
			double TFIDF = this.getTFIDF(word, fileName);
			wordsTFIDF.add(new AbstractMap.SimpleEntry<>(word, TFIDF));
		}
		return wordsTFIDF;
	}

	private static class WordTFIDFComparator implements Comparator<Map.Entry<String, Double>>
	{
		@Override
		public int compare(Map.Entry<String, Double> a, Map.Entry<String, Double> b)
		{
			// compare values first
			int result = Double.compare(b.getValue(), a.getValue());
			if (result != 0)
				return result;
			// compare keys
			return a.getKey().compareTo(b.getKey());
		}
	}

	/*************************************************************/
	/********************* Don't change this ********************/
	/*************************************************************/
	
	public boolean isInitialized(){
		return this.isInitialized;
	}
	
	/* @pre: exist fileName such that getCountInFile(word) > 0*/
	public double getTFIDF(String word, String fileName) throws FileIndexException{
		return this.getTF(word, fileName)*this.getIDF(word);
	}
	
	private static double calcTF(int repetitionsForWord, int numOfWordsInDoc){
		return (double)repetitionsForWord/numOfWordsInDoc;
	}
	
	private static double calcIDF(int numOfDocs, int numOfDocsContainingWord){
		return Math.log((double)numOfDocs/numOfDocsContainingWord);
	}
	
}
