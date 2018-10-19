#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
  for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
  if (instruction!=nullptr) m_instructions.push_back(instruction);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
  int valeur = m_expression->executer(); // On exécute (évalue) l'expression
  ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
  return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
  int og, od, valeur;
  if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
  if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
  // Et on combine les deux opérandes en fonctions de l'opérateur
  if (this->m_operateur == "+") valeur = (og + od);
  else if (this->m_operateur == "-") valeur = (og - od);
  else if (this->m_operateur == "*") valeur = (og * od);
  else if (this->m_operateur == "==") valeur = (og == od);
  else if (this->m_operateur == "!=") valeur = (og != od);
  else if (this->m_operateur == "<") valeur = (og < od);
  else if (this->m_operateur == ">") valeur = (og > od);
  else if (this->m_operateur == "<=") valeur = (og <= od);
  else if (this->m_operateur == ">=") valeur = (og >= od);
  else if (this->m_operateur == "et") valeur = (og && od);
  else if (this->m_operateur == "ou") valeur = (og || od);
  else if (this->m_operateur == "non") valeur = (!og);
  else if (this->m_operateur == "/") {
    if (od == 0) throw DivParZeroException();
    valeur = og / od;
  }
  return valeur; // On retourne la valeur calculée
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstSi::executer() {
  if (m_condition->executer()) m_sequence->executer();
  return 0; // La valeur renvoyée ne représente rien !
}


NoeudInstSiRiche::NoeudInstSiRiche(std::vector<Noeud*> conditions, std::vector<Noeud*> sequences)
: m_conditions(conditions), m_sequences(sequences) {
}

int NoeudInstSiRiche::executer(){
    int i =0;
    for (Noeud * noeud : m_conditions) {//pour chaque condition
        if(noeud != nullptr){
            if(noeud->executer()){//si elle est vraie
                m_sequences[i]->executer(); //executer sa sequence
                break; //si la sequence a eu lieu, aucune autre sequence ne peut avoir lieu.
            }
        }else{ //la sequence est dans un "sinon". on l'execute de toutes manieres, car on ne parvient ici que si toutes les autres conditions sont fausses.
            m_sequences[i]->executer();
            break;
        }
        i++;
    }
    return 0;
}

NoeudInstPour::NoeudInstPour(Noeud * startAff, Noeud * condition, Noeud * endAff, Noeud * seqInst)
:m_startAff(startAff), m_condition(condition), m_endAff(endAff), m_seqInst(seqInst) {

}

int NoeudInstPour::executer(){
    if (m_startAff != nullptr) {
        m_startAff->executer();
    }
    while(m_condition->executer()){
        m_seqInst->executer();
        if (m_endAff != nullptr) {
            m_endAff->executer();
        }
    }
    return 0;
}

NoeudInstLire::NoeudInstLire(std::vector<Noeud*> lectures)
:m_lectures(lectures){

}

int NoeudInstLire::executer(){
    for (Noeud* lecture : m_lectures) {
        int in;
        std::cin>>in;
        ((SymboleValue*)lecture)->setValeur(in);
    }
    return 0;
}
