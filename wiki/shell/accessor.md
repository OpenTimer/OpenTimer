# Accessor Command

An accessor command is an operation that does not alter the internal structure
of the timer.
Under the hook, it calls a *constant* class method.
The time complexity of an action command depends on the algorithm.

| Command | Description |
| :------ | :---------- |
| [license](#license) | shows the license information |
| [version](#version) | shows the version of the OpenTimer |
| [dump_timer](#dump_timer) | dumps the design statistics  |
| [dump_taskflow](#dump_taskflow)| dumps the current lineage graph to a DOT format file |
| [dump_graph](#dump_graph)| dumps the current timing graph to a DOT format file |

## license

The `license` command shows the license information of OpenTimer. 
Currently, OpenTimer is under MIT license and is completely free for use.

## version

The `version` command shows the version of OpenTimer in the form of *major*.*minor*.*patch*.

## dump_timer

The `dump_timer` command dumps the design statistics. 
The output shows a summary of design size, number of pins, number of nets, units, and so on.

## dump_taskflow

The `dump_taskflow` command dumps the present lineage graph to a DOT format file.
You can use tools like [GraphViz][GraphViz] to visualize the graph.

## dump_graph

The `dump_graph` command dumps the present timing graph into a DOT format file.
You can use tools like [GraphViz][GraphViz] to visualize the graph.


* * *

[GraphViz]:              https://dreampuf.github.io/GraphvizOnline/
