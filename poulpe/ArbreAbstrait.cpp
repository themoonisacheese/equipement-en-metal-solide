#include <stdlib.h>
#include <typeinfo>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"


Noeud::~Noeud() {}
// void Noeud::toPython(ostream &out, unsigned int indentation){}
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
void NoeudSeqInst::toPython(ostream &out, unsigned int indentation){
  for (auto inst:m_instructions){
    inst->toPython(out, indentation);
  }
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
void NoeudAffectation::toPython(ostream &out, unsigned int indentation){
    out << setw(4*indentation);
    ((SymboleValue *)m_variable)->toPython(out, indentation);
    out << " = ";
    if ((typeid(*m_expression)==typeid(SymboleValue))) {
      ((SymboleValue *)m_expression)->toPython(out, 0);
    } else {
      m_expression->toPython(out, 0);
    }
    out << endl;
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

void NoeudOperateurBinaire::toPython(ostream &out, unsigned int indentation){
    ((SymboleValue *)m_operandeGauche)->toPython(out, indentation);
    out<<" ";
    m_operateur.toPython(out, indentation);
    out<<" ";
    ((SymboleValue *)m_operandeDroit)->toPython(out, indentation);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

//NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
//: m_condition(condition), m_sequence(sequence) {
//}
//
//int NoeudInstSi::executer() {
//  if (m_condition->executer()) m_sequence->executer();
//  return 0; // La valeur renvoyée ne représente rien !
//}

//////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
//////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTantQue::executer() {
    while(m_condition->executer()) m_sequence->executer();
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstTantQue::toPython(ostream& out, unsigned int indentation) {
    out << setw(4*indentation) << "while ";
    m_condition -> toPython(out,0);
    out << " :" << endl;
    m_sequence -> toPython(out, indentation+1);
    out << endl;
}


//////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
//////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstRepeter::executer() {
    do {m_sequence->executer(); } while(m_condition->executer());
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstRepeter::toPython(ostream &out, unsigned int indentation){
    out << setw(4*indentation) << "while True:" <<endl;
    m_sequence->toPython(out, indentation +1);
    out << setw(4*(indentation+1)) << "if not (";
    m_condition -> toPython(out, 0);
    out<< ") :" << endl;
    out << setw(4*(indentation +2)) << "break";
}


//////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
//////////////////////////////////////////////////////////////////////

NoeudInstEcrire::NoeudInstEcrire(std::vector<Noeud*> sequences)
: m_sequences(sequences) {
}

int NoeudInstEcrire::executer() {
    for (Noeud* seq : m_sequences) {
        if ( (typeid(*seq)==typeid(SymboleValue) &&  *((SymboleValue*)seq)== "<CHAINE>" )){
            std::cout << ((SymboleValue*)seq)->getChaine();
        }else if (((SymboleValue*)seq)-> estDefini()){
            std::cout << seq->executer();
        }
        else {
            throw "essai d'affichage d'une valeur indefinie";
        }
    }
    std::cout<<endl;
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstEcrire::toPython(ostream &out, unsigned int indentation){
    out<< setw(4*indentation) << "print ";
    for (Noeud * seq : m_sequences) {
        out << ((SymboleValue*)seq)->getChaine() << ", "; //python ignore les virgules non suivies
    }
    out << "\"\\n\"";
    out << endl;
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

void NoeudInstSiRiche::toPython(ostream &out, unsigned int indentation){
    int i =0;
    out << setw(4*indentation);// required here because of hacky hack
    for (Noeud * noeud : m_conditions) {//pour chaque condition
        if(noeud != nullptr){
            out <<"if ";
            noeud->toPython(out, 0);
            out << " :" << endl;
            m_sequences[i]->toPython(out, indentation +1);
            if (m_conditions.size()>i && m_conditions[i+1] != nullptr) {
                out<< setw(4*indentation)<< "el"; // whoops
            }
        }else{ //la sequence est dans un "sinon".
            out << setw(4*indentation) << "else:"<<endl;
            m_sequences[i]->toPython(out, indentation +1);
            break;
        }
        i++;
    }
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

void NoeudInstPour::toPython(ostream& out, unsigned int indentation) { //FIXME: test for nullptr
    if(m_startAff != nullptr)
        m_startAff->toPython(out, indentation);
    out << setw(4*indentation) << "while ";
    m_condition->toPython(out, 0);
    out << " :" << endl;
    m_seqInst -> toPython(out, indentation+1);
    if(m_endAff != nullptr)
        m_endAff->toPython(out, indentation +1);
    out << endl;
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

void NoeudInstLire::toPython(ostream& out, unsigned int indentation) {
    for (Noeud* lecture : m_lectures){
        out << setw(4*indentation) << ((SymboleValue*)lecture)-> getChaine()
                << "input(int())" << endl;

    }
}
