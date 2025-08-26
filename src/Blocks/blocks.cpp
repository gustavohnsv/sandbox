#include "blocks.h"

Blocks::Blocks() {
    
    // Bloco de Ar
    database[ID_AR] = {"Ar", ID_AR, {-1, -1}, {-1, -1}, {-1, -1}};
    
    // Bloco de Grama
    database[ID_GRAMA] = {"Grama", ID_GRAMA, {63, 6}, {60, 6}, {27, 5}};
    
    // Bloco de Grama (Neve)
    database[ID_GRAMA_NEVE] = {"Neve", ID_GRAMA_NEVE, {33, 11}, {62, 6}, {27, 5}};
    
    // Bloco de Terra
    database[ID_TERRA] = {"Terra", ID_TERRA, {27, 5}, {27, 5}, {27, 5}};

    // Bloco de Pedra
    database[ID_PEDRA] = {"Pedra", ID_PEDRA, {13, 14}, {13, 14}, {13, 14}};

    // Rocha Matriz (Bedrock)
    database[ID_ROCHA_MATRIZ] = {"Rocha matriz", ID_ROCHA_MATRIZ, {48, 0}, {48, 0}, {48, 0}};
    
    // Água
    database[ID_AGUA] = {"Água", ID_AGUA, {12, 16}, {13, 16}, {13, 16}};

    // Areia
    database[ID_AREIA] = {"Areia", ID_AREIA, {53, 12}, {53, 12}, {53, 12}};
    
    // Arenito
    database[ID_ARENITO] = {"Arenito", ID_ARENITO, {56, 12}, {54, 12}, {55, 12}};

    // Tronco (Carvalho)
    database[ID_CARVALHO] = {"Tronco de carvalho", ID_CARVALHO, {44, 9}, {43, 9}, {44, 9}};

    // Tronco (Pinheiro)
    database[ID_PINHEIRO] = {"Tronco de pinheiro", ID_PINHEIRO, {9, 14}, {8, 14}, {9, 14}};
}

Block Blocks::getBlockInfo(int type) const {
    try {
        return database.at(type);
    } catch (const std::out_of_range &e) {
        return database.at(ID_AR);
    }
}

std::map<int, Block> Blocks::getBlockDatabase() const {
    return database;
}

void Blocks::check() {
    return;
}