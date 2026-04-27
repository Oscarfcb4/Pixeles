#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include "TAllResources.hpp"

struct TNode;

// Gestor de recursos
struct TResourceManager{
    // Constructor por defecto
    TResourceManager(){};
    // Constructor que inicializa el puntero a la raiz de la escena
    TResourceManager(TNode* node){scene = node;};

    // Busca si existe un modelo o un shader, si no lo crea
    TR_Model* getModel(std::string name, std::string src, RType rt);
    TR_Shader* getShader(std::string name, std::string vs, std::string fs);

    // Metodo de crear y recuperar la raiz de la escena
    void setScene(TNode* node){scene = node;};
    TNode* getScene(){return scene;};

    // Metodos de carga, son tameplates que luego se aprovecharan mas cuando, como he comentado en TR_Model.hpp, separe los modelos
    // en distintos tipos mas especificos. Lo he tenido que dejar asi porque mira, no hay tiempo a hacerlo para la entrega.
    template <class ResourceType>
    ResourceType* getResource(std::string name, std::string src, RType rt){
        // Buscamos en el vector de recursos con un iterador, si hay algun recurso con el mismo nombre le devuelve si no lo crea
        std::vector<std::unique_ptr<TResource>>::iterator it = std::find_if(resources.begin(), resources.end(), [&] (std::unique_ptr<TResource>& r){
            if(r->name == name){
                return true;
            }
            return false;
        });
        if(it == resources.end()){
            // Crea el nuevo recurso
            auto newRec = std::make_unique<ResourceType>();
            // Llama al cargador de la clase
            newRec->load(src, texturesLoaded, rt);
            // Le asigna el nombre
            newRec->setName(name);
            // Le asigna la raiz
            newRec->setScene(scene);
            // Actualiza el vector de recursos y lo devuelve
            resources.push_back(std::move(newRec));
            return dynamic_cast<ResourceType*>(resources[resources.size()-1].get());
        }
        // Si lo ha encontrado lo devuelve
        return  dynamic_cast<ResourceType*>((*it).get()); 
    }

    // El de los shaders requiere uno especial. ya lo hare para que funcione con la template de arriba
    template <class ResourceType>
    ResourceType* getResource(std::string name, std::string vs, std::string fs){
        std::vector<std::unique_ptr<TResource>>::iterator it = std::find_if(resources.begin(), resources.end(), [&] (std::unique_ptr<TResource>& r){
            if(r->name == name){
                return true;
            }
            return false;
        });
        if(it == resources.end()){
            auto newRec = std::make_unique<ResourceType>(vs.c_str(), fs.c_str());
            newRec->setName(name);
            newRec->setScene(scene);
            resources.push_back(std::move(newRec));
            return dynamic_cast<ResourceType*>(resources[resources.size()-1].get());
        }
        return  dynamic_cast<ResourceType*>((*it).get()); 
    }

    TR_Shader* loadBasicShader(std::string name);

    // Vector que almacena las texturas cargadas
    std::vector<_Texture> texturesLoaded{};
    private:
        // Vector de recursos, NECESITA que sean shared y no unique para poder usar los metodos static_pointer_cast y dynamic_pointer_cast
        // y asi poder aprovechar el polimorfismo
        std::vector<std::unique_ptr<TResource>> resources;
        // Puntero a la raiz
        TNode* scene{};
        uint32_t lastRay{};
};

