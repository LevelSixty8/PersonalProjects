package edu.asu.cse464;

import org.jgrapht.Graph;
import org.jgrapht.graph.DefaultDirectedGraph;
import org.jgrapht.graph.DefaultEdge;
import org.jgrapht.nio.dot.DOTImporter;
import org.jgrapht.nio.dot.DOTExporter;

import java.io.Writer;
import java.nio.file.Files;
import java.util.function.Function;
import java.io.IOException;
import java.io.Reader;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class DotGraphTool {

    private final Graph<String, DefaultEdge> graph;

    public DotGraphTool() {
        this.graph = new DefaultDirectedGraph<>(DefaultEdge.class);
    }

    // Feature 1: Parse a DOT graph file to create a graph
    public void parseGraph(String filepath) throws IOException {
        DOTImporter<String, DefaultEdge> importer = new DOTImporter<>();
        importer.setVertexFactory(id -> id); // use node id as label

        try (Reader r = Files.newBufferedReader(Path.of(filepath), StandardCharsets.UTF_8)) {
            importer.importGraph(graph, r);
        }
    }

    // Feature 1: Print graph details
    @Override
    public String toString() {
        List<String> nodes = new ArrayList<>(graph.vertexSet());
        Collections.sort(nodes);

        List<String> edges = new ArrayList<>();
        for (DefaultEdge e : graph.edgeSet()) {
            String src = graph.getEdgeSource(e);
            String dst = graph.getEdgeTarget(e);
            edges.add(src + " -> " + dst);
        }
        Collections.sort(edges);

        StringBuilder sb = new StringBuilder();
        sb.append("Number of nodes: ").append(nodes.size()).append("\n");
        sb.append("Node labels: ").append(nodes).append("\n");
        sb.append("Number of edges: ").append(edges.size()).append("\n");
        sb.append("Edges:\n");
        for (String line : edges) {
            sb.append(line).append("\n");
        }
        return sb.toString();
    }

    // Feature 1: Output the text representation to a file
    public void outputGraph(String filepath) throws IOException {
        Files.writeString(Path.of(filepath), this.toString(), StandardCharsets.UTF_8);
    }

    // Feature 2: Add a single node (no duplicates)
public boolean addNode(String label) {
    if (label == null || label.isBlank()) {
        throw new IllegalArgumentException("Node label cannot be null/blank.");
    }
    if (graph.containsVertex(label)) {
        return false;
    }
    graph.addVertex(label);
    return true;
}

// Feature 2: Add multiple nodes (no duplicates). Returns how many were added.
public int addNodes(String[] labels) {
    if (labels == null) {
        throw new IllegalArgumentException("labels cannot be null.");
    }
    int added = 0;
    for (String s : labels) {
        if (s == null || s.isBlank()) {
            continue;
        }
        if (addNode(s)) {
            added++;
        }
    }
    return added;
}

// Feature 3: Add a single edge (no duplicates)
public boolean addEdge(String srcLabel, String dstLabel) {
    if (srcLabel == null || srcLabel.isBlank() || dstLabel == null || dstLabel.isBlank()) {
        throw new IllegalArgumentException("Edge labels cannot be null/blank.");
    }

    // Ensure endpoints exist (safe + avoids surprises)
    if (!graph.containsVertex(srcLabel)) {
        graph.addVertex(srcLabel);
    }
    if (!graph.containsVertex(dstLabel)) {
        graph.addVertex(dstLabel);
    }

    // Duplicate-edge check
    if (graph.containsEdge(srcLabel, dstLabel)) {
        return false;
    }

    graph.addEdge(srcLabel, dstLabel);
    return true;
}

// Feature 4: Output DOT graph to a file
public void outputDOTGraph(String path) throws IOException {
    DOTExporter<String, DefaultEdge> exporter = new DOTExporter<>();
    exporter.setVertexIdProvider(Function.identity());

    try (Writer w = Files.newBufferedWriter(Path.of(path), StandardCharsets.UTF_8)) {
        exporter.exportGraph(graph, w);
    }
}

// Feature 4: Output graphics using Graphviz (png required)
public void outputGraphics(String path, String format) throws IOException, InterruptedException {
    if (format == null || !format.equalsIgnoreCase("png")) {
        throw new IllegalArgumentException("Only png format is supported.");
    }

    // Write DOT to a temporary file then call `dot`
    Path tempDot = Files.createTempFile("graph_", ".dot");
    outputDOTGraph(tempDot.toString());

    ProcessBuilder pb = new ProcessBuilder(
            "dot",
            "-Tpng",
            tempDot.toString(),
            "-o",
            path
    );
    pb.redirectErrorStream(true);

    Process p = pb.start();
    int exit = p.waitFor();

    Files.deleteIfExists(tempDot);

    if (exit != 0) {
        throw new IOException("Graphviz dot failed with exit code " + exit);
    }
}
}
