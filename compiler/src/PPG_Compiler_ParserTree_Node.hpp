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

#include <memory>
#include <shared_ptr>

#define TO_STRING(...) (std::ostringstream() << __VA_ARGS__).str()

#define THROW_ERROR(...) \
   throw TO_STRING(__VA_ARGS__)

namespace Papageno {
namespace ParserTree {

class Node
{   
   public:
      
      Node() 
         :  id_(Node::generateId())
      {}
      
      
      Node(    const std::string &id)
         :  id_(id)
      {
         this->retreiveSourcePosition();
         
         Node::storeNode(id);
      }
      
      virtual ~Node() {
         Node::deleteNode(id);
      }
   
      virtual std::string getPropertyDescription() const {
         return TO_STRING("id = " << id_);
      }
      
      virtual std::string getNodeType() const { return "Node"; }
      
      virtual std::string getDescription() const {
         return TO_STRING(this->getNodeType() << "{ " 
            << this->getPropertyDescription() << " }");
      }
   
   protected:
      
      void retreiveSourcePosition();
      
      static void storeNode(const std::string &id, const Node *node) {
         
         auto it = ids_.find();
         if(it != ids_.end()) {
            THROW_ERROR("An entity with id \'" << id 
               << "\' has already been defined: " 
               << it->second->getPropertyDescription());
         }
         
         ids_[id] = node;
      }
      
      static void deleteNode(const std::string &id) {
         auto it = ids_.find();
         
         assert(it != ids_.end());
         
         ids_.erase(it);
      }
      
      static std::string generateId() {
         ++nextId_;
         return TO_STRING("id_" << nextId_);
      }
         
   protected:
      
      std::string           id_;
      int                   sourcePos_[2];
      
      static std::map<std::string, *Node> ids_;
      
      static int           nextId_;
};

class Action : public Node
{
   public:
      
      typedef std::pair<int, std::string> CountToAction;
      
      Action(  const std::string &id,
               const std::string &type, 
               const std::string &source)
         :  Node(id),
            type_(type),
            source_(source)
      {}
      
      const std::string &getId() const { return id_; }
      
      static void pushAction(const CountToAction &cta) {
         actions_.push_back(cta);
      }
      
      static void popAction() {
         if(inputs_.empty()) {
            THROW_ERROR("No actions available");
         }
         
         auto tmp = actions_.back();
         actions_.pop_back();
         return tmp;
      }
      
      static std::vector<CountToAction> getActions() {
         std::vector<CountToAction> tmp;
         tmp.swap(inputs_);
         return tmp;
      }
         
      static bool hasActions() {
         return !actions_.empty();      
      }
      
      virtual std::string getPropertyDescription() const {
         return TO_STRING(Node::getPropertyDescription() << ", type = " << type_
            << ", source = \'" << source_ << "\'");
      }
      
      virtual std::string getNodeType() const { return "Action"; }
      
   protected:
      
      std::string           type_;
      std::string           source_;
      
      static std::vector<CountToAction>    actions_;
      
};

class Input : public Node
{
   public:
      
      Input(  const std::string &id,
               const std::string &type, 
               const std::string &source)
         :  Node(id),
            type_(type),
            source_(source)
      {}
      
      static void pushInput(const std::shared_ptr<Input> &input) {
         inputs_.push_back(input);
      }
      
      static std::shared_ptr<Input> popInput() {
         if(inputs_.empty()) {
            THROW_ERROR("No inputs available");
         }
         auto tmp = inputs_.back();
         inputs_.pop_back();
         return tmp;
      }
      
      static void getInputs(std::vector<std::shared_ptr<Input>> &inputs) {
         inputs.clear();
         inputs.swap(inputs_);
      }
      
      virtual std::string getPropertyDescription() const {
         return TO_STRING(Node::getPropertyDescription() << ", type = " << type_
            << ", source = \'" << source_ << "\'");
      }
      
      virtual std::string getNodeType() const { return "Input"; }
      
   protected:
      
      std::string           type_;
      std::string           source_;
      
};

class Token : public Node
{
   public:
      
      void setAction(const std::shared_ptr<Action> &action) {
         action_ = action;
      }
      
      void addChild(const std::shared_ptr<Token> &token) {
         children_->push_back(token);
      }
      
      static void pushToken(const std::shared_ptr<Token> &token) {
         tokens_.push_back(token);
      }
      
      static void getTokens(std::vector<std::shared_ptr<Token>> &tokens) {
         tokens.clear();
         tokens.swap(tokens_);
      }
      
   protected:
      
      Token(const std::string &id)
         :  Node(id)
      {}
      
   protected:
      
      std::shared_ptr<Action>               action_;
      std::vector<std::shared_ptr<Token>>   children_;
      
      static std::vector<std::shared_ptr<Token>> tokens_;
};

class Phrase : public Node
{
   public:
      
      Phrase(const std::string &id) 
         :  Node(id)
      {
         Token::getTokens(tokens_);
      }
      
      virtual std::string getNodeType() const { return "Phrase"; }
      
      static std::shared_ptr<Token> lookupPhrase(const std::string &id)
      {
         auto it = phrases_.find(id);
         
         if(it == phrases_.end()) {
            THROW_ERROR("Unable to find phrase " << id)
         }
         
         return it->second;
      }
      
      static void storePhrase(const std::shared_ptr<Phrase> &phrase) {
         phrases_[phrase->getId()] = phrase;
      }
      
   protected:
      
      std::shared_ptr<Token>                phraseRoot_;
      
      static std::map<std::string, std::shared_ptr<Phrase>>  phrases_;
};

class Note : public Token
{
   public:
      
      Note()
      {
         input_ = Input::popInput();
       
         if(Action::hasActions()) {
            this->setAction(Action::popAction());
         }
      }
            
      virtual std::string getPropertyDescription() const {
         return TO_STRING(Node::getPropertyDescription() << ", input = " 
            << input_->getDescription());
      }
      
      virtual std::string getNodeType() const { return "Note"; }
      
   protected:
      
      std::shared_ptr<Input> input_;
};

class Agglomerate : public Token
{
   public:
   
      virtual std::string getPropertyDescription() const {
         
         std::ostringstream inputDesc;
         for(const auto &input: inputs_) {
            inputDesc << input->getDescription() << ", ";
         }
         return TO_STRING(Node::getPropertyDescription() << ", inputs = " 
            << inputDesc);
      }
      
   protected:
      
      Agglomerate()
      {
         Inputs::getInputs(inputs_);
      }
      
   protected:
      
      std::vector<std::shared_ptr<Input>>   inputs_;
};

class Chord : public Agglomerate
{
   public:
   
      virtual std::string getNodeType() const { return "Chord"; }
   protected:
};

class Cluster : public Agglomerate
{
   public:
   
      virtual std::string getNodeType() const { return "Cluster"; }
   protected:
};

std::vector<std::shared_ptr<Token>> tokenStack;

      
} // namespace ParserTree
} // namespace Papageno

