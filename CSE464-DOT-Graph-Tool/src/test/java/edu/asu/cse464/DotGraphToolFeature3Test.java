package edu.asu.cse464;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

public class DotGraphToolFeature3Test {

    @Test
    public void addEdge_preventsDuplicateEdges() {
        DotGraphTool tool = new DotGraphTool();

        assertTrue(tool.addEdge("a", "b"));
        assertFalse(tool.addEdge("a", "b")); // duplicate

        String s = tool.toString();
        assertTrue(s.contains("Number of edges: 1"));
        assertTrue(s.contains("a -> b"));
    }

    @Test
    public void addEdge_allowsDifferentDirections() {
        DotGraphTool tool = new DotGraphTool();

        assertTrue(tool.addEdge("a", "b"));
        assertTrue(tool.addEdge("b", "a")); // reverse direction is a different edge

        String s = tool.toString();
        assertTrue(s.contains("Number of edges: 2"));
        assertTrue(s.contains("a -> b"));
        assertTrue(s.contains("b -> a"));
    }
}
