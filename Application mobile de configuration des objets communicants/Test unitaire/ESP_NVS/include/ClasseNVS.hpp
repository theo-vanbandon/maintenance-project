/**
 * Fichier Interface de la classe
 * Nom du fichier : ClasseNVS.hpp
 * Nom de la classe : ClasseNVS
 * Description : Fichier d'interface de la classe ClasseNVS
 * Auteur : Théo Vanbandon
 * Date : 27/02/2023
 **/

#pragma once
#include <Preferences.h> //Pour le type String

class ClasseNVS {
	private:
		String TypeObjet;
    	String SSID;
    	String Password;
    	String UUID_Moteur;
    	unsigned int Frequence;
    	String UUID_Machine;

	public:
		ClasseNVS();
		unsigned char CreationNamespace(const char *nomNamespace);
		unsigned char AddValueStr(int index, const char *valeurVariable);
		unsigned char AddValueInt(int index, unsigned int valeurVariable);
		unsigned char RemoveAllPref();
		unsigned char RemovePref(const char *nomPref);
		unsigned char ClosePref();
		~ClasseNVS();
};