/* Solution DateHeure du TP4 réutilisable au TP5.
   (C) Éric Beaudry 2022.
   Vous pouvez utiliser ce fichier dans le cadre du TP5 du cours INF3105.
*/
#include "dateheure.h"
#include <cstdio>
#include <iomanip>
#include <assert.h>

DateHeure::DateHeure(int nbsecondes_)
 : nbsecondes(nbsecondes_)
{
}

DateHeure::DateHeure(int j, int h, int m, int s)
{
  nbsecondes = j;
  nbsecondes *= 24;
  nbsecondes += h;
  nbsecondes *= 60;
  nbsecondes += m;
  nbsecondes *= 60;
  nbsecondes += s;
}

bool DateHeure::operator <(const DateHeure& dh) const{
    return nbsecondes < dh.nbsecondes;
}
bool DateHeure::operator <=(const DateHeure& dh) const{
    return nbsecondes <= dh.nbsecondes;
}
bool DateHeure::operator ==(const DateHeure& dh) const{
    return nbsecondes == dh.nbsecondes;
}
bool DateHeure::operator !=(const DateHeure& dh) const{
    return nbsecondes != dh.nbsecondes;
}
bool DateHeure::operator >(const DateHeure& dh) const{
    return nbsecondes > dh.nbsecondes;
}
bool DateHeure::operator >=(const DateHeure& dh) const{
    return nbsecondes >= dh.nbsecondes;
}

const DateHeure& DateHeure::operator +=(const DateHeure& dh){
    nbsecondes += dh.nbsecondes;
    return *this;
}

DateHeure DateHeure::operator +(const DateHeure& d) const{
    return DateHeure(nbsecondes + d.nbsecondes);
}

const DateHeure& DateHeure::operator *=(int f){
    nbsecondes *= f;
    return *this;
}

DateHeure DateHeure::operator *(int f) const{
    return DateHeure(nbsecondes * f);
}

std::ostream& operator << (std::ostream& os, const DateHeure& dh){
    int jours, heures, minutes, secondes;
    int s = dh.nbsecondes;
    secondes = s % 60;
    s /= 60;
    minutes = s % 60;
    s /= 60;
    heures = s % 24;
    s /= 24;
    jours = s;
    char chaine[40];
    sprintf(chaine, "%d_%02d:%02d:%02d", jours, heures, minutes, secondes);
    os << chaine;
    return os;
}

std::istream& operator >> (std::istream& is, DateHeure& d){
    int jours=0, heures=0, minutes=0, secondes=0;
    char seperateurlu1=0, seperateurlu2=0, seperateurlu3=0;
    is >> jours >> seperateurlu1 >> heures >> seperateurlu2 >> minutes >> seperateurlu3 >> secondes;
    assert(seperateurlu1=='_');
    assert(seperateurlu2==':');
    assert(seperateurlu3==':');
    d.nbsecondes = jours;
    d.nbsecondes *= 24;
    d.nbsecondes += heures;
    d.nbsecondes *= 60;
    d.nbsecondes += minutes;
    d.nbsecondes *= 60;
    d.nbsecondes += secondes;
    return is;
}


