/* 
 * File:   GeomReader.cxx
 * Author: ash
 * 
 * Created on 6 Январь 2014 г., 17:44
 */


#include "GeomReader.h"
#include <cassert>

GeomReader::GeomReader() {
}

GeomReader::GeomReader(const GeomReader& orig) {
}

GeomReader::~GeomReader() {
}

bool GeomReader::ReadGeoFile(char* geofile){
    assert(geofile!=0);
    ///
///fill patterns volume with patterns, reading their Z position from fluka geometry file
///
    printf("Opening geometry file \'%s\'\n",geofile);
    FILE* ff = fopen(geofile,"r");
    if(ff==0)return false;
    char line[256];
    double z[4];
    double lim[4];
    int num=-1;

    while(1){
        if(fgets(line,256,ff)!=0) {
	 //printf("line=%s\n",line);
	  if(sscanf(line,"RPP emul_%d %lg %lg %lg %lg %lg %lg",&num,lim,lim+1,lim+2,lim+3,z,z+3)>0)
	    continue;
	  if(sscanf(line,"RPP base_%d %lg %lg %lg %lg %lg %lg",&num,lim,lim+1,lim+2,lim+3,z+1,z+2)>0){
		    for(int i=0;i<4;i++){
		      lim[i]*=1.e4;
		      z[i]*=1.e4;
		    }
		    EdbPlateP* plt=new EdbPlateP;
		    plt->SetID(num+1);
		    plt->SetZ((z[1]+z[2])/2.);
		    plt->SetXY  ((lim[1]+lim[0])/2.,(lim[3]+lim[2])/2.);
		    plt->SetDXDY((lim[1]-lim[0])/2.,(lim[3]-lim[2])/2.);
		    plt->SetPlateLayout(z[2]-z[1],z[1]-z[0],z[3]-z[2]);
		    plt->SetAffXY(1,0,0,1,-lim[0],-lim[2]);
		    eBrick.AddPlate(plt);		    
                    printf("Brick now has %d plates\n",eBrick.Npl());
                }
            }
        else break;
    }
    eBrick.SetDXDY((lim[1]-lim[0])/2.,(lim[3]-lim[2])/2.);
    fclose(ff);
    return true;
}

void GeomReader::FindSide(EdbSegP* s){
    ///find segment side, using this class geometry
//    s->PrintNice();
//    printf("s->DZ=%f\n",s->DZ());
//    printf("Brick has %d plates. Try to find #%d among them...\n",eBrick.Npl(),s->Plate());
    EdbPlateP* plt=eBrick.GetPlate(s->Plate());
    assert(plt);
//    plt->Print();
//    printf("plt#%d->z=%f, seg->z=%f => dz=%f\n",plt->ID(),plt->Z(),s->Z(),s->Z()-plt->Z());
    if(plt->IsInside(1,1,s->Z())==false){
        printf("plt#%d->z=%f, seg->z=%f => dz=%f\n",plt->ID(),plt->Z(),s->Z(),s->Z()-plt->Z());
        //throw s;
    }
    if(s->Z()<=plt->Z()+plt->GetLayer(0)->Zmin()){
        ///top
        s->SetSide(1); 
        ///propagate microtrack to the base surface
        s->PropagateTo(plt->Z()+plt->GetLayer(0)->Zmin());
    }else 
    if(s->Z()>=plt->Z()+plt->GetLayer(0)->Zmax()){
        ///bottom
        s->SetSide(2); 
        ///propagate microtrack to the base surface
        s->PropagateTo(plt->Z()+plt->GetLayer(0)->Zmax());
    }
}

