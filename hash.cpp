#include "hash.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
int TableHachage::f1(QString cle) {
    unsigned int hash = 0;
    for (int i = 0; i < cle.length(); ++i) {
        hash += static_cast<unsigned char>(cle[i].unicode());
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash % taille_max;
}

int TableHachage::f2(QString cle) {
    unsigned int hash = 0;
    int c;
    for (int i = 0; i < cle.length(); ++i) {
        c = static_cast<unsigned char>(cle[i].unicode());
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash % taille_max;
}

int TableHachage::f3(QString cle) {
    int p = 0;
    for (int i = 0; i < cle.length(); i++)
        p += static_cast<int>(cle[i].unicode()) * (i + 1);
    p = (p * 13) % taille_max;
    return p;
}
int TableHachage::calculIndiceHachage(QString cle,int i)
{
    //le parametre i pour choisir la fonction de hachage parmi les trois
    int k;
    switch (i) {
    case 1: k = f1(cle);break;
    case 2: k = f2(cle);break;
    case 3: k = f3(cle); break;
    default: k = -1;
    }
    return k;
}

TableHachage::TableHachage(int N) {
    taille = 0;
    taille_max = N;
    tab = new Liste*[taille_max];
    for (int i = 0; i < taille_max; ++i) {
        tab[i] = nullptr;
    }
}

TableHachage::~TableHachage() {
    for (int i = 0; i < taille_max; i++) {
        Liste* nouv = tab[i];
        while (nouv != nullptr) {
            Liste* tmp = nouv->next;
            delete nouv;
            nouv = tmp;
        }
    }
    delete[] tab;
}
void TableHachage::insertion(QString cle, QString marque, QString gamme, int annee, QString modele, QString photo, QString presentation, int i) {
    //le parametre i pour choisir la fonction de hachage parmi les trois
    if (taille == taille_max) {
        qDebug() << "La table est déjà pleine.";
        return;
    }
    Liste* nouvelElement = new Liste;
    nouvelElement->m_cle = cle;
    nouvelElement->m_marque = marque;
    nouvelElement->m_gamme = gamme;
    nouvelElement->m_annee = annee;
    nouvelElement->m_modele = modele;
    nouvelElement->m_photo = photo;
    nouvelElement->m_presentation = presentation;
    nouvelElement->next = nullptr;

    int index=calculIndiceHachage(cle,i);

    if (index == -1) {
        qDebug() << "Échec du calcul de l'index de hachage.";
        delete nouvelElement;
        return;
    }
    Liste* courant = tab[index];
    while (courant != nullptr) {
        if (courant->m_cle == cle) {
            qDebug() << "Une voiture avec cette clé existe déjà. Insertion refusée.";
            delete nouvelElement;
            return;
        }
        courant = courant->next;
    }
    if (tab[index] == nullptr) {
        tab[index] = nouvelElement;
    } else {
        Liste* p = tab[index];
        while (p->next != nullptr) {
            p = p->next;
        }
        p->next = nouvelElement;
    }

    taille++;
    qDebug() << "Voiture insérée avec succès dans la table de hachage.";
}


Liste* TableHachage::get(QString cle, int i) {
    //le parametre i pour choisir la fonction de hachage parmi les trois
    int k=calculIndiceHachage(cle,i);
    if (k != -1) {
        Liste* p = tab[k];
        while (p != nullptr && p->m_cle != cle) {
            p = p->next;
        }

        return p;
    }
    else
         return nullptr;
}

void TableHachage::supprime(QString cle, int i) {
    //le parametre i pour choisir la fonction de hachage parmi les trois
    int k=calculIndiceHachage(cle,i);
    if (k != -1 && tab[k] != nullptr) {
        Liste* p = tab[k];
        Liste* q = nullptr;
        while (p != nullptr && p->m_cle != cle) {
            q = p;
            p = p->next;
        }
        if (p != nullptr) {
            if (q != nullptr) {
                q->next = p->next;
            } else {
                tab[k] = p->next;
            }
            delete p;
            taille--;
        }
    }
}

bool TableHachage::contient(QString cle, int i) {
    //le parametre i pour choisir la fonction de hachage parmi les trois
    return get(cle, i) != nullptr;
}

int TableHachage::sizee() {
    return taille;
}

bool TableHachage::est_vide() {
    return taille == 0;
}

void TableHachage::afficher() {
    for (int i = 0; i < taille_max; i++) {
        Liste* p = tab[i];
        while (p != nullptr) {
            qDebug() << "Cle:" << p->m_cle;
            qDebug() << "Marque:" << p->m_marque;
            qDebug() << "Gamme:" << p->m_gamme;
            qDebug() << "Annee:" << p->m_annee;
            qDebug() << "Modele:" << p->m_modele;
            qDebug() << "Photo:" << p->m_photo;
            qDebug() << "Presentation:" << p->m_presentation;
            p = p->next;
        }
    }
}

bool TableHachage::sauvegardetable() {
    if (tab != nullptr) {
        QSqlQuery requete;
        for (int i = 0; i < taille_max; ++i) {
            Liste *p = tab[i];
            while (p != nullptr) {
                requete.prepare("INSERT INTO parking(m_cle, m_marque, m_gamme, m_annee, m_modele, m_photo, m_presentation) VALUES (?, ?, ?, ?, ?, ?, ?)");
                requete.bindValue(0, p->m_cle);
                requete.bindValue(1, p->m_marque);
                requete.bindValue(2, p->m_gamme);
                requete.bindValue(3, p->m_annee);
                requete.bindValue(4, p->m_modele);
                requete.bindValue(5, p->m_photo);
                requete.bindValue(6, p->m_presentation);
                if (!requete.exec()) {
                    qDebug() << "Erreur lors de l'insertion dans la base de données:" << requete.lastError().text();
                    return false;
                }
                p = p->next;
            }
        }
        qDebug() << "Base de données sauvegardée avec succès.";
        return true;
    }
    return false;
}
Liste* TableHachage::getelement(int hashIndex) {
    if (hashIndex >= 0 && hashIndex < taille_max) {
        return tab[hashIndex];
    }
    return nullptr;
}
