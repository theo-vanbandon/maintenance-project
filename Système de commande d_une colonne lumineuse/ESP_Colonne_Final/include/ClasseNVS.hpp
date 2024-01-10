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
    	int Frequence;
    	String UUID_Machine;

	public:
		ClasseNVS();
		unsigned char CreationNamespace(const char * prmNomNamespace);
		unsigned char AddValueStr(int prmIndex, const char * prmValeurVariable);
		unsigned char AddValueInt(int prmIndex, int prmValeurVariable);
		unsigned char RemoveAllPref();
		unsigned char RemovePref(const char * prmNomPref);
		unsigned char ClosePref();
		String LireValeurStr(int prmIndex);
		int LireValeurInt(int prmIndex);
		~ClasseNVS();
};