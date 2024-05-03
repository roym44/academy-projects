package histogram;

import java.util.*;

public class HashMapHistogram<T extends Comparable<T>> implements IHistogram<T> {

    private HashMap<T, Integer> map;
    public HashMapHistogram() {
		this.map = new HashMap<>();
    }

    @Override
    public void addItem(T item) {
		// get the current count
		int count = this.getCountForItem(item);
		// increment the count - put will replace the value
		if (count > 0)
			this.map.put(item, count + 1);
		// insert new item with count 1
		else
			this.map.put(item, 1);
    }

    @Override
    public boolean removeItem(T item) {
		return this.map.remove(item) != null;
    }

    @Override
    public void addAll(Collection<T> items) {
		for (T item: items) {
			this.addItem(item);
		}
    }

    @Override
    public int getCountForItem(T item) {
       return this.map.getOrDefault(item, 0);
    }

    @Override
    public void clear() {
        this.map.clear();
    }

    @Override
    public Set<T> getItemsSet() {
		Set<T> itemsSet = new HashSet<>();
		for (Map.Entry<T, Integer> entry : this.map.entrySet()) {
			// add if the count is more than 0
			if (entry.getValue() > 0)
				itemsSet.add(entry.getKey());
		}
		return itemsSet;
    }

	@Override
	public int getCountsSum() {
		int sum = 0;
		for (int value : this.map.values()) {
			sum += value;
		}
		return sum;
	}

    @Override
    public Iterator<Map.Entry<T, Integer>> iterator() {
        return new HashMapHistogramIterator<T>(this.map);

    }
}
