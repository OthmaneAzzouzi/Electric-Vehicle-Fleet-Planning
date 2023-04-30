#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <map>
#include <list>
#include <vector>
#include <cassert>
#include <cmath>
#include <sstream>
#include "reseau.h"
#include "dateheure.h"

using namespace std;

// Variables globales
double vitesse = 0;
bool verbose = true;


bool lirePlanFlotte(ifstream& is, const Reseau& res, const Requete& req, PlanFlotte& pf){
    pf.clear();
    for(unsigned int i=0;i<req.size();i++){
        PlanVehicule pv;
        
        Etape e;
        char arobase=0;
        is >> e.borne >> arobase >> e.fin >> std::ws;
        //borneid = res.getBorneIndex(e.borne);
        if(arobase!='@'){
            cout << " Erreur formatage!" << endl;
            return false;
        }
        e.arrivee = e.debut = e.fin;
        pv.push_back(e);
        while(is && is.peek()!=';'){
            char a=0, co=0, v=0, cf=0;
            is >> e.borne >> a >> e.arrivee >> co >> e.debut >> v >> e.fin >> cf >> std::ws;
            //e.borneid = res.getBorneIndex(e.borne);
            //if(e.borneid<0){
            //    cout << " Erreur: La borne '" << e.borne << "' n'existe pas!" << endl;
            //    return false;                
            //}
            if(a!='@' || co!='[' || v!=',' || cf!=']'){
                cout << " Erreur formatage!" << endl;
                return false;
            }
            pv.push_back(e);
        }
        char pvir=0;
        is >> pvir;
        if(pvir!=';')
            return false;
        pf.push_back(pv);
    }
    is >> pf.maxlue >> std::ws;
    if(pf.maxlue != pf.max()){
        cout << " Erreur: la date/heure max lue (" << pf.maxlue << ") diffère de la réelle dateheure max (" << pf.max() << ")!\n";
    }
    return true;
}

DateHeure Reseau::valider(const Requete& req, PlanFlotte pf, bool& sansconflits) const{
    if(pf.size() != req.size()){
        cout << " * Erreur: Plan vide ou taille différente!" << endl;
        return DateHeure::MAX();
    }
    struct SimVE{
        int borne;
        DateHeure dh;
        double batterie;
    };
    struct SimBorne{
        int vid = -1; // dernier à ce charger
        DateHeure fin = 0; // fin
    };
    vector<SimVE> sves(req.size());
    vector<SimBorne> sbs(bornes.size());
   
    // Initialisation des VÉs à leur position (borne) initiale.
    for(unsigned int i=0;i<req.size();i++){
        //int bdepart = getBorneIndex(req[i].sdepart);
        //int barrivee = getBorneIndex(req[i].sarrivee);
        const Etape& e = pf[i].front();
        if(req[i].sdepart != e.borne){
            cout << " * Erreur VÉ[" << i << "] : la borne de départ dans le plan n'est pas la bonne!\n";
            return DateHeure::MAX();
        }
        if(req[i].datedepart != e.fin){
            cout << " * Erreur VÉ[" << i << "] : la date/heure de départ dans le plan n'est pas la bonne!\n";
            return DateHeure::MAX();
        }
        sves[i].borne = getBorneIndex(e.borne);
        if(sves[i].borne<0){
            cout << " * La borne " << e.borne << " n'existe pas!\n";
            return DateHeure::MAX();
        }
        sves[i].dh = e.fin;
        sves[i].batterie = req[i].batterie;
        if(e.fin < req[i].datedepart){
            cout << " * Erreur VÉ[" << i << "] : l'heure ne peut pas être plus petite !\n";
            return DateHeure::MAX();
        }
        pf[i].pop_front();
    }
    
    sansconflits = true;
    // La simulation est basée sur traiter les charges en ordre chronologique de date/heure de début.
    int nbetape=0;
    while(true){
        int prochainV = -1;
        DateHeure prochaineDate = DateHeure::MAX();
        for(unsigned int i=0;i<pf.size();i++){
            if(!pf[i].empty() && pf[i].front().debut < prochaineDate){
                prochainV = i;
                prochaineDate = pf[i].front().debut;
            }
        }
        if(prochainV==-1)
            break;
               
        const Etape& etape = pf[prochainV].front();
        const Vehicule& v = req[prochainV];
        SimVE& sve = sves[prochainV];
        int bornedest = getBorneIndex(etape.borne);
        if(bornedest<0){
            cout << " * La borne " << etape.borne << " n'existe pas!\n";
            return DateHeure::MAX();
        }

        double distance = bornes[sve.borne].bornesAtteignables.at(bornedest);
        double energie = distance * v.consommation;
        
        if(verbose)
            cout << "* Étape#" << ++nbetape << ": VÉ[" << prochainV << "] branche --> " << etape.borne << " @" << etape.debut << "\n";

        
        if(energie>v.batterie){
            cout << "!!Erreur: la borne " << etape.borne << " est trop loin de " 
                 << bornes[sve.borne].sid << " (" << distance << "m) pour l'autonomie du VÉ ("
                 << v.autonomie << "m)!\n";
            return DateHeure::MAX();
        }
        if(verbose)
            cout << "  @" << sve.dh << ": départ de la borne " << bornes[sve.borne].sid << "\n";
        
        sve.dh += ceil(distance/vitesse); // durée déplacement
        if(verbose)
            cout << "  @" << sve.dh << ": arrivée à la borne " << etape.borne << "\n";
        if(sve.dh != etape.arrivee){
            cout << "!!Avertissement: la date/heure d'arrivée (" << etape.arrivee 
                  << ") n'est pas bonne! Attendue: " << sve.dh << "\n";
        }
        if(etape.debut < sve.dh){
            cout << "!! Erreur: la date/heure du début de la charge est avant l'arrivée du VÉ!\n";
            return DateHeure::MAX();
        }
        if(etape.debut > sve.dh){
            if(verbose)
                cout << "  Attente: " << (etape.debut.getEnSecondes()-sve.dh.getEnSecondes()) << "s\n";
            sve.dh = etape.debut;
        }
        if(verbose)
            cout << "  @" << sve.dh << ": début charge\n";
        SimBorne& sb = sbs[bornedest];
        if(etape.debut < sb.fin){
            cout << "!!Erreur: la borne est déjà occupée par VÉ[" << sb.vid << "] jusqu'à " << sb.fin << " !\n";
            sansconflits=false;
        }
        sve.dh += ceil(energie * 3600.0 / bornes[bornedest].puissance); // durée charge
        if(verbose)
            cout << "  @" << sve.dh << ": fin charge\n";
        if(sve.dh != etape.fin){
            cout << "!!Avertissement: la date/heure de fin (" << etape.fin 
                  << ") n'est pas bonne! Attendue: " << sve.dh << "\n";
        }
        sve.borne = bornedest;
        sb.fin = sve.dh;
        sb.vid = prochainV;
        
        pf[prochainV].pop_front();
    }

    if(verbose)
        cout << "* Plan terminé!\n";
    
    
    // Vérifier si tous les VÉs sont à destinations
    DateHeure date_fin=0;
    for(unsigned int i=0;i<req.size();i++){
        if(sves[i].borne != getBorneIndex(req[i].sarrivee)){
            cout << "!!Erreur: le VÉ[" << i << "] n'est pas arrivée à sa destination finale!\n";
            return DateHeure::MAX();
        }
        if(sves[i].dh > date_fin)
            date_fin = sves[i].dh;
    }
   
    return date_fin;
}

// Code propre au valideur

int main(int argc, const char** argv){
    const char* nomfichier_gbornes = nullptr;
    const char* nomfichier_requetes = nullptr;
    const char* nomfichier_resultats = nullptr;
    const char* nomfichier_solution = nullptr;
    
    {
        // Traitement des arguments reçus en ligne de commande.
        bool erreur_args = false;
        for(int a=1,i=0;a<argc;a++){
            if(argv[a][0]=='-'){
                switch(argv[a][1]){
/*                    case 'N':
                        niveau_algo = atoi(argv[++a]);
                        if(niveau_algo<0 || niveau_algo>4){
                            std::cout << "-N " << argv[a] << " invalide! Doit être 1, 2, 3 ou 4\n";
                            erreur_args=true;   
                        }
                        break;*/
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
                    case 2:
                        nomfichier_resultats = argv[a];
                        break;
                    case 3:
                        nomfichier_solution = argv[a];
                        break;
                    default:
                        std::cout << "Paramètre " << argv[a] << " en trop!\n";
                        erreur_args = true;
                }
            }
        }
        if(erreur_args || nomfichier_resultats==nullptr){
            cout << "Usage: ./valideur gbornes.txt requetes.txt resultats.txt [solution.txt]\n";
            return 1;
        }
        if(vitesse==0){
            vitesse = 72.0 / 3.6; // vitesse par défaut : 72km/h  /3.6 = 20m/s;
        }
    }
    
    Reseau reseau;
    {
        std::ifstream isbornes(nomfichier_gbornes);
        if(isbornes.fail()){
            std::cout << "Erreur d'ouverture du fichier graphe de bornes '" << nomfichier_gbornes << "' !" << std::endl;
            return 2;
        }
        isbornes >> reseau;
    }
    
    vector<Requete> requetes;
    {
        std::ifstream isrequets(nomfichier_requetes);
        while(!isrequets.eof()){
            Requete requete;
            isrequets >> requete >> std::ws;
            requetes.push_back(requete);
        }
        cout << requetes.size() << " requêtes lues." << endl;
    }
    
    // lire les résultats et solution si disponible
    std::ifstream isresultats(nomfichier_resultats);
    if(isresultats.fail())
        cout << "Erreur ouverture/lecture du fichier " << nomfichier_resultats << '\n';
    std::ifstream issolutions;
    if(nomfichier_solution!=nullptr){
        issolutions.open(nomfichier_solution);
        if(issolutions.fail())
            cout << "Erreur ouverture/lecture du fichier " << nomfichier_solution << '\n';
    }
    int noligne=1;
    int nbResultats=0,
        nbValidesL=0, 
        nbValidesSC=0, 
        nbCoutABS=0; // Coût Aussi Bon Solution
    for(unsigned int i=0;i<requetes.size();i++){
        cout << "*** Requête #" << (i+1) << " [ligne #" << noligne << "] ***************\n";
        const Requete& req = requetes[i];
        noligne += req.size() + 1;
        PlanFlotte pr, ps;
        if(isresultats && !isresultats.eof()){
            bool ok = lirePlanFlotte(isresultats, reseau, req, pr);
            if(ok) nbResultats++;
        }else
            cout << "(fichier résultat tronqué)" << endl;
        if(nomfichier_solution!=nullptr && issolutions && !issolutions.eof()){
            lirePlanFlotte(issolutions, reseau, req, ps);
        }else
            cout << "(fichier solution tronqué)" << endl;
        
        bool sansconflits=false;
        DateHeure coutcalcule = reseau.valider(req, pr, sansconflits);
        //reseau.valider(req, ps, sansconflits); // commenté pour éviter les affichages : on se fit au résultat.
        bool valideL = coutcalcule < DateHeure::MAX();
        cout << ">> Niveau(x) atteint(s): ";
        if(valideL){
            nbValidesL++;
            cout << "Valide(N1) ";
        }
        if(valideL && sansconflits){
            nbValidesSC++;
            cout << "SansConflits(N2) ";            
        }
        if(nomfichier_solution!=nullptr && valideL && sansconflits && coutcalcule <= ps.maxlue){
            nbCoutABS++;
            cout << "CoutAussiBon(N3) ";
        }
        cout << "\n";
    }
    
    cout << "#####################################################\n";
    cout << "Compilation:\n";
    cout << "N1\tN2\tN3\t/\tNbRés\t/\tNbReq\n";
    cout << nbValidesL << "\t" << nbValidesSC << "\t" << nbCoutABS << "\t/\t" << nbResultats << "\t/\t" << requetes.size() << "\n";

    return 0;
}

