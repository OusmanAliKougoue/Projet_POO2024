#ifndef HASH_H
#define HASH_H

#include <QString>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

struct Liste {
    QString m_cle;
    QString m_marque;
    QString m_gamme;
    int m_annee;
    QString m_modele;
    QString m_photo;
    QString m_presentation;
    Liste *next;
};

class TableHachage {
private:
    Liste **tab;
    int taille_max;
    int taille;

public:
    TableHachage(int);
    ~TableHachage();
    void insertion(QString cle, QString marque, QString gamme, int annee, QString modele, QString photo, QString presentation, int i);
    void supprime(QString cle, int i);
    bool contient(QString cle, int i);
    int sizee();
    bool est_vide();
    void afficher();
    Liste* getelement(int i);
    bool sauvegardetable();
    Liste* get(QString cle, int i);
    int f1(QString cle);
    int f2(QString cle);
    int f3(QString cle);
     int calculIndiceHachage(QString ,int );
};

#endif // HASH_H
