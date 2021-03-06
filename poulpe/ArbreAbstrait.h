#ifndef ARBREABSTRAIT_H
#define ARBREABSTRAIT_H

// Contient toutes les déclarations de classes nécessaires
//  pour représenter l'arbre abstrait

#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

#include "Symbole.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////
class Noeud {
// Classe abstraite dont dériveront toutes les classes servant à représenter l'arbre abstrait
// Remarque : la classe ne contient aucun constructeur
  public:
    virtual int  executer() =0 ; // Méthode pure (non implémentée) qui rend la classe abstraite
    virtual void ajoute(Noeud* instruction) { throw OperationInterditeException(); }
    virtual ~Noeud();// Présence d'un destructeur virtuel conseillée dans les classes abstraites
    virtual void toPython(ostream &out, unsigned int indentation) = 0; //traduit l'instruction en python et l'ecrit dans out.
};

////////////////////////////////////////////////////////////////////////////////
class NoeudSeqInst : public Noeud {
// Classe pour représenter un noeud "sequence d'instruction"
//  qui a autant de fils que d'instructions dans la séquence
  public:
     NoeudSeqInst();   // Construit une séquence d'instruction vide
    ~NoeudSeqInst() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();    // Exécute chaque instruction de la séquence
    void ajoute(Noeud* instruction);  // Ajoute une instruction à la séquence
    void toPython(ostream &out, unsigned int indentation); //traduit l'instruction en python et l'ecrit dans out.


  private:
    vector<Noeud *> m_instructions; // pour stocker les instructions de la séquence
};

////////////////////////////////////////////////////////////////////////////////
class NoeudAffectation : public Noeud {
// Classe pour représenter un noeud "affectation"
//  composé de 2 fils : la variable et l'expression qu'on lui affecte
  public:
     NoeudAffectation(Noeud* variable, Noeud* expression); // construit une affectation
    ~NoeudAffectation() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();        // Exécute (évalue) l'expression et affecte sa valeur à la variable
    void toPython(ostream &out, unsigned int indentation);

  private:
    Noeud* m_variable;
    Noeud* m_expression;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudOperateurBinaire : public Noeud {
// Classe pour représenter un noeud "opération binaire" composé d'un opérateur
//  et de 2 fils : l'opérande gauche et l'opérande droit
  public:
    NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit);
    // Construit une opération binaire : operandeGauche operateur OperandeDroit
   ~NoeudOperateurBinaire() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();            // Exécute (évalue) l'opération binaire)
    void toPython(ostream &out, unsigned int indentation);

  private:
    Symbole m_operateur;
    Noeud*  m_operandeGauche;
    Noeud*  m_operandeDroit;
};

////////////////////////////////////////////////////////////////////////////////
//class NoeudInstSi : public Noeud {
//// Classe pour représenter un noeud "instruction si"
////  et ses 2 fils : la condition du si et la séquence d'instruction associée
//  public:
//    NoeudInstSi(Noeud* condition, Noeud* sequence);
//     // Construit une "instruction si" avec sa condition et sa séquence d'instruction
//   ~NoeudInstSi() {} // A cause du destructeur virtuel de la classe Noeud
//    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence
//
//  private:
//    Noeud*  m_condition;
//    Noeud*  m_sequence;
//};

class NoeudInstSiRiche : public Noeud {
// Classe pour représenter un noeud "instruction si" riche
//  et ses multiple fils : les condition des si et les séquences d'instructions associées
  public:
    NoeudInstSiRiche(std::vector<Noeud*> conditions, std::vector<Noeud*> sequences);
     // Construit une "instruction si" riche
   ~NoeudInstSiRiche() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction si riche.
    void toPython(ostream &out, unsigned int indentation); //traduit l'instruction en python et l'ecrit dans out.


  private:
      std::vector<Noeud*> m_conditions;
      std::vector<Noeud*> m_sequences;
};

class NoeudInstPour : public Noeud {
// Classe pour représenter un noeud "instruction pour"
  public:
    NoeudInstPour(Noeud * startAff, Noeud * condition, Noeud * endAff, Noeud * seqInst);
   ~NoeudInstPour() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction
    void toPython(ostream& out, unsigned int indentation);

  private:
      Noeud * m_startAff;
      Noeud * m_condition;
      Noeud * m_endAff;
      Noeud * m_seqInst;
};

class NoeudInstLire : public Noeud {
// Classe pour représenter un noeud "instruction lire"
  public:
    NoeudInstLire(std::vector<Noeud*> lectures);
   ~NoeudInstLire() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction
    void toPython(ostream& out, unsigned int indentation);


  private:
      std::vector<Noeud*> m_lectures;
};

//class NoeudInstSiRiche : public Noeud{
//    public:
//
//};

class NoeudInstTantQue : public Noeud {
// Classe pour représenter un noeud "instruction tant que"
//  et ses 2 fils : la condition du while et la séquence d'instruction associée
public:
    NoeudInstTantQue(Noeud* condition, Noeud* sequence);
    ~NoeudInstTantQue() {}
    int executer();
    void toPython(ostream& out, unsigned int indentation);

private:
    Noeud* m_condition;
    Noeud* m_sequence;
};

class NoeudInstRepeter : public Noeud {
public:
    NoeudInstRepeter(Noeud* condition, Noeud* sequence);
    ~NoeudInstRepeter() {}
    int executer();
    void toPython(ostream &out, unsigned int indentation);


private:
    Noeud* m_condition;
    Noeud* m_sequence;
};

class NoeudInstEcrire : public Noeud {
public:
    NoeudInstEcrire(std::vector<Noeud*> sequences);
    ~NoeudInstEcrire() {}
    int executer();
    void toPython(ostream &out, unsigned int indentation);

private:
    std::vector<Noeud*> m_sequences;
};

#endif /* ARBREABSTRAIT_H */
