#pragma once

struct TNode;
struct _Texture;

// Metodo general de recursos, que lo heredaran todos los recursos
struct TResource{
    // Destructor por defecto, importante para la gestion de los smartpointers
    virtual ~TResource() = default;
    // Recupera el nombre del recurso
    std::string getName(){return name;};
    // Setea el nombre del recurso
    void setName(std::string name_){name = name_;};
    // Setea la escena
    void setScene(TNode* node){scene = node;};
    // Metodo de carga generico, los parametros tendran mas o menos valor segun el recurso
    virtual void load(const std::string& name, [[maybe_unused]] std::vector<_Texture>& texturesLoaded, [[maybe_unused]] RType rt) = 0;
    
    // Nombre del recurso, importante para no repetir recursos
    std::string name{};
    // Putero a la raiz de la escena
    TNode* scene;
};