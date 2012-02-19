/* 
 * File:   ObjectIDHandler.h
 * Author: hbakhshi
 *
 * Created on December 18, 2008, 2:30 PM
 */

#ifndef _OBJECTIDHANDLER_H
#define	_OBJECTIDHANDLER_H

#include <map>
#include <string>
#include <stdlib.h>

using namespace std;

//this struct and the class, are for saving the ID's which are assigned to new objects to use the same ID for each combination of objects

class OBJECT_ID_INFO_COLLECTION {
public:

    static OBJECT_ID_INFO_COLLECTION * SINGLETON_OBJECT_ID_INFO;

    typedef struct {
        int firstID;
        double secondID;
        string objectName;
        short operand;
        bool isTwoOperator;
    } OBJECT_ID_INFO;

    OBJECT_ID_INFO_COLLECTION();

    ~OBJECT_ID_INFO_COLLECTION() {
    };

    int FindID(int firstID, int secondID, string objName, short operand) {
        for (std::map<int, OBJECT_ID_INFO>::const_iterator i = innerMap.begin(); i != innerMap.end(); i++) {
            if (i->second.firstID == firstID)
                if (i->second.secondID == double(secondID))
                    if (i->second.objectName == objName)
                        if (i->second.operand == operand)
                            if (i->second.isTwoOperator)
                                return i->first;
        }

        OBJECT_ID_INFO info;
        info.firstID = firstID;
        info.secondID = secondID;
        info.objectName = objName;
        info.operand = operand;
        info.isTwoOperator = true;

        int newID = GenerateNewID();
        innerMap.insert(std::make_pair<int, OBJECT_ID_INFO > (newID, info));
        return newID;
    };

    int FindID(int firstID, double number, string objName, short operand , bool numberAndOp) {
        for (std::map<int, OBJECT_ID_INFO>::const_iterator i = innerMap.begin(); i != innerMap.end(); i++) {
            if (i->second.firstID == firstID)
                if (i->second.secondID == number)
                    if (i->second.objectName == objName)
                        if (i->second.operand == operand)
                            if (!i->second.isTwoOperator)
                                return i->first;
        }

        OBJECT_ID_INFO info;
        info.firstID = firstID;
        info.secondID = number;
        info.objectName = objName;
        info.operand = operand;
        info.isTwoOperator = false;

        int newID = GenerateNewID();
        innerMap.insert(std::make_pair<int, OBJECT_ID_INFO > (newID, info));
        return newID;
    };

    int GenerateNewID() {
        int newID = 0;
        bool isNew = false;

        while (!isNew) {
            newID = 15000 + rand();
            isNew = true;

            for (std::map<int, OBJECT_ID_INFO>::const_iterator i = innerMap.begin(); i != innerMap.end(); i++) {
                if (i->first == newID) {
                    isNew = false;
                    break;
                }
            }
        }
        return newID;
    };

private:
    std::map<int, OBJECT_ID_INFO> innerMap;

};
//***************************************************************************************************************************************

#endif	/* _OBJECTIDHANDLER_H */

