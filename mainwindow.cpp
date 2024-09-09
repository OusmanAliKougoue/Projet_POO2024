#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QPixmap>
#include <QStandardItemModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tableHachage(new TableHachage(600))
{
    ui->setupUi(this);
    connect(ui->boutonRechercher, &QPushButton::clicked, this, &MainWindow::on_boutonRechercher_clicked);
    connect(ui->boutonAchageBase, &QPushButton::clicked, this, &MainWindow::on_boutonAchageBase_clicked);
    connect(ui->boutonInserer_2, &QPushButton::clicked, this, &MainWindow::on_boutonInserer_2_clicked);
    connect(ui->boutonSupprimer, &QPushButton::clicked, this, &MainWindow::on_boutonSupprimer_clicked);
    connect(ui->actionCharger, &QAction::triggered, this, &MainWindow::on_actionCharger_triggered);
    connect(ui->actionTaille, &QAction::triggered, this, &MainWindow::on_actionTaille_triggered);
    connect(ui->actionQuitter, &QAction::triggered, this, &MainWindow::on_actionQuitter_triggered);
    connect(ui->actionInserer, &QAction::triggered, this, &MainWindow::on_actionInserer_triggered);
    connect(ui->actionSupprimer, &QAction::triggered, this, &MainWindow::on_actionSupprimer_triggered);
    connect(ui->actionRechercher_2, &QAction::triggered, this, &MainWindow::on_actionRechercher_2_triggered);
    connect(ui->actionAffichage, &QAction::triggered, this, &MainWindow::on_actionAffichage_triggered);
    connect(ui->actionSauvegarderBase, &QAction::triggered, this, &MainWindow::on_actionSauvegarderBase_triggered);
    connecterBaseDeDonnees();
}

MainWindow::~MainWindow() {
    delete ui;
    delete tableHachage;
}

void MainWindow::connecterBaseDeDonnees() {
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setDatabaseName("voitures");
    db.setPort(5432);
    db.setUserName("postgres");
    db.setPassword("Deribordo@62");

    if (!db.open()) {
        QMessageBox::critical(this, "Erreur de connexion", "Impossible de se connecter à la base de données !");
    } else {
        QMessageBox::information(this, "Connexion réussie", "Connexion à la base de données réussie !");
    }
}
void MainWindow::rechercherVoiture(QString cle, int hashFunc) {
    Liste* voiture = tableHachage->get(cle, hashFunc);
    if (voiture) {
        ui->valeurCleResultat_3->setText(voiture->m_cle);
        ui->valeurMarqueResultat_3->setText(voiture->m_marque);
        ui->valeurModeleResultat_3->setText(voiture->m_modele);
        ui->valeurGammeResultat_3->setText(voiture->m_gamme);
        ui->valeurAnneeResultat_3->setText(QString::number(voiture->m_annee));
        ui->valeurPhotoResultat_3->setText(voiture->m_photo);
        ui->valeurPresentationResultat_3->setText(voiture->m_presentation);

        QPixmap pixmap;
        if (pixmap.load(voiture->m_photo)) {
            ui->etiquetteImageResultat_3->setPixmap(pixmap.scaled(ui->etiquetteImageResultat_3->size(), Qt::KeepAspectRatio));
        } else {
            QMessageBox::warning(this, "Erreur de chargement d'image", "Impossible de charger l'image : " + voiture->m_photo);
            ui->etiquetteImageResultat_3->clear();
        }
    } else {
        QMessageBox::critical(this, "Recherche", "Aucune voiture trouvée avec ces informations.");
    }
}

void MainWindow::insererVoiture() {
    QString cle = ui->champCleInsertion->text();
    QString marque = ui->champMarqueInsertion->text();
    QString gamme = ui->champGammeInsertion->text();
    int annee = ui->champAnneeInsertion->text().toInt();
    QString modele = ui->champModeleInsertion->text();
    QString photo = ui->champPhotoInsertion->text();
    QString presentation = ui->champPresentationInsertion->text();
    int hashFunc = ui->champHashFuncInsertion->value();

    if (cle .isEmpty() || marque.isEmpty() || gamme.isEmpty() || modele.isEmpty() || annee == 0 || photo.isEmpty() || presentation.isEmpty()) {
        QMessageBox::warning(this, "Erreur d'insertion", "Veuillez remplir tous les champs correctement.");
        return;
    }
    else{
    tableHachage->insertion(cle, marque, gamme, annee, modele, photo, presentation,hashFunc);

    QMessageBox::information(this, "Insertion réussie", "La voiture a été insérée avec succès !");
    }
}

void MainWindow::supprimerVoiture() {
    QString cle = ui->champCleSuppression->text();
    int hashFunc = ui->champHashFuncSuppression->value();

    tableHachage->supprime(cle, hashFunc);

    QMessageBox::information(this, "Suppression réussie", "La voiture a été supprimée avec succès !");
}
void MainWindow::chargerTableHachage(TableHachage* tableHachage, int i) {
    QSqlQuery requete;
    if (!requete.exec("SELECT m_cle, m_marque, m_gamme, m_annee, m_modele, m_photo, m_presentation FROM parking")) {
        QMessageBox::critical(this, "Erreur de chargement", "La requête SQL a échoué : " + requete.lastError().text());
        return;
    }

    int count = 0;
    while (requete.next()) {
        QString m_cle = requete.value(0).toString();
        QString m_marque = requete.value(1).toString();
        QString m_gamme = requete.value(2).toString();
        int m_annee = requete.value(3).toInt();
        QString m_modele = requete.value(4).toString();
        QString m_photo = requete.value(5).toString();
        QString m_presentation = requete.value(6).toString();

        tableHachage->insertion(m_cle, m_marque, m_gamme, m_annee, m_modele, m_photo, m_presentation, i);
        count++;
    }

    if (count > 0) {
        QMessageBox::information(this, "Chargement réussi", QString("La table de hachage a été chargée correctement avec %1 enregistrements.").arg(count));
    } else {
        QMessageBox::warning(this, "Chargement terminé", "Aucun enregistrement n'a été trouvé pour charger dans la table de hachage.");
    }
}

int MainWindow::tailleBD() {
    QSqlQuery requete;
    int taille = 0;

    if (requete.exec("SELECT COUNT(*) FROM parking")) {
        if (requete.next()) {
            taille = requete.value(0).toInt();
        }
    } else {
        qDebug() << "Erreur lors de l'exécution de la requête : " << requete.lastError().text();
    }
    return taille;
}
bool MainWindow::SauvagarderBase() {
    if (!tableHachage->est_vide()) {
        QSqlQuery query;
        //pour supprimer les données de la base et le mettre à ajour avec les nouvelles données
        if (query.exec("DELETE FROM parking")) {
            qDebug() << "Suppression des données existantes réussie.";

            if (tableHachage->sauvegardetable()) {//Appel de la fonction sauvegarder pour sauvegarder la table
                QMessageBox::information(this, "Sauvegarde réussi", QString("la table a été seuvegarder avec succes dans la base."));
                return true;
            } else {
                QMessageBox::warning(this, "attention", "Sauvegarde echouée.");
                return false;
            }
        } else {
            QMessageBox::warning(this, "attention", "Erreur lors de la suppression des données existantes.");
            return false;
        }
    } else {
        QMessageBox::warning(this, "attention", "la Table est vide.");
        return false;
    }
}
void MainWindow::affichageMarque(QString marque) {
    //Cette fonction permet d'afficher les voitures de meme marques dans la table de hachage
    QStandardItemModel* modele = new QStandardItemModel();
    modele->setColumnCount(7);
    modele->setHeaderData(0, Qt::Horizontal, "Clé");
    modele->setHeaderData(1, Qt::Horizontal, "Marque");
    modele->setHeaderData(2, Qt::Horizontal, "Gamme");
    modele->setHeaderData(3, Qt::Horizontal, "Année");
    modele->setHeaderData(4, Qt::Horizontal, "Modèle");
    modele->setHeaderData(5, Qt::Horizontal, "Photo");
    modele->setHeaderData(6, Qt::Horizontal, "Présentation");

    QSqlQuery requete;
    requete.prepare("SELECT m_cle, m_marque, m_gamme, m_annee, m_modele, m_photo, m_presentation FROM parking WHERE m_marque = :marque");
    requete.bindValue(":marque", marque);
    if (requete.exec()) {
        int row = 0;
        while (requete.next()) {
            modele->setItem(row, 0, new QStandardItem(requete.value(0).toString()));
            modele->setItem(row, 1, new QStandardItem(requete.value(1).toString()));
            modele->setItem(row, 2, new QStandardItem(requete.value(2).toString()));
            modele->setItem(row, 3, new QStandardItem(requete.value(3).toString()));
            modele->setItem(row, 4, new QStandardItem(requete.value(4).toString()));
            modele->setItem(row, 5, new QStandardItem(requete.value(5).toString()));
            modele->setItem(row, 6, new QStandardItem(requete.value(6).toString()));
            row++;
        }
        ui->tableView->setModel(modele);
    } else {
        QMessageBox::warning(this, "Erreur", "La requête pour afficher les voitures a échoué : " + requete.lastError().text());
    }
}

void MainWindow::afficherDialogueInsertion() {
    ui->boiteInsertion->setVisible(true);
    ui->boiteSuppression->setVisible(false);
}

void MainWindow::afficherDialogueSuppression() {
    ui->boiteInsertion->setVisible(false);
    ui->boiteSuppression->setVisible(true);
}

void MainWindow::on_boutonRechercher_clicked() {
    QString cle = ui->champCleRechercher->text();
    int hashFunc = ui->champHashFuncRechercher->value();
    rechercherVoiture(cle, hashFunc);
}

void MainWindow::on_boutonInserer_2_clicked() {
    insererVoiture();
}

void MainWindow::on_boutonSupprimer_clicked() {
    supprimerVoiture();
    afficherDialogueSuppression();
}

void MainWindow::on_actionCharger_triggered() {
    if (tableHachage) {
        delete tableHachage;
    }

    int taille = tailleBD();
    tableHachage = new TableHachage(taille);
    chargerTableHachage(tableHachage, 1);
}
void MainWindow::on_actionCharger_2_triggered() {
    if (tableHachage) {
        delete tableHachage;
    }

    int taille = tailleBD();
    tableHachage = new TableHachage(taille);
    chargerTableHachage(tableHachage, 2);
}
void MainWindow::on_actionCharger_3_triggered() {
    if (tableHachage) {
        delete tableHachage;
    }

    int taille = tailleBD();
    tableHachage = new TableHachage(taille);
    chargerTableHachage(tableHachage, 3);
}

void MainWindow::on_actionQuitter_triggered() {
    close();
}

void MainWindow::on_actionTaille_triggered() {
    QMessageBox::information(this, "Taille calculée", QString("La taille de la base est de %1 voitures.").arg(tailleBD()));
}

void MainWindow::on_actionInserer_triggered() {
    ui->stackedWidget->setCurrentIndex(2);
    afficherDialogueInsertion();
}

void MainWindow::on_actionSupprimer_triggered() {
    ui->stackedWidget->setCurrentIndex(0);
    afficherDialogueSuppression();
}

void MainWindow::on_actionRechercher_2_triggered() {
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_actionSauvegarderBase_triggered() {
    SauvagarderBase();
}

void MainWindow::on_actionAffichage_triggered() {
     ui->stackedWidget->setCurrentIndex(4);
}

void MainWindow::on_boutonInserer_clicked()
{
    insererVoiture();
}

void MainWindow::afficherPartieDeBase(int offset, int limit) {
    QSqlQuery query;
    query.prepare("SELECT m_cle, m_marque, m_gamme, m_annee, m_modele, m_photo, m_presentation FROM parking LIMIT :limit OFFSET :offset");
    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);

    if (!query.exec()) {
        QMessageBox::warning(this, "Erreur", "La requête pour afficher les voitures a échoué : " + query.lastError().text());
        return;
    }

    QStandardItemModel* modele = new QStandardItemModel();
    modele->setColumnCount(7);
    modele->setHeaderData(0, Qt::Horizontal, "Clé  ");
    modele->setHeaderData(1, Qt::Horizontal, "Marque  ");
    modele->setHeaderData(2, Qt::Horizontal, "Gamme   ");
    modele->setHeaderData(3, Qt::Horizontal, "Année   ");
    modele->setHeaderData(4, Qt::Horizontal, "Modèle    ");
    modele->setHeaderData(5, Qt::Horizontal, "Photo    ");
    modele->setHeaderData(6, Qt::Horizontal, "Présentation     ");

    int row = 0;
    while (query.next()) {
        modele->setItem(row, 0, new QStandardItem(query.value(0).toString()));
        modele->setItem(row, 1, new QStandardItem(query.value(1).toString()));
        modele->setItem(row, 2, new QStandardItem(query.value(2).toString()));
        modele->setItem(row, 3, new QStandardItem(query.value(3).toString()));
        modele->setItem(row, 4, new QStandardItem(query.value(4).toString()));
        modele->setItem(row, 5, new QStandardItem(query.value(5).toString()));
        modele->setItem(row, 6, new QStandardItem(query.value(6).toString()));
        row++;
    }
    ui->tableView->setModel(modele);
}

void MainWindow::on_boutonAchageBase_clicked()
{

    afficherPartieDeBase(0, tailleBD());
}
void  MainWindow::on_actionmemeMarque_triggered(){
    ui->stackedWidget->setCurrentIndex(3);
}
void MainWindow::on_actionf1_2(){

}
void MainWindow::on_actionf2_2(){

}
void MainWindow::on_actionf3_2(){

}

