#ifndef LM_COMPOSER_BU_H__
#define LM_COMPOSER_BU_H__

#include <travatar/lm-composer.h>
#include <lm/left.hh>
#include <boost/shared_ptr.hpp>
#include <string>

namespace travatar {

class HyperNode;
class HyperGraph;

typedef std::vector<HyperNode*> ChartEntry;

// A virtual class to represent templated functions needed for handling
// various types of KenLM LMs
class LMComposerBUFunc {
public:
    static LMComposerBUFunc * CreateFromType(lm::ngram::ModelType type);
    virtual std::pair<double,int> CalcNontermScore(const LMData* data, const Sentence & syms, const std::vector<HyperNode*> & tails, const std::vector<std::vector<lm::ngram::ChartState> > & states, int lm_id, lm::ngram::ChartState & out_state) = 0;
    virtual double CalcFinalScore(const void * lm, const lm::ngram::ChartState & prev_state) = 0;
    virtual ~LMComposerBUFunc() { }
};

template <class LMType>
class LMComposerBUFuncTemplate : public LMComposerBUFunc {
    virtual std::pair<double,int> CalcNontermScore(const LMData* data, const Sentence & syms, const std::vector<HyperNode*> & tails, const std::vector<std::vector<lm::ngram::ChartState> > & states, int lm_id, lm::ngram::ChartState & out_state);
    virtual double CalcFinalScore(const void * lm, const lm::ngram::ChartState & prev_state);
    virtual ~LMComposerBUFuncTemplate() { }
};

// A bottom up language model composer that uses cube pruning to keep the
// search space small
class LMComposerBU : public LMComposer {

protected:

    // The maximum number of stack items popped during search
    int stack_pop_limit_;
    // The maximum number of elements in a single chart cell
    int chart_limit_;
    // The functions used to calculate LM scores for each model
    std::vector<LMComposerBUFunc*> funcs_;

public:
    LMComposerBU(const std::string & str) :
            LMComposer(str), stack_pop_limit_(0), chart_limit_(0) {
        for(int i = 0; i < (int)lm_data_.size(); i++)
            funcs_.push_back(LMComposerBUFunc::CreateFromType(lm_data_[i]->GetType()));
            
    }
    LMComposerBU(void * lm, lm::ngram::ModelType type, VocabMap * vocab_map) :
            LMComposer(lm, type, vocab_map), stack_pop_limit_(0), chart_limit_(0) { 
        for(int i = 0; i < (int)lm_data_.size(); i++)
            funcs_.push_back(LMComposerBUFunc::CreateFromType(lm_data_[i]->GetType()));
    }
    virtual ~LMComposerBU() { }

    // Intersect this graph with a language model, using cube pruning to control
    // the overall state space.
    virtual HyperGraph * TransformGraph(const HyperGraph & hg) const;

    int GetStackPopLimit() const { return stack_pop_limit_; }
    void SetStackPopLimit(double stack_pop_limit) { stack_pop_limit_ = stack_pop_limit; }
    int GetChartLimit() const { return chart_limit_; }
    void SetChartLimit(double chart_limit) { chart_limit_ = chart_limit; }

protected:

    // Build a chart entry for one of the nodes in the input parse
    const ChartEntry & BuildChartCubePruning(
                        const HyperGraph & parse,
                        std::vector<boost::shared_ptr<ChartEntry> > & chart, 
                        std::vector<std::vector<lm::ngram::ChartState> > & states, 
                        int id,
                        HyperGraph & graph) const;

};

}

#endif
