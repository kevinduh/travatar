#ifndef TEST_TREE_IO_H__
#define TEST_TREE_IO_H__

#include "test-base.h"
#include <travatar/tree-io.h>
#include <travatar/dict.h>
#include <boost/scoped_ptr.hpp>

namespace travatar {

class TestTreeIO : public TestBase {

public:

    TestTreeIO() {
        tree_str = " (A (B (C x) (D y))\n (E z))AAA";
        // Create the words
        HyperNode* a_node = new HyperNode(Dict::WID("A"), MakePair(0,3)); tree_exp.AddNode(a_node);
        HyperNode* b_node = new HyperNode(Dict::WID("B"), MakePair(0,2)); tree_exp.AddNode(b_node);
        HyperNode* c_node = new HyperNode(Dict::WID("C"), MakePair(0,1)); tree_exp.AddNode(c_node);
        HyperNode* x_node = new HyperNode(Dict::WID("x"), MakePair(0,1)); tree_exp.AddNode(x_node);
        HyperNode* d_node = new HyperNode(Dict::WID("D"), MakePair(1,2)); tree_exp.AddNode(d_node);
        HyperNode* y_node = new HyperNode(Dict::WID("y"), MakePair(1,2)); tree_exp.AddNode(y_node);
        HyperNode* e_node = new HyperNode(Dict::WID("E"), MakePair(2,3)); tree_exp.AddNode(e_node);
        HyperNode* z_node = new HyperNode(Dict::WID("z"), MakePair(2,3)); tree_exp.AddNode(z_node);
        tree_exp.SetWords(Dict::ParseWords("x y z"));
        // Make the edge for A
        HyperEdge * a_edge = new HyperEdge(a_node);
        a_edge->AddTail(b_node); a_edge->AddTail(e_node);
        a_node->AddEdge(a_edge); tree_exp.AddEdge(a_edge);
        // Make the edge for B
        HyperEdge * b_edge = new HyperEdge(b_node);
        b_edge->AddTail(c_node); b_edge->AddTail(d_node);
        b_node->AddEdge(b_edge); tree_exp.AddEdge(b_edge);
        // Make the edge for C
        HyperEdge * c_edge = new HyperEdge(c_node);
        c_edge->AddTail(x_node);
        c_node->AddEdge(c_edge); tree_exp.AddEdge(c_edge);
        // Make the edge for D
        HyperEdge * d_edge = new HyperEdge(d_node);
        d_edge->AddTail(y_node);
        d_node->AddEdge(d_edge); tree_exp.AddEdge(d_edge);
        // Make the edge for E
        HyperEdge * e_edge = new HyperEdge(e_node);
        e_edge->AddTail(z_node);
        e_node->AddEdge(e_edge); tree_exp.AddEdge(e_edge);

        graph_str = "{\"words\": [\"running\", \"water\"], \"nodes\": [ {\"id\": 0, \"sym\": \"ROOT\", \"span\": [0, 2], \"trg_span\": [0, 2]}, {\"id\": 1, \"sym\": \"VP\", \"span\": [0, 2]}, {\"id\": 2, \"sym\": \"NP\", \"span\": [0, 2]}, {\"id\": 3, \"sym\": \"JJ\", \"span\": [0, 1]}, {\"id\": 4, \"sym\": \"VP\", \"span\": [0, 1]}, {\"id\": 5, \"sym\": \"VPG\", \"span\": [0, 1]}, {\"id\": 6, \"sym\": \"running\", \"span\": [0, 1]}, {\"id\": 7, \"sym\": \"NP\", \"span\": [1, 2]}, {\"id\": 8, \"sym\": \"NN\", \"span\": [1, 2]}, {\"id\": 9, \"sym\": \"water\", \"span\": [1, 2]} ], \"edges\": [ {\"id\": 0, \"head\": 0, \"tails\": [1], \"score\": 1.0}, {\"id\": 1, \"head\": 0, \"tails\": [2]}, {\"id\": 2, \"head\": 1, \"tails\": [4, 7]}, {\"id\": 3, \"head\": 2, \"tails\": [3, 8]}, {\"id\": 4, \"head\": 3, \"tails\": [6]}, {\"id\": 5, \"head\": 4, \"tails\": [5]}, {\"id\": 6, \"head\": 5, \"tails\": [6]}, {\"id\": 7, \"head\": 7, \"tails\": [8]}, {\"id\": 8, \"head\": 8, \"tails\": [9]}, {\"id\": 9, \"head\": 8} ]}\nAAA";

        // Create the words
        HyperNode* node0 = new HyperNode(Dict::WID("ROOT"),    MakePair(0,2)); graph_exp.AddNode(node0);
        node0->GetTrgSpan().insert(0); node0->GetTrgSpan().insert(2);
        HyperNode* node1 = new HyperNode(Dict::WID("VP"),      MakePair(0,2)); graph_exp.AddNode(node1);
        HyperNode* node2 = new HyperNode(Dict::WID("NP"),      MakePair(0,2)); graph_exp.AddNode(node2);
        HyperNode* node3 = new HyperNode(Dict::WID("JJ"),      MakePair(0,1)); graph_exp.AddNode(node3);
        HyperNode* node4 = new HyperNode(Dict::WID("VP"),      MakePair(0,1)); graph_exp.AddNode(node4);
        HyperNode* node5 = new HyperNode(Dict::WID("VPG"),     MakePair(0,1)); graph_exp.AddNode(node5);
        HyperNode* node6 = new HyperNode(Dict::WID("running"), MakePair(0,1)); graph_exp.AddNode(node6);
        HyperNode* node7 = new HyperNode(Dict::WID("NP"),      MakePair(1,2)); graph_exp.AddNode(node7);
        HyperNode* node8 = new HyperNode(Dict::WID("NN"),      MakePair(1,2)); graph_exp.AddNode(node8);
        HyperNode* node9 = new HyperNode(Dict::WID("water"),   MakePair(1,2)); graph_exp.AddNode(node9);
        HyperEdge* edge0 = new HyperEdge(node0); edge0->AddTail(node1); node0->AddEdge(edge0); graph_exp.AddEdge(edge0); edge0->SetScore(1.0);
        HyperEdge* edge1 = new HyperEdge(node0); edge1->AddTail(node2); node0->AddEdge(edge1); graph_exp.AddEdge(edge1);
        HyperEdge* edge2 = new HyperEdge(node1); edge2->AddTail(node4); edge2->AddTail(node7); node1->AddEdge(edge2); graph_exp.AddEdge(edge2);
        HyperEdge* edge3 = new HyperEdge(node2); edge3->AddTail(node3); edge3->AddTail(node8); node2->AddEdge(edge3); graph_exp.AddEdge(edge3);
        HyperEdge* edge4 = new HyperEdge(node3); edge4->AddTail(node6); node3->AddEdge(edge4); graph_exp.AddEdge(edge4);
        HyperEdge* edge5 = new HyperEdge(node4); edge5->AddTail(node5); node4->AddEdge(edge5); graph_exp.AddEdge(edge5);
        HyperEdge* edge6 = new HyperEdge(node5); edge6->AddTail(node6); node5->AddEdge(edge6); graph_exp.AddEdge(edge6);
        HyperEdge* edge7 = new HyperEdge(node7); edge7->AddTail(node8); node7->AddEdge(edge7); graph_exp.AddEdge(edge7);
        HyperEdge* edge8 = new HyperEdge(node8); edge8->AddTail(node9); node8->AddEdge(edge8); graph_exp.AddEdge(edge8);
        HyperEdge* edge9 = new HyperEdge(node8); node8->AddEdge(edge9); graph_exp.AddEdge(edge9);
        graph_exp.SetWords(Dict::ParseWords("running water"));
 
        HyperNode* quotenode = new HyperNode(Dict::WID("\""),    MakePair(0,1)); quote_exp.AddNode(quotenode);
        quote_exp.SetWords(Dict::ParseWords("\""));
    }
    ~TestTreeIO() { }

    int TestReadPenn() {
        // Use this tree_str
        istringstream instr(tree_str);
        PennTreeIO io;
        boost::scoped_ptr<HyperGraph> hg_act(io.ReadTree(instr));
        // Check to make sure that the remaining values still remain
        string left_act; instr >> left_act;
        string left_exp = "AAA";
        // Check that both values are equal
        return tree_exp.CheckEqual(*hg_act) && left_act == left_exp;
    }

    int TestReadJSON() {
        // Use this tree
        istringstream instr(graph_str);
        JSONTreeIO io;
        boost::scoped_ptr<HyperGraph> hg_act(io.ReadTree(instr));
        // Check to make sure that the remaining values still remain
        string left_act; instr >> left_act;
        string left_exp = "AAA";
        // Check that both values are equal
        return graph_exp.CheckEqual(*hg_act) && left_act == left_exp;
    }

    int TestWriteJSON() {
        // Use this tree
        stringstream strm;
        JSONTreeIO io;
        io.WriteTree(graph_exp, strm);
        cerr << "strm: " << strm.str() << endl;
        boost::scoped_ptr<HyperGraph> hg_act(io.ReadTree(strm));
        // Check that both values are equal
        return graph_exp.CheckEqual(*hg_act);
    }

    int TestWriteJSONQuote() {
        // Use this tree
        stringstream strm;
        JSONTreeIO io;
        io.WriteTree(quote_exp, strm);
        cerr << "strm: " << strm.str() << endl;
        boost::scoped_ptr<HyperGraph> hg_act(io.ReadTree(strm));
        // Check that both values are equal
        return quote_exp.CheckEqual(*hg_act);
    }

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestReadJSON()" << endl; if(TestReadJSON()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestWriteJSON()" << endl; if(TestWriteJSON()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestWriteJSONQuote()" << endl; if(TestWriteJSONQuote()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestReadPenn()" << endl; if(TestReadPenn()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestTreeIO Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

private:

    string tree_str, graph_str;
    HyperGraph tree_exp, graph_exp, quote_exp;

};

}

#endif
