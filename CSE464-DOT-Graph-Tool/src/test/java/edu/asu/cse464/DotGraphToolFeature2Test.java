package edu.asu.cse464;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

public class DotGraphToolFeature2Test {

    @Test
    public void addNode_preventsDuplicates() {
        DotGraphTool tool = new DotGraphTool();

        assertTrue(tool.addNode("x"));
        assertFalse(tool.addNode("x"));

        String s = tool.toString();
        assertTrue(s.contains("Number of nodes: 1"));
        assertTrue(s.contains("Node labels: [x]"));
    }

    @Test
    public void addNodes_addsOnlyNewOnes() {
        DotGraphTool tool = new DotGraphTool();

        assertTrue(tool.addNode("x"));
        int added = tool.addNodes(new String[]{"x", "y", "y", "z"});
        assertEquals(2, added);

        String s = tool.toString();
        assertTrue(s.contains("Number of nodes: 3"));
        assertTrue(s.contains("Node labels: [x, y, z]"));
    }
}
