Summary
=======

The k-shortest transit paths algorithm contains two parts. 
The first part computes the least transfer times matrix, 
which provides a _heuristic_ value to the second part, 
the k-shortest transit paths searching.

An revised _Bellman-Ford_ algorithm is implemented to search 
the first k shortest paths from one source to all the other 
destinations. The algorithm is implemented and tested with 
a real world transit network including more than _500 lines 
and 4000 stops_. 

[Here](https://github.com/wlxiong/k_shortest_bus_routes/wiki/Dijkstra's-algorithm-and-priority-queue) 
is a note on Dijkstra's algorithm and priority queue. Dijkstra's algorithm is not the core of my
algorithm (only used to calculate the heuristic value), but someone asked me about the implementation. 
So I post this note in the [wiki](https://github.com/wlxiong/k_shortest_bus_routes/wiki) page. 

Publication
===========

Liyuan Zhao, Yiliang Xiong, and Hong Sun.
_The K Shortest Transit Paths Choosing Algorithm in Stochastic Transit Network_. 
Lecture Notes in Computer Science, Vol. 5009 (2008), pp. 747--754, 
[Fulltext](http://www.springerlink.com/content/6r1578566781l030/). 