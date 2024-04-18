package il.ac.tau.cs.sw1.ex7;
import java.util.*;

public class FractionalKnapSack implements Greedy<FractionalKnapSack.Item>{
    int capacity;
    List<Item> lst;

    FractionalKnapSack(int c, List<Item> lst1){
        capacity = c;
        lst = lst1;
    }

    public static class Item {
        double weight, value;
        Item(double w, double v) {
            weight = w;
            value = v;
        }

        @Override
        public String toString() {
            return "{" + "weight=" + weight + ", value=" + value + '}';
        }
    }

    private static class ItemComparator implements Comparator<Item> {
        public int compare(Item i1, Item i2) {
            // Comparison by relative weight: value/weight
            return Double.compare(i2.value / i2.weight, i1.value / i1.weight);
        }
    }

    @Override
    public Iterator<Item> selection() {
        List<Item> sortedList = new ArrayList<>(this.lst);
        sortedList.sort(new ItemComparator());
        return sortedList.iterator();
    }

    private static double getListWeight(List<Item> candidates_lst){
        double listWeight = 0;
        for (Item item : candidates_lst) {
            listWeight += item.weight;
        }
        return listWeight;
    }

    @Override
    public boolean feasibility(List<Item> candidates_lst, Item element) {
        return getListWeight(candidates_lst) < this.capacity;
    }

    @Override
    public void assign(List<Item> candidates_lst, Item element) {
        double weightList = getListWeight(candidates_lst);
        double itemWeight = Math.min(element.weight, this.capacity - weightList);
        double relativeValue = element.value * itemWeight / element.weight;
        candidates_lst.add(new Item(itemWeight, relativeValue));
    }

    @Override
    public boolean solution(List<Item> candidates_lst) {
        return getListWeight(candidates_lst) == this.capacity;
    }
}
