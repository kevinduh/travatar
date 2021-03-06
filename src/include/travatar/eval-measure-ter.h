#ifndef EVAL_MEASURE_TER_H__
#define EVAL_MEASURE_TER_H__

#include <travatar/sentence.h>
#include <travatar/eval-measure.h>
#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>

namespace travatar {

class EvalStatsTer : public EvalStatsAverage {
public:
    EvalStatsTer(double val, double denom = 1.0, bool reverse = false)
        : EvalStatsAverage(val,denom), reverse_(reverse) { }
    virtual std::string GetIdString() const { return "TER"; }
    virtual double ConvertToScore() const {
        double score = vals_[1] ? vals_[0]/vals_[1] : 0;
        return reverse_ ? 1-score : score;
    }
    EvalStatsPtr Clone() const { return EvalStatsPtr(new EvalStatsTer(vals_[0], vals_[1], reverse_)); }
protected:
    bool reverse_;
};

class EvalMeasureTer : public EvalMeasure {

public:

    EvalMeasureTer(bool reverse = false) : reverse_ (reverse) { }
    EvalMeasureTer(const std::string & str);

    // Calculate the stats for a single sentence
    virtual boost::shared_ptr<EvalStats> CalculateStats(
                const Sentence & ref,
                const Sentence & sys) const;

    // Calculate the stats for a single sentence
    virtual EvalStatsPtr ReadStats(
                const std::string & file);

protected:
    
    // TER is better when it is lower, so for tuning we want to be able to
    // subtract TER from 1 to get a value that is better when it is higher
    bool reverse_;

};

}

#endif
