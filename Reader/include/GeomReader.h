/* 
 * File:   GeomReader.h
 * Author: ash
 *
 * Created on 6 Январь 2014 г., 17:44
 */

#ifndef GEOMREADER_H
#define	GEOMREADER_H

#include "EdbBrick.h"

class GeomReader {
private:
    EdbBrickP eBrick;
public:
    GeomReader();
    GeomReader(const GeomReader& orig);
    const EdbBrickP* Brick()const {return &eBrick;}
    void SetBrick(EdbBrickP* b){eBrick=*b;}
    bool ReadGeoFile(char* geofile);
    void FindSide(EdbSegP* s);
    virtual ~GeomReader();
};

#endif	/* GEOMREADER_H */

