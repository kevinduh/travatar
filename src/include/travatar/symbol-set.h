#ifndef SYMBOL_SET_H__
#define SYMBOL_SET_H__

#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>
#include <tr1/unordered_map>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <sstream>

namespace travatar {

template < class T >
class SymbolSet {

public:

    typedef std::tr1::unordered_map<std::string,T> Map;
    typedef std::vector< std::string* > Vocab;
    typedef std::vector< T > Ids;

protected:
    
    Map map_;
    Vocab vocab_;
    Ids reuse_;
    boost::shared_mutex mutex_;

public:
    SymbolSet() : map_(), vocab_(), reuse_(), mutex_() { }
    SymbolSet(const SymbolSet & ss) : 
                map_(ss.map_), vocab_(ss.vocab_), reuse_(ss.reuse_), mutex_() {
        for(typename Vocab::iterator it = vocab_.begin(); 
                                                it != vocab_.end(); it++) 
            if(*it)
                *it = new std::string(**it);
    }
    ~SymbolSet() {
        for(typename Vocab::iterator it = vocab_.begin(); 
                                            it != vocab_.end(); it++)
            if(*it)
                delete *it;
    }

    const std::vector<std::string*> & GetSymbols() const { return vocab_; }
    const std::string & GetSymbol(T id) const {
        return *vocab_[id];
        // return *SafeAccess(vocab_, id);
    }
    T GetId(const std::string & sym, bool add = false) {
        // boost::upgrade_lock< boost::shared_mutex > lock(mutex_);
        typename Map::const_iterator it = map_.find(sym);
        if(it != map_.end())
            return it->second;
        else if(add) {
            // boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
            T id;
            if(reuse_.size() != 0) {
                id = reuse_.back(); reuse_.pop_back();
                vocab_[id] = new std::string(sym);
            } else {
                id = vocab_.size();
                vocab_.push_back(new std::string(sym));
            }
            map_.insert(std::make_pair(sym,id));
            return id;
        }
        return -1;
    }
    T GetId(const std::string & sym) const {
        return const_cast< SymbolSet<T>* >(this)->GetId(sym,false);
    }
    size_t size() const { return vocab_.size() - reuse_.size(); }
    size_t capacity() const { return vocab_.size(); }
    size_t hashCapacity() const { return map_.size(); }
    void removeId(const T id) {
        boost::unique_lock< boost::shared_mutex > lock(mutex_);
        map_.erase(*vocab_[id]);
        delete vocab_[id];
        vocab_[id] = 0;
        reuse_.push_back(id);
    }
    
    void ToStream(std::ostream & out) {
        boost::unique_lock< boost::shared_mutex >  lock(mutex_);
        out << vocab_.size() << std::endl;
        for(int i = 0; i < (int)vocab_.size(); i++)
            out << *vocab_[i] << std::endl;
        out << std::endl;
    }
    static SymbolSet<T>* FromStream(std::istream & in) {
        std::string line;
        int size;
        SymbolSet<T> * ret = new SymbolSet<T>;
        getline(in, line); 
        std::istringstream iss(line);
        for(iss >> size; size > 0 && getline(in, line); size--)
            ret->GetId(line, true);
        getline(in,line);
        if(line != "")
            throw std::runtime_error("Expected empty line while reading SymbolSet, but got non-empty line");
        return ret;
    }

    Map & GetMap() { return map_; }

};

}

#endif
