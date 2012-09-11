#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <travatar/dict.h>
#include <travatar/util.h>
#include <travatar/tree-io.h>
#include <travatar/travatar-runner.h>
#include <travatar/lookup-table-hash.h>
#include <travatar/lookup-table-marisa.h>
#include <travatar/lm-composer-bu.h>
#include <travatar/binarizer-directional.h>
#include <lm/model.hh>

using namespace travatar;
using namespace std;
using namespace boost;
using namespace lm::ngram;

// Run the model
void TravatarRunner::Run(const ConfigTravatarRunner & config) {
    // Load the rule table
    ifstream tm_in(config.GetString("tm_file").c_str());
    cerr << "Reading TM file from "<<config.GetString("tm_file")<<"..." << endl;
    if(!tm_in)
        THROW_ERROR("Could not find TM: " << config.GetString("tm_file"));
    // Load the translation model
    shared_ptr<LookupTable> tm;
    if(config.GetString("tm_storage") == "hash")
        tm.reset(LookupTableHash::ReadFromRuleTable(tm_in));
    else if(config.GetString("tm_storage") == "marisa")
        tm.reset(LookupTableMarisa::ReadFromRuleTable(tm_in));
    tm_in.close();
    // Create the binarizer
    shared_ptr<GraphTransformer> binarizer;
    if(config.GetString("binarize") == "left") {
        binarizer.reset(new BinarizerDirectional(BinarizerDirectional::BINARIZE_LEFT));
    } else if(config.GetString("binarize") == "right") {
        binarizer.reset(new BinarizerDirectional(BinarizerDirectional::BINARIZE_RIGHT));
    } else if(config.GetString("binarize") != "none") {
        THROW_ERROR("Invalid binarizer type " << config.GetString("binarizer"));
    }
    // Load the weight file
    ifstream weight_in(config.GetString("weight_file").c_str());
    cerr << "Reading weight file from "<<config.GetString("weight_file")<<"..." << endl;
    if(!weight_in)
        THROW_ERROR("Could not find weights: " << config.GetString("weight_file"));
    SparseMap weights = Dict::ParseFeatures(weight_in);
    weight_in.close();
    // Load the language model
    shared_ptr<LMComposerBU> lm;
    if(config.GetString("lm_file") != "") {
        LMComposerBU * bu = new LMComposerBU(new Model(config.GetString("lm_file").c_str()));
        bu->SetLMWeight(weights[Dict::WID("lm")]);
        bu->SetStackPopLimit(config.GetInt("pop_limit"));
        lm.reset(bu);
    }
    // Open the n-best output stream if it exists
    int nbest_count = config.GetInt("nbest");
    scoped_ptr<ostream> nbest_out;
    if(config.GetString("nbest_out") != "") {
        nbest_out.reset(new ofstream(config.GetString("nbest_out").c_str()));
        if(!*nbest_out)
            THROW_ERROR("Could not open nbest output file: " << config.GetString("nbest_out"));
    } else {
        nbest_count = 1;
    }
    // Open the n-best output stream if it exists
    scoped_ptr<ostream> trace_out;
    if(config.GetString("trace_out") != "") {
        trace_out.reset(new ofstream(config.GetString("trace_out").c_str()));
        if(!*trace_out)
            THROW_ERROR("Could not open trace output file: " << config.GetString("trace_out"));
    }
    // Process one at a time
    int sent = 0;
    string line;
    PennTreeIO penn;
    cerr << "Translating..." << endl;
    while(getline(std::cin, line)) {
        istringstream in(line);
        shared_ptr<HyperGraph> tree_graph(penn.ReadTree(in));
        if(tree_graph.get() == NULL) break;
        // Binarizer if necessary
        if(binarizer.get() != NULL) {
            shared_ptr<HyperGraph> bin_graph(binarizer->TransformGraph(*tree_graph));
            tree_graph.swap(bin_graph);
        }
        shared_ptr<HyperGraph> rule_graph(tm->TransformGraph(*tree_graph));
        rule_graph->ScoreEdges(weights);
        rule_graph->ResetViterbiScores();
        // If we have an lm, score with the LM
        // { /* DEBUG */ JSONTreeIO io; io.WriteTree(*rule_graph, cerr); cerr << endl; }
        if(lm.get() != NULL) {
            shared_ptr<HyperGraph> lm_graph(lm->TransformGraph(*rule_graph));
            lm_graph.swap(rule_graph);
        }
        // { /* DEBUG */ JSONTreeIO io; io.WriteTree(*rule_graph, cerr); cerr << endl; }
        vector<shared_ptr<HyperPath> > nbest_list = rule_graph->GetNbest(nbest_count);
        cout << Dict::PrintWords(nbest_list[0]->CalcTranslation(tree_graph->GetWords())) << endl;
        if(nbest_out.get() != NULL) {
            BOOST_FOREACH(const shared_ptr<HyperPath> & path, nbest_list) {
                *nbest_out
                    << sent
                    << " ||| " << Dict::PrintWords(path->CalcTranslation(tree_graph->GetWords()))
                    << " ||| " << path->GetScore()
                    << " ||| " << Dict::PrintFeatures(path->CalcFeatures()) << endl;
            }
        }
        if(trace_out.get() != NULL) {
            BOOST_FOREACH(const HyperEdge * edge, nbest_list[0]->GetEdges()) {
                *trace_out
                    << sent
                    << " ||| " << edge->GetHead()->GetSpan()
                    << " ||| " << edge->GetRuleStr() 
                    << " ||| " << Dict::PrintAnnotatedWords(edge->GetTrgWords())
                    << " ||| " << Dict::PrintFeatures(edge->GetFeatures())
                    << endl;
            }
        }
        sent++;
        cerr << (sent%100==0?'!':'.'); cerr.flush();
    }
    cerr << endl << "Done translating..." << endl;
}
