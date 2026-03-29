package edu.asu.cse464;

public class AppMain {
    public static void main(String[] args) throws Exception {
        if (args.length < 1) {
            System.out.println("Usage: AppMain <input.dot> [feature2|feature3|feature4]");
            return;
        }

        DotGraphTool tool = new DotGraphTool();
        tool.parseGraph(args[0]);

        // Optional demo mode for Feature 2 / Feature 3
        if (args.length >= 2) {
            if (args[1].equalsIgnoreCase("feature2")) {
                System.out.println("Feature 2 demo:");
                System.out.println("addNode(a): " + tool.addNode("a"));
                System.out.println("addNode(x): " + tool.addNode("x"));
                System.out.println("addNode(x): " + tool.addNode("x"));

                int addedCount = tool.addNodes(new String[]{"y", "y", "z", "a"});
                System.out.println("addNodes(y,y,z,a) added: " + addedCount);
                System.out.println();

            } else if (args[1].equalsIgnoreCase("feature3")) {
                System.out.println("Feature 3 demo:");
                System.out.println("addEdge(a,b): " + tool.addEdge("a", "b"));
                System.out.println("addEdge(b,a): " + tool.addEdge("b", "a"));
                System.out.println("addEdge(x,y): " + tool.addEdge("x", "y"));
                System.out.println("addEdge(x,y): " + tool.addEdge("x", "y"));
                System.out.println();

            } else if (args[1].equalsIgnoreCase("feature4")) {
		System.out.println("Feature 4 demo:");
                tool.outputDOTGraph("out.dot");
                tool.outputGraphics("out.png", "png");
                System.out.println("Wrote out.dot and out.png");
                System.out.println();
            }
        }

        System.out.println(tool.toString());
        tool.outputGraph("graph_info.txt");
        System.out.println("Wrote graph_info.txt");
    }
}
