import java.util.*;


public class Graph implements Greedy<Graph.Edge> {
    List<Edge> lst; //Graph is represented in Edge-List. It is undirected. Assumed to be connected.
    int n; //nodes are in [0,..., n]

    Graph(int n1, List<Edge> lst1) {
        lst = lst1;
        n = n1;
    }

    public static class Edge {
        int node1, node2;
        double weight;

        Edge(int n1, int n2, double w) {
            node1 = n1;
            node2 = n2;
            weight = w;
        }

        @Override
        public String toString() {
            return "{" + "(" + node1 + "," + node2 + "), weight=" + weight + '}';
        }
    }

    private static class EdgeComparator implements Comparator<Edge> {
        public int compare(Edge e1, Edge e2) {
            // Comparison by weight, according to node1 and node2
            if (e1.weight == e2.weight) {
                if (e1.node1 == e2.node1) {
                    if (e1.node2 == e2.node2)
                        return 0;
                    return (e1.node2 > e2.node2) ? 1 : -1;
                }
                return (e1.node1 > e2.node1) ? 1 : -1;
            }
            return (e1.weight > e2.weight) ? 1 : -1;
        }
    }

    @Override
    public Iterator<Edge> selection() {
        List<Edge> sortedList = new ArrayList<>(this.lst);
        sortedList.sort(new Graph.EdgeComparator());
        return sortedList.iterator();
    }

    private boolean existPathRec(List<Integer> visited, List<Edge> candidates, int src, int dest) {
        if (src == dest)
            return true;

        // mark src as visited
        visited.add(src);

        // go forward with src using the edges, until reaching the dest
        for (int i = 0; i < candidates.size(); i++) {
            Edge edge = candidates.get(i);
            // move using edge from src (edge's node1) to a next node (edge's node2)
            if (edge.node1 == src && !visited.contains(edge.node2))
                return existPathRec(visited, candidates, edge.node2, dest);
                // move using edge from src (edge's node2) to a next node (edge's node1)
            else if (edge.node2 == src && !visited.contains(edge.node1))
                return existPathRec(visited, candidates, edge.node1, dest);
        }
        return false;
    }


    @Override
    public boolean feasibility(List<Edge> candidates_lst, Edge element) {
        List<Integer> visited = new ArrayList<>();
        return !existPathRec(visited, candidates_lst, element.node1, element.node2);
    }

    @Override
    public void assign(List<Edge> candidates_lst, Edge element) {
        candidates_lst.add(element);
    }

    @Override
    public boolean solution(List<Edge> candidates_lst) {
        return candidates_lst.size() == n; // There are n + 1 edges.
    }
}
