/*
# username - roymayan
# id1      - 206483554
# name1    - Roy Mayan
# id2      - 207110479
# name2    - Oded Neeman
 */

/**
 * FibonacciHeap - an implementation of a Fibonacci Heap over integers.
 */
public class FibonacciHeap {
    private HeapNode first;
    private HeapNode min;
    private int nodesCount;
    private int treesCount;
    private int markedCount;
    private static int totalCuts;
    private static int totalLinks;
    public static final double GOLDEN = (1 + Math.sqrt(5)) / 2;

    public FibonacciHeap() { }

    public HeapNode getFirst() {
        return this.first;
    }

    public HeapNode getLast() {
        if (this.isEmpty())
            return null;
        return this.first.getPrev();
    }

    /**
     * public boolean isEmpty()
     * <p>
     * Returns true if and only if the heap is empty.
     */
    public boolean isEmpty() {
        return this.first == null;
    }

    /**
     * public HeapNode insert(int key)
     * <p>
     * Creates a node (of type HeapNode) which contains the given key, and inserts it into the heap.
     * The added key is assumed not to already belong to the heap.
     * <p>
     * Returns the newly created node.
     */
    public HeapNode insert(int key) {
        // LAZY INSERT
        HeapNode node = new HeapNode(key);
        this.insertNode(node, false);
        return node;
    }

    public void insertNode(HeapNode node, boolean consolidate) {
        if (this.isEmpty()) {
            // just update the min
            this.min = node;
        } else {
            HeapNode oldFirst = this.first;
            // keep old first's prev (=last) before updating it
            HeapNode last = oldFirst.getPrev();
            // update node's pointers
            node.setNextPrev(oldFirst);
            // update last's pointers
            last.setNextPrev(node);
            // update min
            if (node.getKey() < this.min.getKey())
                this.min = node;
        }
        // node is always inserted as first
        this.first = node;
        // we get 1 new tree always
        this.treesCount++;
        // in consolidate we maintain nodesCount separately
        if (!consolidate)
            this.nodesCount++;
    }


    /**
     * public void deleteMin()
     * <p>
     * Deletes the node containing the minimum key.
     */
    public void deleteMin() {
        // TODO: we assume we don't call this function for an empty heap
        // Fix pointers
        HeapNode child = this.min.getChild();
        // Only one tree in the heap
        if (this.treesCount == 1) {
            // Has no children - sole node
            if (child == null) {
                this.first = null;
                this.min = null;
                // there aren't any children - we remove the whole tree
                this.treesCount--;
                this.nodesCount--;
                return;
            } else {
                this.first = child;
                this.treesCount = this.min.getRank();
            }
        } else {
            // min has no children - fix pointers on root level
            if (child == null) {
                // update first (we always take first on the left)
                if (this.min == this.first)
                    this.first = this.min.getNext();
                // 1 pointer fixed
                this.min.getPrev().setNextPrev(this.min.getNext());
                this.treesCount--;
            }
            // fix new exposed children pointers that now join root level
            else {
                // update first to be child
                if (this.min == this.first)
                    this.first = child;
                // 2 pointers fixed
                // child's prev is the last node exposed after we remove min
                child.getPrev().setNextPrev(this.min.getNext());
                // now we can edit this one - the order is crucial!
                this.min.getPrev().setNextPrev(child);
                // we removed min (1 tree) and we got new trees (rank = number of children)
                this.treesCount += this.min.getRank() - 1;
            }
        }
        // we always remove 1 and only 1 node (min)
        this.nodesCount--;
        // call consolidate - will take care of unmarking the new roots and finding new min
        this.consolidate();
    }

    public void consolidate() {
        // to-buckets
        int bucketsSize = (int) (Math.log(nodesCount) / Math.log(GOLDEN)) + 1;
        HeapNode[] buckets = new HeapNode[bucketsSize];

        // using x to iterate over the roots
        HeapNode x = this.first;
        x.getPrev().setNext(null); // Make the root level list not circular
        while (x != null) {
            // first thing - unmark and decrease marked count
            if (x.getMark()) {
                this.markedCount--;
                x.setMark(false);
            }
            // y is our current root to take care of
            HeapNode y = x;
            x = x.getNext();
            // reset y's pointers (we want to keep the child)
            y.resetKeepChild();

            while (buckets[y.getRank()] != null) {
                // linking takes care of updating the rank of new parent
                y = y.link(buckets[y.getRank()]);
                buckets[y.getRank() - 1] = null;
            }
            buckets[y.getRank()] = y;
        }

        // reset the heap except nodesCount that we maintain
        this.first = null;
        this.min = null;
        this.treesCount = 0;

        // from-buckets
        // we go from the end of the buckets array to the beginning because
        // our insert is always insert-first, so this.first will point to the lowest ranked tree from the buckets
        for (int i = bucketsSize - 1; i >= 0; i--) {
            // the bucket is not empty
            if (buckets[i] != null)
                // insert takes care of - minimum, treesCount, nodesCount
                this.insertNode(buckets[i], true);
        }
    }


    /**
     * public HeapNode findMin()
     * <p>
     * Returns the node of the heap whose key is minimal, or null if the heap is empty.
     */
    public HeapNode findMin() {
        return this.min;
    }

    /**
     * public void meld (FibonacciHeap heap2)
     * <p>
     * Melds heap2 with the current heap.
     */
    public void meld(FibonacciHeap heap2) {
        // LAZY MELD
        if (heap2.isEmpty())
            return;
        if (this.isEmpty()) {
            this.first = heap2.first;
            this.min = heap2.min;
        } else {
            HeapNode heap2last = heap2.getLast();
            // connect this.last with heap2.first
            this.getLast().setNextPrev(heap2.first);
            // connect heap2.last with this.first
            heap2last.setNextPrev(this.first);
            // update min
            if (heap2.min.getKey() < this.min.getKey())
                this.min = heap2.min;
        }
        // update counts
        this.nodesCount += heap2.nodesCount;
        this.treesCount += heap2.treesCount;
        this.markedCount += heap2.markedCount;
    }

    /**
     * public int size()
     * <p>
     * Returns the number of elements in the heap.
     */
    public int size() {
        return this.nodesCount;
    }

    /**
     * public int[] countersRep()
     * <p>
     * Return an array of counters. The i-th entry contains the number of trees of order i in the heap.
     * (Note: The size of the array depends on the maximum order of a tree.)
     */
    public int[] countersRep() {
        // empty case
        if (this.isEmpty())
            return new int[]{};
        this.first.getPrev().setNext(null); // Make the root level list not circular
        HeapNode x = this.first;
        int maxRank = 0;
        while (x != null) {
            if (x.getRank() > maxRank)
                maxRank = x.getRank();
            x = x.getNext();
        }

        int[] counters = new int[maxRank + 1];
        x = this.first;
        while (x != null) {
            counters[x.getRank()]++;
            x = x.getNext();
        }
        this.first.getPrev().setNext(this.first); // Restore state
        return counters;
    }

    /**
     * public void delete(HeapNode x)
     * <p>
     * Deletes the node x from the heap.
     * It is assumed that x indeed belongs to the heap.
     */
    public void delete(HeapNode x) {
        int delta = x.getKey() - this.min.getKey();
        // we make x the new minimum
        this.decreaseKey(x, delta + 1);
        this.deleteMin();
    }

    public void cascadingCuts(HeapNode x) {
        HeapNode y = x.getParent();
        // decrease marked count if the cut node was marked
        if (x.getMark())
            this.markedCount--;
        x.cut();
        // we assume insert receives a node which doesn't exist in the heap
        // so we decrease the nodesCount, only for insert to increase it later
        this.nodesCount--;
        this.insertNode(x, false);
        if (!y.isRoot()) {
            // mark the parent that we cut its child
            if (!y.getMark()) {
                y.setMark(true);
                this.markedCount++;
            } else
                this.cascadingCuts(y);
        }
    }

    /**
     * public void decreaseKey(HeapNode x, int delta)
     * <p>
     * Decreases the key of the node x by a non-negative value delta. The structure of the heap should be updated
     * to reflect this change (for example, the cascading cuts procedure should be applied if needed).
     */
    public void decreaseKey(HeapNode x, int delta) {
        x.setKey(x.getKey() - delta);
        if (x.isRoot()) {
            if (x.getKey() < this.min.getKey())
                this.min = x;
        } else {
            if (x.getKey() < x.getParent().getKey())
                this.cascadingCuts(x);
        }
    }

    /**
     * public int nonMarked()
     * <p>
     * This function returns the current number of non-marked items in the heap
     */
    public int nonMarked() {
        return this.nodesCount - this.markedCount;
    }

    /**
     * public int potential()
     * <p>
     * This function returns the current potential of the heap, which is:
     * Potential = #trees + 2*#marked
     * <p>
     * In words: The potential equals to the number of trees in the heap
     * plus twice the number of marked nodes in the heap.
     */
    public int potential() {
        return this.treesCount + 2 * this.markedCount;
    }

    /**
     * public static int totalLinks()
     * <p>
     * This static function returns the total number of link operations made during the
     * run-time of the program. A link operation is the operation which gets as input two
     * trees of the same rank, and generates a tree of rank bigger by one, by hanging the
     * tree which has larger value in its root under the other tree.
     */
    public static int totalLinks() {
        return FibonacciHeap.totalLinks;
    }

    /**
     * public static int totalCuts()
     * <p>
     * This static function returns the total number of cut operations made during the
     * run-time of the program. A cut operation is the operation which disconnects a subtree
     * from its parent (during decreaseKey/delete methods).
     */
    public static int totalCuts() {
        return FibonacciHeap.totalCuts; // should be replaced by student code
    }

    /**
     * public static int[] kMin(FibonacciHeap H, int k)
     * <p>
     * This static function returns the k smallest elements in a Fibonacci heap that contains a single tree.
     * The function should run in O(k*deg(H)). (deg(H) is the degree of the only tree in H.)
     * <p>
     * ###CRITICAL### : you are NOT allowed to change H.
     */
    public static int[] kMin(FibonacciHeap H, int k) {
        int[] arr = new int[k];
        if (k == 0)
            return arr;
        FibonacciHeap heap = new FibonacciHeap();
        // the first minimal key
        HeapNode min = H.getFirst();
        arr[0] = min.getKey();
        min.setOriginal(min); // just for the first case - because this min didn't come from heap, but from H itself
        for (int i = 1; i < k; i++) {
            // add the children
            HeapNode x = min.getOriginal().getChild();
            // only if there are children
            if (x != null) {
                do {
                    HeapNode newNode = new HeapNode(x.getKey());
                    newNode.setOriginal(x); // we must keep a pointer back to the original node in H
                    heap.insertNode(newNode, false);
                    x = x.getNext();
                }
                while (x != min.getOriginal().getChild());
            }

            // find the next minimum
            min = heap.findMin();
            heap.deleteMin(); // remove from the heap
            arr[i] = min.getKey();
        }
        return arr;
    }

    /**
     * HeapNode - an implementation of a Fibonacci Heap integer node.
     */
    public static class HeapNode {
        private int key;
        private int rank;
        private boolean mark;
        private HeapNode child;
        private HeapNode next;
        private HeapNode prev;
        private HeapNode parent;
        private HeapNode original; // for kMin only!

        public HeapNode(int key) {
            this.key = key;
            this.rank = 0;
            this.mark = false;
            this.child = null;
            // we want a doubly-linked list
            this.next = this;
            this.prev = this;
            this.parent = null;
        }

        public int getKey() {
            return this.key;
        }

        public void setKey(int key) {
            this.key = key;
        }

        public int getRank() {
            return this.rank;
        }

        public boolean getMark() {
            return this.mark;
        }

        public void setMark(boolean mark) {
            this.mark = mark;
        }

        public HeapNode getChild() {
            return child;
        }

        public void setChild(HeapNode child) {
            this.child = child;
            // this is important to update the rank after adding a new child
            this.rank++;
        }

        public HeapNode getNext() {
            return next;
        }

        public void setNext(HeapNode next) {
            this.next = next;
        }

        public HeapNode getPrev() {
            return prev;
        }

        public void setPrev(HeapNode prev) {
            this.prev = prev;
        }

        public void setNextPrev(HeapNode next) {
            this.setNext(next);
            next.setPrev(this);
        }

        public HeapNode getParent() {
            return parent;
        }

        public void setParent(HeapNode parent) {
            this.parent = parent;
        }

        public void setParentChild(HeapNode parent) {
            this.setParent(parent);
            parent.setChild(this);
        }

        public void resetKeepChild() {
            this.setNextPrev(this);
            this.setParent(null);
        }

        public HeapNode getOriginal() {
            return original;
        }

        public void setOriginal(HeapNode original) {
            this.original = original;
        }

        public boolean isRoot() {
            return this.parent == null;
        }

        public HeapNode link(HeapNode other) {
            // we want to keep the smaller key as the parent
            FibonacciHeap.totalLinks++;
            if (other.key < this.key) {
                // fix children pointers
                if (other.child != null) {
                    // fix last pointer
                    other.child.prev.setNextPrev(this);
                    this.setNextPrev(other.child);
                }
                this.setParentChild(other);
                return other;
            } else {
                // fix children pointers
                if (this.child != null) {
                    // fix last pointer
                    this.child.prev.setNextPrev(other);
                    other.setNextPrev(this.child);
                }
                other.setParentChild(this);
                return this;
            }
        }

        public void cut() {
            HeapNode y = this.parent;
            this.parent = null;
            this.mark = false;
            y.rank -= 1;
            if (this.next == this)
                y.child = null;
            else {
                // in case we cut the child
                if (y.child == this) {
                    y.child = this.next;
                }
                // fix pointers around the node that we cut
                this.prev.setNextPrev(this.next);
                this.resetKeepChild();
            }
            FibonacciHeap.totalCuts++;
        }
    }
}