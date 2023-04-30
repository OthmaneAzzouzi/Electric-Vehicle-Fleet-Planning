/* Solution DateHeure du TP4.
   (C) Éric Beaudry 2022.
   Vous pouvez utiliser ce fichier dans le cadre du TP5 du cours INF3105.
*/
#if !defined(__DATEHEURE_H__)
#define __DATEHEURE_H__

#include <iostream>
#include <limits>

class DateHeure{
  public:
    DateHeure(int nbsecondes_=0);
    DateHeure(int j, int h, int m=0, int s=0);
    
    bool operator <(const DateHeure& dh) const;
    bool operator <=(const DateHeure& dh) const;
    bool operator !=(const DateHeure& dh) const;
    bool operator ==(const DateHeure& dh) const;
    bool operator >(const DateHeure& dh) const;
    bool operator >=(const DateHeure& dh) const;

    const DateHeure& operator +=(const DateHeure& dh);
    DateHeure operator +(const DateHeure& dh) const;
    
    const DateHeure& operator -=(const DateHeure& dh);
    DateHeure operator -(const DateHeure& dh) const;
    
    const DateHeure& operator *=(int facteur);
    DateHeure operator *(int facteur) const;
    
    int getEnSecondes() const {return nbsecondes;}
    
    static DateHeure MAX() { return DateHeure(std::numeric_limits<int>::max());}
    
  private:
    int nbsecondes;
    
  friend std::ostream& operator << (std::ostream&, const DateHeure& dh);
  friend std::istream& operator >> (std::istream&, DateHeure& dh);
};


#endif
