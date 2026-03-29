package edu.asu.cse464;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Assumptions;

import java.nio.file.Files;
import java.nio.file.Path;

import static org.junit.jupiter.api.Assertions.*;

public class DotGraphToolFeature4Test {

    @Test
    public void outputDOTGraph_createsDotFile() throws Exception {
        DotGraphTool tool = new DotGraphTool();
        tool.addEdge("a", "b");

        Path out = Files.createTempFile("graph_", ".dot");
        tool.outputDOTGraph(out.toString());

        assertTrue(Files.exists(out));
        String content = Files.readString(out);
        assertTrue(content.contains("a"));
        assertTrue(content.contains("b"));
        assertTrue(content.contains("a -> b"));

        Files.deleteIfExists(out);
    }

    @Test
    public void outputGraphics_createsPngFile() throws Exception {
        // Skip test if `dot` is not available on the machine running tests
        boolean dotExists;
        try {
            Process p = new ProcessBuilder("dot", "-V").start();
            dotExists = (p.waitFor() == 0);
        } catch (Exception e) {
            dotExists = false;
        }
        Assumptions.assumeTrue(dotExists);

        DotGraphTool tool = new DotGraphTool();
        tool.addEdge("a", "b");

        Path png = Files.createTempFile("graph_", ".png");
        tool.outputGraphics(png.toString(), "png");

        assertTrue(Files.exists(png));
        assertTrue(Files.size(png) > 0);

        Files.deleteIfExists(png);
    }
}
