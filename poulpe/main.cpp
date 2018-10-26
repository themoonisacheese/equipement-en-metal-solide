#include <iostream>
using namespace std;
#include "Interpreteur.h"
#include "Exceptions.h"

int main(int argc, char* argv[]) {
  string nomFich;
  if (argc < 2) {
    cout << "Usage : " << argv[0] << " nom_fichier_source" << endl << endl;
    cout << "Entrez le nom du fichier que voulez-vous interpréter : ";
    getline(cin, nomFich);
  } else
    nomFich = argv[1];
  bool convert = false;
  if (argc == 3 && strcmp(argv[2],"-p") ==0) {
    convert = true;
  }
  ifstream fichier(nomFich.c_str());
  try {
    Interpreteur interpreteur(fichier);
    interpreteur.analyse();
    if(interpreteur.isValid()){
            // Si pas d'exception levée, l'analyse syntaxique a réussi
        cout << endl << "================ Syntaxe Correcte" << endl;

        if (convert) {
            cout << endl <<  "================ Convertissage ..." << endl;
            ofstream fpython((nomFich.substr(0, nomFich.length() -3) + "py").c_str());
            if (interpreteur.getArbre()!=nullptr) interpreteur.getArbre()->toPython(fpython, 0);
            cout << endl <<  "================ Fini" << endl;

        } else {
            // On affiche le contenu de la table des symboles avant d'exécuter le programme
            cout << endl << "================ Table des symboles avant exécution : " << interpreteur.getTable();
            cout << endl << "================ Execution de l'arbre" << endl;
            // On exécute le programme si l'arbre n'est pas vide
            if (interpreteur.getArbre()!=nullptr) interpreteur.getArbre()->executer();
            // Et on vérifie qu'il a fonctionné en regardant comment il a modifié la table des symboles
            cout << endl << "================ Table des symboles apres exécution : " << interpreteur.getTable();
        }
    }
  } catch (InterpreteurException & e) {
    cout << e.what() << endl;
  }
  return 0;
}
