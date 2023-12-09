# username - odedneeman
# id1      - 207110479
# name1    - Oded Neeman
# id2      - 206483554
# name2    - Roy Mayan

import random

"""A class representing a node in an AVL tree"""

class AVLNode(object):
	"""constructor
	@type value: str
	@param value: data of your node
	@type is_real: bool
	@param is_real: True if the node should be created real, False if virtual
	@complexity: O(1)
	"""

	def __init__(self, value, is_real=True):
		# Create two virtual children for a real node
		left_virtual, right_virtual = None, None
		if is_real:
			left_virtual = AVLNode("", is_real=False)
			right_virtual = AVLNode("", is_real=False)
			left_virtual.setParent(self)
			right_virtual.setParent(self)

		self.value = None if (not is_real) else value
		self.left = left_virtual
		self.right = right_virtual
		self.parent = None
		self.height = -1 if (not is_real) else 0
		self.size = 0 if (not is_real) else 1
		self.is_real = is_real  # real = not virtual

	"""returns the left child
	@rtype: AVLNode
	@returns: the left child of self, None if there is no left child
	@complexity: O(1)
	"""

	def getLeft(self):
		return self.left

	"""returns the right child
	@rtype: AVLNode
	@returns: the right child of self, None if there is no right child
	@complexity: O(1)
	"""

	def getRight(self):
		return self.right

	"""returns the parent 
	@rtype: AVLNode
	@returns: the parent of self, None if there is no parent
	@complexity: O(1)
	"""

	def getParent(self):
		return self.parent

	"""return the value
	@rtype: str
	@returns: the value of self, None if the node is virtual
	@complexity: O(1)
	"""

	def getValue(self):
		return self.value

	"""returns the height
	@rtype: int
	@returns: the height of self, -1 if the node is virtual
	@complexity: O(1)
	"""

	def getHeight(self):
		return self.height

	"""returns the size
	@rtype: int
	@returns: the size of self, 0 if the node is virtual
	@complexity: O(1)
	"""

	def getSize(self):
		return self.size

	"""sets left child
	@type node: AVLNode
	@param node: a node
	@complexity: O(1)
	"""

	def setLeft(self, node):
		self.left = node

	"""sets right child
	@type node: AVLNode
	@param node: a node
	@complexity: O(1)
	"""

	def setRight(self, node):
		self.right = node

	"""sets parent
	@type node: AVLNode
	@param node: a node
	@complexity: O(1)
	"""

	def setParent(self, node):
		self.parent = node

	"""sets value
	@type value: str
	@param value: data
	@complexity: O(1)
	"""

	def setValue(self, value):
		self.value = value

	"""sets the height of the node
	@type h: int
	@param h: the height
	@complexity: O(1)
	"""

	def setHeight(self, h):
		self.height = h

	"""sets the size of the node
	@type s: int
	@param s: the size
	@complexity: O(1)
	"""

	def setSize(self, s):
		self.size = s

	"""returns whether self is not a virtual node 
	@rtype: bool
	@returns: False if self is a virtual node, True otherwise.
	"""

	def isRealNode(self):
		return self.is_real

	"""returns the balance factor
	@rtype: int
	@returns: the balance factor of the node
	@complexity: O(1)
	"""

	def getBF(self):
		return self.left.height - self.right.height

	"""updates the size of the node based on its children
	@complexity: O(1)
	"""

	def updateSize(self):
		self.size = self.left.size + self.right.size + 1

	"""updates the height of the node based on its children
	@complexity: O(1)
	"""

	def updateHeight(self):
		self.height = max(self.left.height, self.right.height) + 1


"""
A class implementing the ADT list, using an AVL tree.
"""

class AVLTreeList(object):

	"""constructor
	@complexity: O(1)
	"""
	def __init__(self):
		self.root = None
		self.first_node = None
		self.last_node = None

	"""returns whether the list is empty
	@rtype: bool
	@returns: True if the list is empty, False otherwise
	@complexity: O(1)
	"""
	def empty(self):
		return self.root is None



	"""retrieves the value of the i'th item in the list
	@type i: int
	@pre: 0 <= i < self.length()
	@param i: index in the list
	@rtype: str
	@returns: the value of the i'th item in the list
	@complexity: O(logn)
	"""
	def retrieve(self, i):
		if i >= self.length():
			return None
		return AVLTreeList.selectRec(self.root, i + 1).getValue()

	"""perform shared operations for both types of rotations (right/left)
	@type A: AVLNode
	@param A: a child of B
	@type B: AVLNode
	@param B: discovered as an AVL criminal
	@complexity: O(1)
	"""
	def rotate(self, A, B):
		""" Take care of parents: now A's parent is B's original parent.
		if B was the root - its parent is None and now A's parent is None
		"""
		A.setParent(B.getParent())

		# Maybe B was the root so its parent is None (which is now A's parent)
		if A.getParent() is None:
			self.root = A
		elif A.getParent() is not None:
			""" we want to find out whether B was a left or right son of its parent
			B's parent still has its left and right original pointers
			"""
			bparent_left = A.getParent().getLeft()
			if bparent_left == B: # B was its parent LEFT son
				A.getParent().setLeft(A)
			else: # B was its parent RIGHT son - there is no other option
				A.getParent().setRight(A)

		# We always do this
		B.setParent(A)
		# Update size and height
		A.setSize(B.getSize())
		B.updateSize()
		B.updateHeight()
		A.updateHeight()

	"""perform right rotation and takes care of updating size/height fields
	@type criminal: AVLNode
	@param criminal: the AVL criminal who initiated the rotation
	@complexity: O(1)
	"""
	def rightRotation(self, criminal):
		# Get pointers
		B = criminal
		A = criminal.getLeft()
		AR = A.getRight()
		# Update pointers
		B.setLeft(AR)
		B.getLeft().setParent(B)
		A.setRight(B)

		self.rotate(A, B)

	"""perform left rotation and takes care of updating size/height fields
	@type criminal: AVLNode
	@param criminal: the AVL criminal who initiated the rotation
	@complexity: O(1)
	"""
	def leftRotation(self, criminal):
		# Get pointers
		B = criminal
		A = criminal.getRight()
		AL = A.getLeft()
		# Update pointers
		B.setRight(AL)
		B.getRight().setParent(B)
		A.setLeft(B)

		self.rotate(A, B)

	"""takes care of rotations for Insert and Delete altogether depending on the BF of the criminal
	@type criminal: AVLNode
	@param criminal: the AVL criminal who initiated the rotation
	@rtype: int
	@returns: the number of rotations performed
	@complexity: O(1)
	"""
	def performRotations(self, criminal):
		bf = criminal.getBF()
		if bf == 2:
			left_bf = criminal.getLeft().getBF()
			if left_bf == -1:
				self.leftRotation(criminal.getLeft())
				self.rightRotation(criminal)
				return 2
			elif left_bf == 1 or left_bf == 0:
				self.rightRotation(criminal)
				return 1
		else: # has to be -2
			right_bf = criminal.getRight().getBF()
			if right_bf == 1:
				self.rightRotation(criminal.getRight())
				self.leftRotation(criminal)
				return 2
			elif right_bf == -1 or right_bf == 0:
				self.leftRotation(criminal)
				return 1

	""" fixes the tree after some manipulation performed on it (insert/delete/join)
	@type start_node: AVLNode
	@param start_node: the node to start fixing the tree from (up to the root)
	@type is_delete: bool
	@param is_delete: True if we are in case of delete (or join) - don't stop after one fix, False otherwise in case of insert
	@rtype: int
	@returns: the total number of fixes (rotations) performed
	@complexity: O(logn)
	"""
	def fixTree(self, start_node, is_delete=False):
		fixes_count = 0
		y = start_node.getParent()
		# Take care of criminals and height+size fields
		fixed_criminals = False
		while y is not None:
			# Height - get old and new heights
			old_height = y.getHeight()
			y.updateHeight()
			new_height = y.getHeight()
			# Size - update size field every time we enter the loop
			y.updateSize()

			if fixed_criminals: # Done fixing - keep going up to fix size field
				y = y.getParent()
			else: # Fix criminals
				bf = y.getBF()
				# "terminate" but actually go up to fix size
				if abs(bf) < 2 and old_height == new_height:
					y = y.getParent()
					# Only if we are in Insert - we finished fixing
					if not is_delete:
						fixed_criminals = True
				elif abs(bf) < 2 and old_height != new_height:
					y = y.getParent()
				# criminal!
				elif abs(bf) == 2:
					# Delete - we accumulate current rotation, and we may have to perform more Rotations
					if is_delete:
						fixes_count += self.performRotations(y)
						y = y.getParent()
					# Insert - it is guaranteed that we perform only 1 rotation (R,L,RL,LR)
					else:
						# = or += not relevant here, because we know that for Insert fixes_count was 0 before
						fixes_count = self.performRotations(y)
						# "terminate" but actually go up to fix size
						fixed_criminals = True
						y = y.getParent()
		return fixes_count


	"""inserts val at position i in the list
	@type i: int
	@pre: 0 <= i <= self.length()
	@param i: The intended index in the list to which we insert val
	@type val: str
	@param val: the value we inserts
	@rtype: list
	@returns: the number of rebalancing operation due to AVL rebalancing
	@complexity: O(logn)
	"""
	def insert(self, i, val):
		z = AVLNode(val)
		# First insertion to an empty tree
		if self.empty():
			self.root = z
			self.first_node = z
			self.last_node = z
		# Insert-first
		elif i == 0:
			first_node = self.first_node
			first_node.setLeft(z) # make z its left child
			z.setParent(first_node) # update parent
			self.first_node = z # update first_node field
		# Insert-last
		elif i == self.length():
			last_node = self.last_node
			last_node.setRight(z) # make z its right child
			z.setParent(last_node) # update parent
			self.last_node = z # update last_node field
		# Any other index
		else:
			node = AVLTreeList.selectRec(self.root, i + 1)  # retrieve(i) = node of rank i+1
			# Has no left child
			if not node.getLeft().isRealNode():
				node.setLeft(z)
				z.setParent(node)
			# Get predecessor
			else:
				node_pred = AVLTreeList.getPredecessor(node)
				node_pred.setRight(z)
				z.setParent(node_pred)

		return self.fixTree(z)


	"""perform a bypass between node's parent and given child
	This works for both cases of node: leaf / has one child.
	@type node: AVLNode
	@pre: node != self.root
	@param node: the node on which to perform the bypass
	@type child: AVLNode
	@param child: the child of the node for the bypass
	@complexity: O(1)
	"""
	def deleteLeafOrSingle(self, node, child):
		parent = node.getParent()
		# Check if the leaf was its parent left son
		if node == parent.getLeft():
			parent.setLeft(child)
			parent.getLeft().setParent(parent)
		# It has to be its right son
		else:
			parent.setRight(child)
			parent.getRight().setParent(parent)


	"""deletes the i'th item in the list
	@type i: int
	@pre: 0 <= i < self.length()
	@param i: The intended index in the list to be deleted
	@rtype: int
	@returns: the number of re-balancing operation due to AVL re-balancing
	@complexity: O(logn)
	"""
	def delete(self, i):
		# empty tree case AND illegal i
		if i >= self.length():
			return -1

		# retrieve the node in index i == select(i+1)
		z = AVLTreeList.selectRec(self.root, i + 1)

		# z is either a leaf or has one child (can be only the right child)
		left_real = z.getLeft().isRealNode()
		right_real = z.getRight().isRealNode()
		if (not left_real) or (not right_real):
			# Take care of root deletion
			if self.root == z:
				# leaf
				if (not left_real) and (not right_real):
					self.root = None
					self.first_node = None
					self.last_node = None
				elif left_real:
					self.root = z.getLeft()
					self.root.setParent(None)
					self.last_node = self.root
				# right_real
				else:
					self.root = z.getRight()
					self.root.setParent(None)
					self.first_node = self.root
				# no need to fix anything
				return 0
			else:
				"""
				z has only one child - bypass it:
					we send the true child to deleteLeafOrSingle and it will connect it to z's parent
					as wanted. 
				z is a leaf - remove it:
					both children will be virtual nodes, and deleteLeafOrSingle will connect either of them to z's parent
					which is what we want, since we remove z and replace it with a virtual node
				"""
				# Maintain first and last
				if z == self.first_node:
					self.first_node = AVLTreeList.getSuccessor(z)
				elif z == self.last_node:
					self.last_node = AVLTreeList.getPredecessor(z)

				if left_real:
					self.deleteLeafOrSingle(z, z.getLeft())
				else:
					# getRight() could be virtual, but it's fine for the leaf situation
					self.deleteLeafOrSingle(z, z.getRight())

				return self.fixTree(z, is_delete=True)

		# z has two children - replace with successor
		# also: in this situation, z can't be first/last node
		else:
			suc = AVLTreeList.getSuccessor(z)
			# Delete successor - he has no left child, so we bypass with right child
			self.deleteLeafOrSingle(suc, suc.getRight())
			# now succ.getRight() will have as its parent, succ's original parent - we need to fix from it
			start_node = suc.getRight()

			# Replace z with its successor
			# Fix parent pointers
			parent = z.getParent()
			suc.setParent(parent)
			# Take care of root deletion
			if self.root == z:
				self.root = suc
			else:
				if parent.getLeft() == z:
					parent.setLeft(suc)
				else:
					parent.setRight(suc)

			# Fix children pointers
			z.getLeft().setParent(suc)
			suc.setLeft(z.getLeft())
			z.getRight().setParent(suc)
			suc.setRight(z.getRight())

			# Update size and height because successor has changed its place
			suc.updateHeight()
			suc.updateSize()

			return self.fixTree(start_node, is_delete=True)


	"""returns the value of the first item in the list
	@rtype: str
	@returns: the value of the first item, None if the list is empty
	@complexity: O(1)
	"""
	def first(self):
		return None if self.empty() else self.first_node.getValue()

	"""returns the value of the last item in the list
	@rtype: str
	@returns: the value of the last item, None if the list is empty
	@complexity: O(1)
	"""
	def last(self):
		return None if self.empty() else self.last_node.getValue()

	"""returns an array representing list 
	@rtype: list
	@returns: a list of strings representing the data structure
	@complexity: O(n)
	"""
	def listToArray(self):
		arr = []
		if self.empty():
			return arr
		# Use an in-order traversal
		AVLTreeList.inOrderRec(self.root, arr)
		return arr

	"""returns the size of the list 
	@rtype: int
	@returns: the size of the list
	@complexity: O(1)
	"""
	def length(self):
		return 0 if self.empty() else self.root.getSize()

	"""permute the info values of the list 
	@rtype: list
	@returns: an AVLTreeList where the values are permuted randomly by the info of the original list. ##Use Randomness
	@complexity: O(n)
	"""
	def permutation(self):
		lst = self.listToArray()
		n = len(lst)
		# shuffle the list
		for i in range(n - 1): # 0...n-2
			j = random.randint(i + 1, n - 1)  # select a random index
			lst[i], lst[j] = lst[j], lst[i]  # swap the two items

		# We treat lst as "sorted" so we use what we saw in Recitation 5
		root = AVLTreeList.sortedArrayToAVL(lst, 0, len(lst) - 1)
		perm_tree = AVLTreeList()
		perm_tree.root = root
		if n > 0:
			perm_tree.first = perm_tree.retrieve(0)
			perm_tree.last = perm_tree.retrieve(n - 1)
		return perm_tree

	"""sort the info values of the list
	@rtype: list
	@returns: an AVLTreeList where the values are sorted by the info of the original list.
	@complexity: O(nlogn)
	"""
	def sort(self):
		arr = self.listToArray()
		sorted = AVLTreeList.mergeSort(arr)
		sorted_tree = AVLTreeList()
		for val in sorted:
			sorted_tree.insert(sorted_tree.length(), val)
		return sorted_tree

	"""concatenates lst to self
	@type lst: AVLTreeList
	@param lst: a list to be concatenated after self
	@rtype: int
	@returns: the absolute value of the difference between the height of the AVL trees joined
	@complexity: O(height diff)
	"""
	def concat(self, lst):
		# Get tree heights
		t1_height = -1 if self.empty() else self.root.getHeight()
		t2_height = -1 if lst.empty() else lst.root.getHeight()
		# Empty cases
		if self.empty() and lst.empty():
			return 0
		if self.empty():
			self.root = lst.root
			self.first_node = lst.first_node
			self.last_node = lst.last_node
		# there is another case where other.empty(), in which we do nothing to self
		elif not lst.empty():
			t1_original_length = self.length()
			# we take x as a "dummy" node, which will be deleted later
			x = AVLNode("")
			if t1_height <= t2_height:
				# find b in T2 - the first node on the left spine with height <= t1_height
				b = lst.root
				while b.getHeight() > t1_height:
					b = b.getLeft()

				# save b's parent as c before updating b's parent to x
				c = b.getParent()

				# connect x to t1's root and b we just found,
				x.setLeft(self.root)
				self.root.setParent(x)
				x.setRight(b)
				b.setParent(x)

				# connect x to c is it's not None (b wasn't root)
				if c is not None:
					x.setParent(c)
					c.setLeft(x)

				# update x's fields (they are still default)
				x.updateHeight()
				x.updateSize()

				# self is a shorter tree so the new root is other's root
				self.root = x if t1_height == t2_height else lst.root
			else:
				# find b in T1 - the first node on the right spine with height <= t2_height
				b = self.root
				while b.getHeight() > t2_height:
					b = b.getRight()

				# save b's parent as c before updating b's parent to x
				c = b.getParent()

				# connect x to t2's root and b we just found, and to c
				x.setRight(lst.root)
				lst.root.setParent(x)
				x.setLeft(b)
				b.setParent(x)

				# connect x to c is it's not None (b wasn't root)
				if c is not None:
					x.setParent(c)
					c.setRight(x)

				# update x's fields (they are still default)
				x.updateHeight()
				x.updateSize()

			# do re-balancing from x upwards
			self.fixTree(x, is_delete=True)
			# delete x - his index is t1's length
			self.delete(t1_original_length)
			self.last_node = lst.last_node

		return abs(t1_height - t2_height)


	"""searches for a *value* in the list
	@type val: str
	@param val: a value to be searched
	@rtype: int
	@returns: the first index that contains val, -1 if not found.
	@complexity: O(n)
	"""
	def search(self, val):
		lst = self.listToArray()
		# iterate over list until finding the value
		try:
			i = lst.index(val)
		except ValueError:
			return -1
		return i

	"""returns the root of the tree representing the list
	@rtype: AVLNode
	@returns: the root, None if the list is empty
	@complexity: O(1)
	"""
	def getRoot(self):
		return self.root


	""" Static methods """

	"""performs Select on a tree where its root is x, and the requested rank is k.
	returns the node with rank k, in the tree whose root is x
	@type x: AVLNode
	@param x: a root of some AVLTreeList
	@type k: int
	@pre: 0 <= k < x.size (the length of the tree whose root is x)
	@param k: a rank
	@rtype: AVLNode
	@returns: the node with rank k in the tree
	@complexity: O(logn)
	"""
	@staticmethod
	def selectRec(x, k):
		r = x.left.size + 1
		if k == r:
			return x
		if k < r:
			return AVLTreeList.selectRec(x.left, k)
		return AVLTreeList.selectRec(x.right, k - r)

	"""returns the successor of node x
	@type x: AVLNode
	@param x: a node
	@rtype: AVLNode
	@returns: the successor of x
	@complexity: O(logn)
	"""
	@staticmethod
	def getSuccessor(x):
		# If x has right child - go right once, then left all the way
		right = x.getRight()
		if right.isRealNode():
			return AVLTreeList.selectRec(right, 1)
		# Otherwise go up from x until the first turn right
		y = x.getParent()
		while (y is not None) and (x == y.getRight()):
			x = y
			y = x.getParent()
		return y

	"""returns the predecessor of node x
	@type x: AVLNode
	@param x: a node
	@rtype: AVLNode
	@returns: the predecessor of x
	@complexity: O(logn)
	"""
	@staticmethod
	def getPredecessor(x):
		# If x has left child - go left once, then right all the way
		left = x.getLeft()
		if left.isRealNode():
			return AVLTreeList.selectRec(left, left.getSize())
		# Otherwise go up from x until the first turn left
		y = x.getParent()
		while (y is not None) and (x == y.getLeft()):
			x = y
			y = x.getParent()
		return y


	"""perform an in-order traversal of the tree whose root is node, and add the values to lst
	@type node: AVLNode
	@param node: a node
	@type lst: list
	@param lst: will contain the values of the tree nodes
	@complexity: O(n)
	"""
	@staticmethod
	def inOrderRec(node, lst):
		if not node.isRealNode():
			return
		AVLTreeList.inOrderRec(node.getLeft(), lst)
		lst.append(node.getValue())
		AVLTreeList.inOrderRec(node.getRight(), lst)

	"""return a new AVL tree representing the sorted array
	@type arr: list
	@param arr: a sorted array
	@type l: int
	@param l: the left index
	@type r: int
	@param r: the right index
	@rtype: AVLNode
	@returns: the root of the new tree
	@complexity: O(n)
	"""
	@staticmethod
	def sortedArrayToAVL(arr, l, r):
		if l > r:
			return None
		# Create a node for the median
		mid = round((l + r) / 2)
		root = AVLNode(str(arr[mid]))
		# Left child
		left = AVLTreeList.sortedArrayToAVL(arr, l, mid - 1)
		if left is not None:
			root.setLeft(left)
			left.setParent(root)
		# Right child
		right = AVLTreeList.sortedArrayToAVL(arr, mid + 1, r)
		if right is not None:
			root.setRight(right)
			right.setParent(root)

		# we are not calling any AVLTreeList API (insert), so we need to make sure we update the fields
		root.updateHeight()
		root.updateSize()

		return root


	"""merging two sorted lists into one list
	@type lst1: list
	@param lst1: a sorted list of size n
	@type lst2: list
	@param lst2: a sorted list of size m
	@rtype: list
	@returns: the merged list
	@complexity: O(n + m)
	"""
	@staticmethod
	def merge(lst1, lst2):
		n = len(lst1)
		m = len(lst2)
		merged_lst = [0 for i in range(n + m)]

		a, b, c = 0, 0, 0
		while a < n and b < m:  # more element in both A and B
			if lst1[a] < lst2[b]:
				merged_lst[c] = lst1[a]
				a += 1
			else:
				merged_lst[c] = lst2[b]
				b += 1
			c += 1

		merged_lst[c:] = lst1[a:] + lst2[b:]
		return merged_lst

	"""
	@type lst: list
	@param lst: a list to be sorted
	@rtype: list
	@returns: the sorted list
	@complexity: O(nlogn)
	"""
	@staticmethod
	def mergeSort(lst):
		n = len(lst)
		if n <= 1:
			return lst
		return AVLTreeList.merge(AVLTreeList.mergeSort(lst[0:n//2]), AVLTreeList.mergeSort(lst[n//2:n]))