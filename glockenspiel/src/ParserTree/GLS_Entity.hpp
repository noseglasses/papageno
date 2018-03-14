/* Copyright 2018 noseglasses <shinynoseglasses@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "ParserTree/GLS_Node.hpp"
#include "ParserTree/GLS_Alias.hpp"

#include <set>
#include <map>

namespace Glockenspiel {
namespace ParserTree {
   
template<typename EntityType__,
         typename NextEntityType__,
         typename NextEntityCompare__>
class Entity : public Node
{   
   public:
      
      typedef std::set<NextEntityType__, NextEntityCompare__> NextEntities;
      typedef std::map<std::string, std::shared_ptr<EntityType__>> Entities;
      typedef std::map<std::string, Parser::LocationOfDefinition> LocationsOfDefinition;
      
      typedef std::vector<std::shared_ptr<EntityType__>> EntityCollection;
      typedef std::map<std::string, EntityCollection> EntitiesByType;
      
      typedef std::pair<std::shared_ptr<EntityType__>, std::shared_ptr<EntityType__>> EntityToEntity;
      typedef std::vector<EntityToEntity> EntityToEntityCollection;
      typedef std::map<std::string, EntityToEntityCollection> EntitiesToEntitiesByType;
      
      Entity(  const Parser::Token &id,
               const Parser::Token &type, 
               const Parser::Token &parameters,
               bool parametersDefined
           )
         :  Node(id),
            type_(type),
            parameters_(parameters),
            parametersDefined_(parametersDefined),
            wasRequested_(false)
      {
      }
      
      static void define(const std::shared_ptr<EntityType__> &entity)
      {
         const auto &id = entity->getId().getText();
         
         auto it = entities_.find(id);
         if(it != entities_.end()) {
            auto lodIt = locationsOfDefinition_.find(id);
            THROW_TOKEN_ERROR(entity->getId(), 
               "Action multiply defined. First definition here: " 
               << lodIt->second);
         }
         
         entities_[id] = entity;
         locationsOfDefinition_[id] = entity->getId().getLOD();
      }
      
      static const std::shared_ptr<EntityType__> &lookup(const std::string &id)
      {
         std::string aliasRepl = Alias::replace(id);
         
         auto it = entities_.find(aliasRepl);
         
         if(it == entities_.end()) {
            std::ostringstream out;
            out << "Unable to retreive undefined entity \'" << aliasRepl << "\'";
            if(id != aliasRepl) {
               out << " (alias \'" << id << "\')";
            }
            THROW_ERROR(out.str());
         }
         
         return it->second;
      }
      
      static void pushNext(const NextEntityType__ &id)
      {
         auto it = nextEntities_.find(id);
         if(it != nextEntities_.end()) {
            THROW_TOKEN_ERROR(id, "Redundant entity added. Previous definition: " << it->getLOD());
         }
         nextEntities_.insert(id);
      }
      
      static const NextEntityType__ &popNext()
      {
         if(nextEntities_.empty()) {
            THROW_ERROR("No entities available");
         }
         if(nextEntities_.size() > 1) {
            THROW_ERROR("Ambiguous entities specified");
         }
         const auto &tmp = *nextEntities_.begin();
         nextEntities_.clear();
         return tmp;
      }
      
      static bool hasNextEntities() { return !nextEntities_.empty(); }
      
      static void getNextEntities(std::vector<NextEntityType__> &entities)
      {
         entities.clear();
         std::copy(nextEntities_.begin(), nextEntities_.end(), std::back_inserter(entities));
         nextEntities_.clear();
      }
      
      void setWasRequested(bool state) { wasRequested_ = state; }
      bool getWasRequested() const { return wasRequested_; }
      
      virtual std::string getPropertyDescription() const 
      {
         return TO_STRING(Node::getPropertyDescription() << ", type = " << type_
            << ", parameters = \'" << parameters_ << "\'");
      }
      
      static EntitiesByType getEntitiesByType(bool onlyRequested = false) 
      {   
         EntitiesByType result;
         
         for(const auto &entitiesEntry: entities_) {
            if(onlyRequested && !entitiesEntry.second->getWasRequested()) { continue; }
            result[entitiesEntry.second->getType().getText()].push_back(entitiesEntry.second);
         }
         
         return result;
      }
      
      static const std::map<std::string, std::shared_ptr<EntityType__>> &getNextEntities()
      {
         return entities_;
      }
      
      const Parser::Token &getType() const 
      {
         return type_; 
      }
      
      const Parser::Token &getParameters() const
      {
         return parameters_; 
      }
      
      bool getParametersDefined() const
      {
         return parametersDefined_;
      }
      
      static void joinDuplicateEntries()
      {
         EntitiesByType ibt = getEntitiesByType();
         
         entities_.clear();
         
         for(const auto &ibtEntry: ibt) {
            
            std::map<std::string, std::shared_ptr<EntityType__>> entitiesByParameters;
            
            for(const auto &entityPtr: ibtEntry.second) {
               
               const auto &params = entityPtr->getParameters().getText();
               
               // Sort entities by parameters definition
               //
               auto it = entitiesByParameters.find(params);
               
               if(it != entitiesByParameters.end()) {
                  
                  entities_[entityPtr->getId().getText()] = it->second;
                  
                  joinedEntities_[entityPtr->getType().getText()].push_back(EntityToEntity(entityPtr, it->second));
               }
               else {
                  entitiesByParameters[params] = entityPtr;
                  entities_[entityPtr->getId().getText()] = entityPtr;
               }
            }
         }
      }
      
      static const EntitiesToEntitiesByType &getJoinedEntities() { return joinedEntities_; }
      
   protected:
      
      Parser::Token           type_;
      Parser::Token           parameters_;
      bool                    parametersDefined_;
      bool                    wasRequested_;
      
      static NextEntities nextEntities_;
      static Entities entities_;
      static LocationsOfDefinition locationsOfDefinition_;
      static EntitiesToEntitiesByType joinedEntities_;
};

template<typename EntityType__,
         typename NextEntityType__,
         typename NextEntityCompare__>
typename Entity<EntityType__, NextEntityType__, NextEntityCompare__>::NextEntities
   Entity<EntityType__, NextEntityType__, NextEntityCompare__>::nextEntities_;
   
template<typename EntityType__,
         typename NextEntityType__,
         typename NextEntityCompare__>
typename Entity<EntityType__, NextEntityType__, NextEntityCompare__>::Entities
   Entity<EntityType__, NextEntityType__, NextEntityCompare__>::entities_;
   
template<typename EntityType__,
         typename NextEntityType__,
         typename NextEntityCompare__>
typename Entity<EntityType__, NextEntityType__, NextEntityCompare__>::LocationsOfDefinition
   Entity<EntityType__, NextEntityType__, NextEntityCompare__>::locationsOfDefinition_;
   
template<typename EntityType__,
         typename NextEntityType__,
         typename NextEntityCompare__>
typename Entity<EntityType__, NextEntityType__, NextEntityCompare__>::EntitiesToEntitiesByType
   Entity<EntityType__, NextEntityType__, NextEntityCompare__>::joinedEntities_;

} // namespace ParserTree
} // namespace Glockenspiel

