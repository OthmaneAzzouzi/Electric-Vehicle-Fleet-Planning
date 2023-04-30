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
#include <queue>
#include <list>
#include "reseau.h"

#include <cmath>
using namespace std;

// Retourne l'index (dans le tableau bornes) du nom de la borne (ID string).
int Reseau::getBorneIndex(const std::string& sid) const{
    auto i = indexbornes.find(sid);
    if(i==indexbornes.end())
        return -1;
    return i->second;
}

PlanFlotte Reseau::calculerN1(const Requete& r) const{
    PlanFlotte plan;
    for(const Vehicule& v : r)
        plan.push_back(calculer(v));
    return plan;
}

PlanVehicule Reseau::calculer(const Vehicule& v) const{
    //À compléter: c'est ici, dans calculer, que vous devez implémenter l'algorithme de Dijkstra.
    int id_borne_depart = indexbornes.at(v.sdepart);
    int id_borne_arrivee = indexbornes.at(v.sarrivee);

    int d; // la duree pour se rendre a la borne suivante plus la duree de charge
    int tempsDeplacement; // temps pour se rendre a la borne suivante
    int tempsDeChargement; // temps pour se rendre a la borne suivante
    double BatterieUtilisee;
    map<int , int> durees;
    map<int , int> parents;
    /* structure des elements dans le monceau Q contient l'id de borne et la distance pour y rendre */
    struct BornePrioritaire {
        int id;
        int duree;
        BornePrioritaire(int _id, int _duree): id(_id),duree(_duree) {}
        bool operator <(const BornePrioritaire borne) const {
            return duree < borne.duree;
        }
    };

    for (int i = 0;  i <= bornes.size(); ++i) {
       if (bornes[id_borne_depart].bornesAtteignables.count(bornes[i].id) > 0) {
            durees[bornes[i].id] = std::numeric_limits<int>::max();
            parents[bornes[i].id] = -1;
       }
    }
    // distances[s] ← 0
    durees[id_borne_depart] = 0;
    BornePrioritaire borneDepart = BornePrioritaire(id_borne_depart,0 );
    // Q ← créer FilePrioritaire(V )
    priority_queue<BornePrioritaire, vector<BornePrioritaire> >  Q;
    Q.push(borneDepart);

    while( !Q.empty() ){
         BornePrioritaire V = Q.top();
         Q.pop();
        if ( durees[V.id] == std::numeric_limits<int>::max() ) {
             break;
         }
        for( int i = bornes[V.id].bornesAtteignables.begin()->first; i <= bornes[V.id].bornesAtteignables.size(); ++ i) {
            // les aretes qui dépasse l'autonomie de vehicule
            if ( bornes[V.id].bornesAtteignables.count(i) <= 0 || bornes[V.id].bornesAtteignables.find(i)->second > v.autonomie) {
                continue;
            }
            tempsDeplacement = std::ceil(bornes[V.id].bornesAtteignables.find(i)->second/vitesse); // correcte
            BatterieUtilisee = (bornes[V.id].bornesAtteignables.find(i)->second) * (v.consommation);
            tempsDeChargement = std::ceil((BatterieUtilisee*3600)/bornes[bornes[V.id].bornesAtteignables.find(i)->first].puissance);//correcte
            d = durees[V.id] + tempsDeplacement + tempsDeChargement;

            if (d < durees[bornes[V.id].bornesAtteignables.find(i)->first]) {
                parents[bornes[V.id].bornesAtteignables.find(i)->first] = bornes[V.id].id;
                 durees[bornes[V.id].bornesAtteignables.find(i)->first] = d;
                 BornePrioritaire w = BornePrioritaire(bornes[V.id].bornesAtteignables.find(i)->first, (-1) * d);
                 Q.push(w);
             }
         }
     }
    // trouver le chemin minimum entre le point de depart et lien de depart
    int courant;
    int parent;
    //-> point d'arrivé
    std::map<int,int>::iterator it_parent=parents.find(id_borne_arrivee);
    courant = it_parent->first;

    vector<int> chemin;
    chemin.push_back(id_borne_depart);
    while(courant != id_borne_depart) {
        parent = it_parent->second;
        courant = parent;
        it_parent = parents.find(courant);
        if (parent != id_borne_depart) {
            chemin.push_back(bornes[parent].id);
        }
    }
    chemin.push_back(id_borne_arrivee);

    PlanVehicule pv;
    std::list<Etape>::iterator etape = pv.begin();
    if(id_borne_depart==id_borne_arrivee) { // depart
        Etape e = Etape(v.sdepart,id_borne_depart,v.datedepart,v.datedepart,v.datedepart);
        pv.push_back(e);

    } else {
        for (int i: chemin) {
            if (i == id_borne_depart ) {
                Etape e = Etape(bornes[i].sid, i, v.datedepart, v.datedepart, v.datedepart);
                pv.push_back(e);
            } else {
            Etape etap_precedente = pv.back(); // extraire les informations sur l'étape précédente
            int id_precedent = etap_precedente.id_borne; // borne précédente
            DateHeure fin_precedent = etap_precedente.fin; // fin du chargement et départ de la borne précédente

            tempsDeplacement = std::ceil(bornes[id_precedent].bornesAtteignables.find(i)->second / vitesse);
            BatterieUtilisee = (bornes[id_precedent].bornesAtteignables.find(i)->second) * (v.consommation);
            tempsDeChargement = std::ceil((BatterieUtilisee*3600) /
                    bornes[bornes[id_precedent].bornesAtteignables.find(i)->first].puissance);
            DateHeure arrivee_courant = fin_precedent + tempsDeplacement;
            DateHeure fin_courant = arrivee_courant + tempsDeChargement;

            Etape e = Etape(bornes[i].sid, i, arrivee_courant, arrivee_courant ,fin_courant);
            pv.push_back(e);
            }
        }
    }
    return pv;
}
PlanFlotte Reseau::calculerN2(const Requete& r) const{
    PlanFlotte plan;
    for(const Vehicule& v : r){
        PlanVehicule pv = calculer(v);
        plan.push_back(pv);
        //À compléter: réserver les bornes pour pv
    }
    //À compléter: il faut annuler les réservations
    return plan;
}

PlanFlotte Reseau::calculerN3(const Requete& r) const{
    std::cerr << "Niveau 3 pas encore implémenté. Utilisation de Niveau 2 («fallback»)." << std::endl;
    return calculerN2(r);
}

DateHeure PlanFlotte::max() const{
    DateHeure max;
    for(PlanVehicule pv : *this)
        if(!pv.empty() && pv.back().fin > max)
            max = pv.back().fin;
    return max;
}

