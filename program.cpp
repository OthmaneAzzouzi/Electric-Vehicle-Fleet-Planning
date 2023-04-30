/*
 * TP5 - Été 2022 - Planification d'une flotte de véhicules électriques
 * UQAM / Département d'informatique
 * INF3105 - Structures de données et algorithmes
 * @author Besma Jabri
 *      Code permanent : JABB71560008
 * @author Othmane Azzouzi
 *      Code permanent : AZZO27129703
 */

#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include "reseau.h"

void tp5(const Reseau&, std::istream&);
double vitesse = 0;
int niveau_algo=3;

int main(int argc, const char** argv){
    const char* nomfichier_gbornes=nullptr;
    const char* nomfichier_requetes=nullptr;
    
    {
        // Traitement des arguments reçus en ligne de commande.
        bool erreur_args = false;
        for(int a=1,i=0;a<argc;a++){
            if(argv[a][0]=='-'){
                switch(argv[a][1]){
                    case 'N':
                        niveau_algo = atoi(argv[++a]);
                        if(niveau_algo<0 || niveau_algo>4){
                            std::cout << "-N " << argv[a] << " invalide! Doit être 1, 2, 3 ou 4\n";
                            erreur_args=true;   
                        }
                        break;
                    case 'V':
                    {
                        std::stringstream ss(argv[++a]);
                        std::string unite;
                        ss >> vitesse >> unite;
                        if(unite=="km/h")
                            vitesse /= 3.6;
                        else if(unite.empty() || unite=="m/s") { }
                        else{
                            std::cout << "Unité " << unite << " invalide!\n";
                            erreur_args=true;
                        }
                        break;
                    }
                    default:
                        std::cout << "Option " << argv[a] << " invalide!\n";
                        erreur_args = true;
                }
            }else{
                switch(i++){
                    case 0:
                        nomfichier_gbornes = argv[a];
                        if(vitesse==0 && std::strstr(nomfichier_gbornes, "uqam-")!=nullptr){
                            //std::cout << "Fixer la vitesse: 1m/s!\n";
                            vitesse = 1.0; // 1 m/s
                        }
                        break;
                    case 1:
                        nomfichier_requetes = argv[a];
                        break;
                    default:
                        std::cout << "Paramètre " << argv[a] << " en trop!\n";
                        erreur_args = true;
                }
            }
        }
        if(erreur_args || nomfichier_gbornes==nullptr){
            std::cout << "Usage tp5:\n "
                << "./tp5 [-V 1{m/s|km/h}] gbornes.txt [requetes.txt]" << std::endl;
            return 1;
        }
        if(vitesse==0){
            vitesse = 72.0 / 3.6; // vitesse par défaut : 72km/h  /3.6 = 20m/s;
        }
    }
       
    std::ifstream isbornes(nomfichier_gbornes);
    if(isbornes.fail()){
        std::cout << "Erreur lecture fichier graphe de bornes '" << nomfichier_gbornes << "' !" << std::endl;
        return 2;
    }
    Reseau reseau;
    isbornes >> reseau;
    
    if(nomfichier_requetes!=nullptr){
        std::ifstream isrequetes(nomfichier_requetes);
        if(isrequetes.fail()){
            std::cout << "Erreur lecture fichier requêtes '" << nomfichier_requetes << "' !" << std::endl;
            return 3;
        }
        tp5(reseau, isrequetes);
    }else
        tp5(reseau, std::cin);
    return 0;
}

void tp5(const Reseau& reseau, std::istream& entree){
    while(!entree.eof()){
        Requete requete;
        entree >> requete;
        PlanFlotte pf;
        if(niveau_algo==1)
            pf = reseau.calculerN1(requete);
        else if(niveau_algo==2)
            pf = reseau.calculerN2(requete);
        else if(niveau_algo==3)
            pf = reseau.calculerN3(requete);
        else{
            std::cout << "-N " << niveau_algo << " pas encore implémenté!" << std::endl;
        }
        std::cout << pf;
        entree >> std::ws;
    }
}
