#include "blocks.h"

BlockRenderType getBlockRenderType(int type) {
    switch (type) {
    case ID_GRAMA:
    case ID_GRAMA_NEVE:
    case ID_PEDRA:
    case ID_CARVALHO:
    case ID_PINHEIRO:
    case ID_AREIA:
    case ID_ARENITO:
    case ID_TERRA:
    case ID_ROCHA_MATRIZ:
    case ID_CACTO:
        return SOLID;
    case ID_FOLHA_CARVALHO:
    case ID_VIDRO:
        return TRANSLUCENT;
    case ID_AGUA:
        return WATER;
    case ID_FLOR_ROSA:
    case ID_FLOR_MARGARIDA:
    case ID_GRAMA_BAIXA:
        return CROSS;
    default:
        return AIR;
    }
}

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

    // Vidro
    database[ID_VIDRO] = {"Vidro", ID_VIDRO, {52, 6}, {52, 6}, {52, 6}};

    // Areia
    database[ID_AREIA] = {"Areia", ID_AREIA, {53, 12}, {53, 12}, {53, 12}};
    
    // Arenito
    database[ID_ARENITO] = {"Arenito", ID_ARENITO, {56, 12}, {54, 12}, {55, 12}};

    // Tronco (Carvalho)
    database[ID_CARVALHO] = {"Tronco de carvalho", ID_CARVALHO, {44, 9}, {43, 9}, {44, 9}};

    // Tronco (Pinheiro)
    database[ID_PINHEIRO] = {"Tronco de pinheiro", ID_PINHEIRO, {9, 14}, {8, 14}, {9, 14}};

    // Folha (Carvalho)
    database[ID_FOLHA_CARVALHO] = {"Folha de carvalho", ID_FOLHA_CARVALHO, {42, 9}, {42, 9}, {42, 9}};

    // Flor (Rosa)
    database[ID_FLOR_ROSA] = {"Rosa", ID_FLOR_ROSA, {22, 11}, {22, 11}, {22, 11}};

    // Flor (Margarida)
    database[ID_FLOR_MARGARIDA] = {"Margarida", ID_FLOR_MARGARIDA, {5, 10}, {5, 10}, {5, 10}};

    // Grama baixa
    database[ID_GRAMA_BAIXA] = {"Grama", ID_GRAMA_BAIXA, {17, 13}, {17, 13}, {17, 13}};

    // Cacto
    database[ID_CACTO] = {"Cacto", ID_CACTO, {6, 2}, {5, 2}, {6, 2}};

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