/*
 * TP5 - Été 2022 - Planification d'une flotte de véhicules électriques
 * UQAM / Département d'informatique
 * INF3105 - Structures de données et algorithmes
 * @author Besma Jabri
 *      Code permanent : JABB71560008
 * @author Othmane Azzouzi
 *      Code permanent : AZZO27129703
 */
#include <cassert>
#include <iostream>
#include "reseau.h"
using namespace std;

istream& operator>>(istream& is, Borne& b){
    char c=0;
    double puissance=0;
    is >> b.id 
       >> b.sid
       >> puissance >> c;
    //cout << "b.sid=" << b.sid << endl;
    if(c=='k'){
        puissance*=1000;
        is >> c;
    }
    assert(c=='W');
    b.puissance = (int) puissance;
    is >> c;
    assert(c=='n');
    is >> b.osmid;
    is >> c;
    assert(c==':');
    while(!is.eof() && is.peek()!=';'){
        int autreborne=-1;
        is >> autreborne;
        is >> c;
        assert(c=='=');
        double d;
        is >> d;
        is >> c >> std::ws;
        assert(c=='m');
        b.bornesAtteignables[autreborne] = d;
    }
    is >> c >> std::ws;
    assert(c==';');
    return is;
}

istream& operator>>(istream& is, Reseau& r){
    r.bornes.clear();
    while(!is.eof()){
        if(is.peek()=='#'){
            string ligne;
            getline(is, ligne);
            continue;
        }
        Borne b;
        is >> b >> std::ws;
        //cout << "b.bornesAtteignables.size()=" << b.bornesAtteignables.size() << endl;
        int& id = r.indexbornes[b.getSID()];
        assert(id==0); // s'assurer que le ID n'existe pas
        id = r.bornes.size();
        r.bornes.push_back(b);
    }
    return is;
}

std::istream& operator>>(std::istream& is, Vehicule& v){
    std::string unitecapbat, uniteconsom;
    char pv=0;
    is >> v.datedepart >> v.sdepart >> v.sarrivee 
       >> v.batterie >> unitecapbat
       >> v.consommation >> uniteconsom >> pv;
    assert(pv==';');
    if(unitecapbat=="kWh")
        v.batterie *= 1000;     // convertir en Wh
    else
        assert(unitecapbat=="Wh");
    if(uniteconsom=="Wh/km")
        v.consommation /= 1000; // convertir en Wh/m
    else
        assert(uniteconsom=="Wh/m");
    v.autonomie = v.batterie / v.consommation; // en mètres (m)
    is >> std::ws;
    return is;
}

std::istream& operator>>(std::istream& is, Requete& r){
    r.clear();
    while(!is.eof()){
        if(is.peek()=='-')
            break;
        Vehicule v;
        is >> v >> std::ws;
        r.push_back(v);
    }
    char t=0;
    is >> t;
    return is;
}

std::ostream& operator<<(std::ostream& os, const Etape& e){
    os << e.borne << " @" << e.arrivee << " [" << e.debut << ',' << e.fin << "] ";
    return os;
}

std::ostream& operator<<(std::ostream& os, const PlanVehicule& pv){
    auto iter=pv.begin();
    os << iter->borne << " @" << iter->arrivee << ' ';
    for(++iter;iter!=pv.end();++iter)
        os << *iter << ' ';
    os << ';';
    return os;
}

std::ostream& operator<<(std::ostream& os, const PlanFlotte& pf){
    for(unsigned int i=0;i<pf.size();i++){
        os << pf[i] << '\n';
    }
    os << pf.max() << endl;
    return os;
}

