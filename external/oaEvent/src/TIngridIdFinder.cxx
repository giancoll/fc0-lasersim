#include <cstdlib>
#include <vector>
#include <string>
#ifdef DEBUG_IngridFinder
#include <iostream>
#endif

#include "ND280GeomId.hxx"
#include "ND280GeomIdDef.hxx"
#include "TIngridIdFinder.hxx"

ND::TIngridIdFinder::TIngridIdFinder()  {;}
ND::TIngridIdFinder::~TIngridIdFinder() {;}


bool ND::TIngridIdFinder::Search(const std::vector<std::string>& names, ND::TGeometryId& id){
    if(names.back().find("scinti")==std::string::npos) return false;

#ifdef DEBUG_IngridFinder
    for (std::vector<std::string>::const_iterator s = names.begin();
         s != names.end();
         ++s) std::cout << " " << *s;
    std::cout << std::endl;
#endif

    UInt_t proj=-1, objType=-1, scintiNum=-1;

    if(names.back().find("vscinti")!=std::string::npos)
        objType = ND::GeomId::Def::INGRID::kIngridVeto;
    else
        objType = ND::GeomId::Def::INGRID::kIngridModule;

    if     (names.back().find("scinti_h")!=std::string::npos){
        proj = ND::GeomId::Def::INGRID::kHorizontal;
    }
    else if(names.back().find("scinti_v")!=std::string::npos){
        proj = ND::GeomId::Def::INGRID::kVertical;
    }
    else
        return false;
    scintiNum = atoi(names.back().substr(names.back().find("_",strlen("scinti_."))+1).c_str());

    if(objType == ND::GeomId::Def::INGRID::kIngridVeto){
        if((names.end()-3)->find("ingrid")==std::string::npos) return false;
        if(proj == ND::GeomId::Def::INGRID::kHorizontal){
            if((names.end()-2)->find("veto_h")==std::string::npos) return false;
            UInt_t objNum=atoi((names.end()-2)->substr(strlen("veto_h"),(names.end()-2)->find("_",strlen("veto_h"))-strlen("veto_h")).c_str());
            id = ND::GeomId::INGRID::HorzVetoScintillator(objNum,scintiNum);
        }
        else{
            if((names.end()-2)->find("veto_v")==std::string::npos) return false;
            UInt_t objNum=atoi((names.end()-2)->substr(strlen("veto_v"),(names.end()-2)->find("_",strlen("veto_v"))-strlen("veto_v")).c_str());
            id = ND::GeomId::INGRID::VertVetoScintillator(objNum,scintiNum);
        }
    }
    else if((names.end()-4)->find("ingrid")==std::string::npos) return false;
    else if((names.end()-3)->find("standard")!=std::string::npos){
        if((names.end()-2)->find("tracker")==std::string::npos) return false;
        UInt_t trkNum=atoi((names.end()-2)->substr((names.end()-2)->find("_")+1).c_str());
        UInt_t objNum=atoi((names.end()-3)->substr(strlen("standard"),(names.end()-3)->find('_',strlen("standard"))).c_str());
        id = ND::GeomId::INGRID::ModScintillator(objNum,trkNum,proj,scintiNum);
    }
    else if((names.end()-3)->find("proton")!=std::string::npos){
        if((names.end()-2)->find("ptracker")==std::string::npos) return false;
        UInt_t trkNum=atoi((names.end()-2)->substr((names.end()-2)->find("_")+1).c_str());
        UInt_t objNum=16;
        id = ND::GeomId::INGRID::ModScintillator(objNum,trkNum,proj,scintiNum);
    }
    else return false;
    return true;
}
