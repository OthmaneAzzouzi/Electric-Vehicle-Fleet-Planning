/*
 * TP5 - Été 2022 - Planification d'une flotte de véhicules électriques
 * UQAM / Département d'informatique
 * INF3105 - Structures de données et algorithmes
 * @author Besma Jabri
 *      Code permanent : JABB71560008
 * @author Othmane Azzouzi
 *      Code permanent : AZZO27129703
 */

#if !defined(_RESEAU_H_)
#define _RESEAU_H_

#include <string>
#include <list>
#include <vector>
#include <map>
#include "dateheure.h"

extern double vitesse; // variable globale

class Borne{
  public:
    const std::string getSID() const { return sid; }

  private:
    int id;           // identifiant entier séquentiel
    std::string sid;  // identifiant string pour l'usager
    int puissance;    // en W
    long osmid;       // noeud OpenStreetMap le plus près (inutilisé pour TP5)
    std::map<int, double> bornesAtteignables; // arêtes sortantes : id borne --> distance en m
  friend std::istream& operator>>(std::istream&, Borne&);
  friend class Reseau;
};

class Vehicule{
    std::string sdepart,  // id string de la borne de départ
                sarrivee; // et d'arrivée
    DateHeure datedepart;
    double batterie;      // capacité en Wh
    double consommation;  // unité Wh/m
    double autonomie=0;   // unité m (calculé = batterie/consommation)

  friend std::istream& operator>>(std::istream&, Vehicule&);
  friend class Reseau;
};

// typedef permet de spécifier un nouveau type à partir d'un autre.
typedef std::vector<Vehicule> Requete;

std::istream& operator>>(std::istream&, Requete&);

// La structure Etape représente une étape dans un plan.
// Il s'agit d'un ID string d'une borne et des temps correspondants.
struct Etape{
    std::string borne = "?";
    int id_borne = -1;
    DateHeure arrivee, debut, fin;
    Etape() {}
    Etape( std::string _borne, int _id_borne, DateHeure _arrivee, DateHeure _debut, DateHeure _fin) :
            borne(_borne), id_borne(_id_borne), arrivee (_arrivee), debut(_debut), fin(_fin) {}
    friend std::ostream& operator<<(std::ostream&, const Etape&);
};

typedef std::list<Etape> PlanVehicule;

// PlanFlotte hérite de std::vector<PlanVehicule>
struct PlanFlotte : public std::vector<PlanVehicule> {
    DateHeure max() const;
    DateHeure maxlue; // variable pour le valideur (valideur.cpp).
};

std::ostream& operator<<(std::ostream&, const PlanVehicule&);
std::ostream& operator<<(std::ostream&, const PlanFlotte&);


class Reseau{
  public:
    // Les 3 fonctions suivantes sont les points d'entrées.
    // La fonction main du TP5 appelle l'une d'elles.
    PlanFlotte calculerN1(const Requete& r) const;
    PlanFlotte calculerN2(const Requete& r) const;
    PlanFlotte calculerN3(const Requete& r) const;

    // pour valideur.cpp    
    int getBorneIndex(const std::string& sid) const;
    DateHeure valider(const Requete&, PlanFlotte, bool& sansconflits) const;

  private:
    std::vector<Borne> bornes;
    std::map<std::string, int> indexbornes; // ID string --> ID séquentiel 0, 1, 2,...

    PlanVehicule calculer(const Vehicule& v) const;

  friend std::istream& operator>>(std::istream&, Reseau&);    
};

#endif

