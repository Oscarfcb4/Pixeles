#pragma once
#include "R_AllResources.hpp"

// Forward declaration
struct Node;

// Gestor de recursos
struct ResourceManager{
    // Constructor por defecto
    ResourceManager(){};
    // Constructor que inicializa el puntero a la raiz de la escena
    ResourceManager(Node* node){scene = node;};

    // Busca si existe un modelo o un shader, si no lo crea
    R_Model* getModel(std::string name, std::string src, RType rt);
    R_Shader* getShader(std::string name, std::string vs, std::string fs);

    // Metodo de crear y recuperar la raiz de la escena
    void setScene(Node* node){scene = node;};
    Node* getScene(){return scene;};

    // Metodos de carga, son tameplates que luego se aprovecharan mas si 
    // separas los modelos en tipos más específicos y tu motor crezca
    template <class ResourceType>
    ResourceType* getResource(std::string name, std::string src, RType rt){
        // Buscamos en el vector de recursos con un iterador
        std::vector<std::shared_ptr<R_Resource>>::iterator it = std::find_if(resources.begin(), 
            resources.end(), [&] (std::shared_ptr<R_Resource> r){
            if(r->name == name){
                return true;
            }
            return false;
        });
        // Si ha encontrado, no entra en este if, si no ha encontrado ninguno con
        // ese nombre, entra aquí y crea un recurso nuevo
        if(it == resources.end()){
            // Crea el nuevo recurso
            auto newRec = std::make_shared<ResourceType>();
            // Llama al cargador de la clase
            newRec->load(src, rt);
            // Le asigna el nombre
            newRec->setName(name);
            // Le asigna la raiz
            newRec->setScene(scene);
            // Actualiza el vector de recursos y lo devuelve
            resources.push_back(std::static_pointer_cast<R_Resource>(newRec));
            return newRec.get();
        }
        // Si lo ha encontrado lo devuelve
        return  std::dynamic_pointer_cast<ResourceType>(*it).get(); 
    }

    // El de los shaders requiere uno especial, lo he hecho así para agilizar
    // pero se podría hacer un método general
    template <class ResourceType>
    ResourceType* getResource(std::string name, std::string vs, std::string fs){
        std::vector<std::shared_ptr<R_Resource>>::iterator it = std::find_if(resources.begin(), 
            resources.end(), [&] (std::shared_ptr<R_Resource> r){
            if(r->name == name){
                return true;
            }
            return false;
        });
        if(it == resources.end()){
            auto newRec = std::make_shared<ResourceType>(vs.c_str(), fs.c_str());
            newRec->setName(name);
            newRec->setScene(scene);
            resources.push_back(std::static_pointer_cast<R_Resource>(newRec));
            return newRec.get();
        }
        return  std::dynamic_pointer_cast<ResourceType>(*it).get(); 
    }

    private:
        // Vector de recursos, NECESITA que sean shared y no unique para poder usar los metodos 
        // static_pointer_cast y dynamic_pointer_cast y asi poder aprovechar el polimorfismo
        std::vector<std::shared_ptr<R_Resource>> resources;
        // Puntero a la raiz
        Node* scene{};
};

