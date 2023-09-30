""" Our kmeans implementation from HW1, for the analysis """
#!/usr/bin/env python3
import math

class Cluster:
    def __init__(self, centroid=None):
        if centroid is not None:
            self.points = [centroid]
        else:
            self.points = []
        self.centroid: DataPoint = centroid

    def __repr__(self):
        return f"Cluster(centroid: {self.centroid}, points={len(self.points)})"
    
    def __str__(self):
        return str(self.centroid)
        
    def add_point(self, point) -> None:
        self.points.append(point)

    def remove_point(self, point) -> None:
        self.points.remove(point) # Note that the coordinates are unique

    # This is only used once there is at least one point in the cluster
    def update_centroid(self) -> float: 
        d = self.centroid.d
        n = len(self.points)
    
        # find the new centroid
        new_coords = [0 for i in range(d)]
        for point in self.points:
            for i in range(d):
                new_coords[i] += point.coords[i]
        new_coords = [x/n for x in new_coords]
        new_centroid = DataPoint(new_coords, -1)

        delta = self.centroid.distance(new_centroid)
        self.centroid = new_centroid
        return delta
    

class DataPoint:
    def __init__(self, coords, initial_index):
        self.coords = coords
        self.d = len(coords)
        # the original index of the data point in the initial input,
        # used to produce the labels needed for the silhouette score in analysis.py
        self.initial_index = initial_index
        self.cluster = None
        

    def __repr__(self):
        return f"DataPoint(Coords: {self.coords})"
    
    def __str__(self):
        format_coords = [f"{coord:.4f}" for coord in self.coords]
        return ",".join(format_coords)
        
    def distance(self, other) -> float:
        temp_sum = 0
        for i in range(self.d):
            temp_sum += (self.coords[i] - other.coords[i]) ** 2
        return math.sqrt(temp_sum)

    # Adds point to closest cluster
    def assign_to_closest(self, clusters) -> None:
        min_dist = float('inf')
        target_cluster = None

        for cluster in clusters:
            distance = self.distance(cluster.centroid)
            if distance < min_dist:
                min_dist = distance
                target_cluster = cluster

        # add current point to cluster and remove from previous
        target_cluster.add_point(self)
        if self.cluster is not None:
            self.cluster.remove_point(self)
        self.cluster = target_cluster # update the cluster which the point belongs to
