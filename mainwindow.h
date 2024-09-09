#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include "hash.h"
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void rechercherVoiture(QString cle, int hashFunc);
    void insererVoiture();
    void supprimerVoiture();
    void chargerTableHachage(TableHachage* tableHachage, int hashFunc);
    void afficherDialogueInsertion();
    void afficherDialogueSuppression();
    void connecterBaseDeDonnees();
    int tailleBD();
    bool SauvagarderBase();
    void affichageMarque(QString marque);
    void afficherPartieDeBase(int offset, int limit);


    void on_boutonRechercher_clicked();
    void on_boutonInserer_2_clicked();
    void on_boutonSupprimer_clicked();
    void on_actionCharger_triggered();
    void on_actionTaille_triggered();
    void on_actionQuitter_triggered();
    void on_actionInserer_triggered();
    void on_actionSupprimer_triggered();
    void on_actionSauvegarderBase_triggered();
    void on_actionRechercher_2_triggered();
    void on_actionAffichage_triggered();

    void on_actionmemeMarque_triggered();
    void on_boutonInserer_clicked();
    void on_actionf1_2();
    void on_actionf2_2();
    void on_actionf3_2();
    void on_boutonAchageBase_clicked();
    void on_actionCharger_2_triggered();
    void on_actionCharger_3_triggered();




private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QStandardItemModel *modele;
    TableHachage *tableHachage;
};

#endif // MAINWINDOW_H
