
#include <travatar/weights.h>
#include <travatar/hyper-graph.h>
#include <travatar/eval-measure.h>
#include <boost/foreach.hpp>

using namespace std;
using namespace travatar;

namespace travatar {

double operator*(Weights & lhs, const SparseMap & rhs) {
    double ret = 0;
    BOOST_FOREACH(const SparsePair & val, rhs) {
        ret += val.second * lhs.GetCurrent(val.first);
    }
    return ret;
}
double operator*(const Weights & lhs, const SparseMap & rhs) {
    double ret = 0;
    BOOST_FOREACH(const SparsePair & val, rhs) {
        ret += val.second * lhs.GetCurrent(val.first);
    }
    return ret;
}

// Adjust based on a single one-best list
void Weights::Adjust(const Sentence & src,
                     const std::vector<Sentence> & refs,
                     const EvalMeasure & eval,
                     const NbestList & nbest) {
    std::vector<std::pair<double,double> > scores;
    std::vector<SparseMap*> features;
    BOOST_FOREACH(const boost::shared_ptr<HyperPath> & path, nbest) {
        Sentence my_hyp = path->CalcTranslation(src);
        std::pair<double,double> score(path->GetScore(), -DBL_MAX);
        BOOST_FOREACH(const Sentence & ref, refs)
            score.second = std::max(
                            eval.CalculateStats(ref, src)->ConvertToScore(),
                            score.second);
        scores.push_back(score);
        features.push_back(new SparseMap(path->GetFeatures()));
    }
    Adjust(scores,features);
    BOOST_FOREACH(SparseMap * feat, features)
        delete feat;
}

};
