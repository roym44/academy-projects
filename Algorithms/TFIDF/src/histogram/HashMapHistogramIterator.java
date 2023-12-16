package il.ac.tau.cs.sw1.ex8.histogram;

import java.util.*;

public class HashMapHistogramIterator<T extends Comparable<T>> 
							implements Iterator<Map.Entry<T, Integer>>{
	private final List<Map.Entry<T, Integer>> mapList;
	private int index;

	public HashMapHistogramIterator(HashMap<T, Integer> histogram) {
		this.mapList = new ArrayList<>(histogram.entrySet());
		this.mapList.sort(new EntryComparator());
		this.index = 0;
	}

	@Override
	public boolean hasNext() {
		return index < this.mapList.size();
	}

	@Override
	public Map.Entry<T, Integer> next() {
		// get current entry at current index and increment it
		return this.mapList.get(this.index++);
	}

	@Override
	public void remove() {
		throw new UnsupportedOperationException();
		
	}

	private class EntryComparator implements Comparator<Map.Entry<T, Integer>> {
		@Override
		public int compare(Map.Entry<T, Integer> a, Map.Entry<T, Integer> b) {
			// compare keys first
			int result = a.getKey().compareTo(b.getKey());
			if (result != 0)
				return result;
			// compare values
			return Integer.compare(b.getValue(), a.getValue());
		}
	}
}
