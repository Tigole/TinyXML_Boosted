cmake_minimum_required(VERSION 3.10)

 

# Nom du projet

project(TinyXML_Boosted)

 

# Ajout des fichiers sources et d'en-tête

set(SOURCES

    ../../Sources/XMLFileLoader.cpp

    ../../Sources/tinyxml.cpp

    ../../Sources/tinyxmlerror.cpp

    ../../Sources/tinyxmlparser.cpp

)

 

set(HEADERS

    ../../Include/TinyXML_Boosted/XMLFileLoader.hpp

    ../../Include/TinyXML_Boosted/tinystr.h

    ../../Include/TinyXML_Boosted/tinyxml.h

)

 

# Création de la bibliothèque

add_library(TinyXML_Boosted ${SOURCES} ${HEADERS})

 

# Ajout du chemin d'accès aux fichiers d'en-tête

target_include_directories(TinyXML_Boosted PUBLIC ../../Include/TinyXML_Boosted)

 

# Spécification du répertoire d'installation pour la bibliothèque et les fichiers d'en-tête

install(TARGETS TinyXML_Boosted

        LIBRARY DESTINATION lib

        ARCHIVE DESTINATION lib)

 

install(FILES ${HEADERS} DESTINATION include/TinyXML_Boosted)