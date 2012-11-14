#include "CommonAlembic.h"
#include "CommonSceneGraph.h"


//SceneNode::~SceneNode()
//{
//   ESS_LOG_WARNING("deleting node");
//}


Abc::IObject SceneNodeAlembic::getObject()
{
   return iObj;
}

bool SceneNodeAlembic::wasMerged()
{
   return bWasMerged;
}

void SceneNodeAlembic::setMerged(bool bMerged)
{
   bWasMerged = bMerged;
}


struct PrintStackElement
{
   SceneNodePtr eNode;
   int level;
   PrintStackElement(SceneNodePtr enode, int l):eNode(enode), level(l)
   {}
};

void printSceneGraph(SceneNodePtr root, bool bOnlyPrintSelected)
{
 
   char* table[]={
      "SCENE_ROOT",
      "ETRANSFORM",// external transform (a parent of a geometry node)
      "ITRANSFORM",// internal transform (all other transforms)
      "CAMERA",
      "POLYMESH",
      "SUBD",
      "SURFACE",
      "CURVES",
      "PARTICLES",
      "LIGHT",
      "UNKNOWN",
      "NUM_NODE_TYPES"
   };

   ESS_LOG_WARNING("ExoSceneGraph Begin");

   std::list<PrintStackElement> sceneStack;
   
   sceneStack.push_back(PrintStackElement(root, 0));

   while( !sceneStack.empty() )
   {

      PrintStackElement sElement = sceneStack.back();
      SceneNodePtr eNode = sElement.eNode;
      sceneStack.pop_back();

      if(!bOnlyPrintSelected || (bOnlyPrintSelected && eNode->selected)){
         ESS_LOG_WARNING("Level: "<<sElement.level<<" - Name: "<<eNode->name<<" - Selected: "<<(eNode->selected?"true":"false")<<" - path: "<<eNode->dccIdentifier<<" - type: "<<table[eNode->type]);
            //<<" - identifer: "<<eNode->dccIdentifier);
        
      }

    

      for( std::list<SceneNodePtr>::iterator it = eNode->children.begin(); it != eNode->children.end(); it++){
         sceneStack.push_back(PrintStackElement(*it, sElement.level+1));
      }
   }

   ESS_LOG_WARNING("ExoSceneGraph End");
}

bool hasExtractableTransform( SceneNode::nodeTypeE type )
{
   return 
      type == SceneNode::CAMERA ||
      type == SceneNode::POLYMESH ||
      type == SceneNode::SUBD ||
      type == SceneNode::SURFACE ||
      type == SceneNode::CURVES ||
      type == SceneNode::PARTICLES;
}

struct SelectChildrenStackElement
{
   SceneNodePtr eNode;
   bool bSelectChildren;
   SelectChildrenStackElement(SceneNodePtr enode, bool selectChildren):eNode(enode), bSelectChildren(selectChildren)
   {}
};
void selectNodes(SceneNodePtr root, SceneNode::SelectionT selectionMap, bool bSelectParents, bool bChildren, bool bSelectShapeNodes)
{


   std::list<SelectChildrenStackElement> sceneStack;
   
   sceneStack.push_back(SelectChildrenStackElement(root, false));

   while( !sceneStack.empty() )
   {
      SelectChildrenStackElement sElement = sceneStack.back();
      SceneNodePtr eNode = sElement.eNode;
      sceneStack.pop_back();

      bool bSelected = false;
      if(selectionMap.find(eNode->dccIdentifier) != selectionMap.end() && 
         (eNode->type == SceneNode::ETRANSFORM || eNode->type == SceneNode::ITRANSFORM)){
         
         //this node's name matches one of the names from the selection map, so select it
         eNode->selected = true;

         if(eNode->type == SceneNode::ETRANSFORM && bSelectShapeNodes){
            for(std::list<SceneNodePtr>::iterator it=eNode->children.begin(); it != eNode->children.end(); it++){
               if(::hasExtractableTransform((*it)->type)){
                  (*it)->selected = true;
                  break;
               }
            }
         }

         if(bSelectParents){// select all parent nodes
            SceneNode* currNode = eNode->parent;
            while(currNode){
               currNode->selected = true;
               currNode = currNode->parent;
            }
         }

         if(bChildren){// select the children
            bSelected = true;
         } 
      }
      if(sElement.bSelectChildren){
         bSelected = true;
         eNode->selected = true;
      }

      for( std::list<SceneNodePtr>::iterator it = eNode->children.begin(); it != eNode->children.end(); it++){
         sceneStack.push_back(SelectChildrenStackElement(*it, bSelected));
      }
   }

   root->selected = true;
}


//void filterNodeSelection(SceneNodePtr root, bool bExcludeNonTransforms)
//{
//   struct stackElement
//   {
//      SceneNodePtr eNode;
//      stackElement(SceneNodePtr enode):eNode(enode)
//      {}
//   };
//
//   std::list<stackElement> sceneStack;
//   
//   sceneStack.push_back(stackElement(root));
//
//   while( !sceneStack.empty() )
//   {
//      stackElement sElement = sceneStack.back();
//      SceneNodePtr eNode = sElement.eNode;
//      sceneStack.pop_back();
//
//      if(bExcludeNonTransforms && 
//         (eNode->type != SceneNode::ITRANSFORM && eNode->type != SceneNode::ETRANSFORM && eNode->type != SceneNode::UNKNOWN)
//      ){
//         eNode->selected = false;
//      }
//
//      for( std::list<SceneNodePtr>::iterator it = eNode->children.begin(); it != eNode->children.end(); it++){
//         sceneStack.push_back(stackElement(*it));
//      }
//   }
//
//   root->selected = true;
//}
