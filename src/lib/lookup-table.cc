
#include <travatar/translation-rule.h>
#include <travatar/lookup-table.h>
#include <travatar/dict.h>
#include <travatar/hyper-graph.h>
#include <travatar/global-debug.h>
#include <boost/foreach.hpp>

using namespace travatar;
using namespace boost;
using namespace std;

LookupTable::LookupTable() : 
    unk_rule_(CfgDataVector(), Dict::ParseSparseVector("unk=1")),
    match_all_unk_(false), save_src_str_(false) { }

LookupTable::~LookupTable() { }

// Find all the translation rules rooted at a particular node in a parse graph
vector<shared_ptr<LookupState> > LookupTable::LookupSrc(
            const HyperNode & node, 
            const vector<shared_ptr<LookupState> > & old_states) const {
    vector<shared_ptr<LookupState> > ret_states;
    BOOST_FOREACH(const shared_ptr<LookupState> & state, old_states) {
        // Match the current node
        shared_ptr<LookupState> my_state(MatchNode(node, *state));
        if(my_state != NULL) ret_states.push_back(my_state);
        // Match all rules the require descent into the next node
        shared_ptr<LookupState> start_state(MatchStart(node, *state));
        if(start_state == NULL) continue;
        // Cycle through all the edges from this node
        BOOST_FOREACH(const HyperEdge * edge, node.GetEdges()) {
            vector<shared_ptr<LookupState> > my_states(1, start_state);
            // Cycle through all the tails in this edge
            BOOST_FOREACH(const HyperNode * child, edge->GetTails())
                my_states = LookupSrc(*child, my_states);
            // Finish all the states found
            BOOST_FOREACH(const shared_ptr<LookupState> & my_state, my_states) {
                shared_ptr<LookupState> fin_state(MatchEnd(node, *my_state));
                if(fin_state != NULL) {
                    fin_state->AddFeatures(edge->GetFeatures());
                    ret_states.push_back(fin_state);
                }
            }
        }
        
    }
    return ret_states;
}

HyperGraph * LookupTable::TransformGraph(const HyperGraph & parse) const {
    // First, for each non-terminal in the input graph, make a node in the output graph
    // and lookup the rules matching each node
    HyperGraph * ret = new HyperGraph;
    ret->SetWords(parse.GetWords());
    std::map<int,int> node_map, rev_node_map;
    vector<vector<shared_ptr<LookupState> > > lookups;
    vector<shared_ptr<LookupState> > init_state;
    init_state.push_back(shared_ptr<LookupState>(GetInitialState()));
    BOOST_FOREACH(const HyperNode * node, parse.GetNodes()) {
        if(!node->IsTerminal()) {
            rev_node_map.insert(make_pair(node_map.size(), node->GetId()));
            node_map.insert(make_pair(node->GetId(), node_map.size()));
            HyperNode * next_node = new HyperNode();
            ret->AddNode(next_node);
            next_node->SetSym(node->GetSym());
            next_node->SetSpan(node->GetSpan());
            lookups.push_back(LookupSrc(*node, init_state));
        }
    }
    // For each node
    BOOST_FOREACH(HyperNode * next_node, ret->GetNodes()) {
        // If there is at least one matching node
        int my_size = lookups[next_node->GetId()].size();
        if(my_size > 0) {
            // For each matched portion of the source tree
            BOOST_FOREACH(const shared_ptr<LookupState> & state, lookups[next_node->GetId()]) {
                // For each tail in the 
                vector<HyperNode*> next_tails;
                BOOST_FOREACH(const HyperNode * tail, state->GetNonterms())
                    next_tails.push_back(ret->GetNode(node_map[tail->GetId()]));
                // For each rule
                BOOST_FOREACH(const TranslationRule * rule, *FindRules(*state)) {
                    HyperEdge * next_edge = new HyperEdge(next_node);
                    next_edge->SetTails(next_tails);
                    next_edge->SetRule(rule, state->GetFeatures());
                    if(save_src_str_) next_edge->SetSrcStr(state->GetString());
                    next_node->AddEdge(next_edge);
                    ret->AddEdge(next_edge);
                }
            }
        }
        // For unmatched nodes, add an unknown rule for every edge in the parse
        if(my_size == 0 || match_all_unk_) {
            const HyperNode * parse_node = parse.GetNode(rev_node_map[next_node->GetId()]);
            BOOST_FOREACH(const HyperEdge * parse_edge, parse_node->GetEdges()) {
                ostringstream oss;
                if(save_src_str_) oss << Dict::WSym(parse_node->GetSym()) << " (";
                HyperEdge * next_edge = new HyperEdge(next_node);
                BOOST_FOREACH(const HyperNode * parse_node, parse_edge->GetTails())
                    if(!parse_node->IsTerminal())
                        next_edge->AddTail(ret->GetNode(node_map[parse_node->GetId()]));
                next_edge->SetRule(GetUnknownRule(), parse_edge->GetFeatures());
                if(next_edge->GetTails().size() > 0) {
                    vector<int> trg(next_edge->GetTails().size());
                    for(int i = 0; i < (int)trg.size(); i++) {
                        trg[i] = -1 - i;
                        if(save_src_str_) oss << " x" << i << ':' << Dict::WSym(next_edge->GetTail(i)->GetSym());
                    }
                    next_edge->SetTrgData(CfgDataVector(GlobalVars::trg_factors, trg));
                } else {
                    pair<int,int> span = parse_node->GetSpan();
                    if(span.second - span.first != 1)
                        THROW_ERROR("Multi-terminal edges are not supported.");
                    vector<WordId> trg_words(1, parse.GetWord(span.first));
                    if(save_src_str_) oss << " \"" << Dict::WSym(parse.GetWord(span.first)) << '"';
                    next_edge->SetTrgData(CfgDataVector(GlobalVars::trg_factors, trg_words));
                }
                next_node->AddEdge(next_edge);
                if(save_src_str_) {
                    oss << " )";
                    next_edge->SetSrcStr(oss.str());
                }
                ret->AddEdge(next_edge);
            }
        }
    }
    return ret;
}

const TranslationRule * LookupTable::GetUnknownRule() const { return &unk_rule_; }
