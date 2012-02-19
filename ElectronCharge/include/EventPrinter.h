/* 
 * File:   EventPrinter.h
 * Author: hbakhshi
 *
 * Created on January 24, 2011, 8:06 PM
 */

#ifndef EVENTPRINTER_H
#define	EVENTPRINTER_H

#include "NTupleAnalyzer/include/base/Analyzer.h"

class EventPrinter : public Analyzer<EventSelector::EVENTTYPE> {
public:

    typedef Analyzer<EventSelector::EVENTTYPE> BASE;

    EventPrinter(const edm::ParameterSet& ps) : BASE(ps){};
    virtual ~EventPrinter(){};
    
    bool Run(EventSelector::EVENTTYPE* ev){
        ev->Print();
    };
};

#endif	/* EVENTPRINTER_H */

