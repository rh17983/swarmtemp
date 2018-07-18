
/**
 * @file <argos3/core/wrappers/lua/lua_controller.cpp>
 *
 * @author Carlo Pinciroli <ilpincy@gmail.com>
 */
#include "lua_controller.h"
#include <argos3/core/utility/logging/argos_log.h>

#include <argos3/core/wrappers/lua/lua_utility.h>

namespace argos {

   /****************************************/
   /****************************************/

   CLuaController::CLuaController() :
      m_ptLuaState(NULL),
      m_bScriptActive(false),
      m_bIsOK(true),
      m_pcRNG(NULL) {
   }

   /****************************************/
   /****************************************/

   CLuaController::~CLuaController() {
   }

   /****************************************/
   /****************************************/

   void CLuaController::Init(TConfigurationNode& t_tree) {
      try {
         /* Create RNG */
         m_pcRNG = CRandom::CreateRNG("argos");
         /* Load script */
         std::string strScriptFileName;
         GetNodeAttributeOrDefault(t_tree, "script", strScriptFileName, strScriptFileName);
         if(strScriptFileName != "") {
            SetLuaScript(strScriptFileName);
            if(! m_bIsOK) {
               THROW_ARGOSEXCEPTION("Error loading Lua script \"" << strScriptFileName << "\": " << lua_tostring(m_ptLuaState, -1));
            }
         }
         else {
            /* Create a new Lua stack */
            m_ptLuaState = luaL_newstate();
            /* Load the Lua libraries */
            luaL_openlibs(m_ptLuaState);
            /* Create and set Lua state */
            CreateLuaState();
            SensorReadingsToLuaState();
         }
      }
      catch(CARGoSException& ex) {
         THROW_ARGOSEXCEPTION_NESTED("Error initializing Lua controller", ex);
      }
   }

   /****************************************/
   /****************************************/

   void CLuaController::ControlStep() {
      if(m_bScriptActive && m_bIsOK) {
         /* Update Lua state through sensor readings */
         SensorReadingsToLuaState();
         /* Execute script step function */
         if(! CLuaUtility::CallLuaFunction(m_ptLuaState, "step")) {
            m_bIsOK = false;
         }
      }
   }

   /****************************************/
   /****************************************/

   void CLuaController::Reset() {
      if(m_bScriptActive) {
         if(m_bIsOK) {
            m_bIsOK = CLuaUtility::CallLuaFunction(m_ptLuaState, "reset");
         }
         else {
            SetLuaScript(m_strScriptFileName);
         }
      }
   }

   /****************************************/
   /****************************************/

   void CLuaController::Destroy() {
      if(m_bScriptActive && m_bIsOK) {
         /* Execute script destroy function */
         CLuaUtility::CallLuaFunction(m_ptLuaState, "destroy");
      }
      /* Close Lua */
      lua_close(m_ptLuaState);
   }

   /****************************************/
   /****************************************/

   void CLuaController::SetLuaScript(const std::string& str_script) {
      /* First, delete old script */
      if(m_bScriptActive) {
         lua_close(m_ptLuaState);
         m_bScriptActive = false;
         m_strScriptFileName = "";
      }
      /* Create a new Lua stack */
      m_ptLuaState = luaL_newstate();
      /* Load the Lua libraries */
      luaL_openlibs(m_ptLuaState);
      /* Load script */
      if(!CLuaUtility::LoadScript(m_ptLuaState, str_script)) {
         m_bIsOK = false;
         return;
      }
      m_strScriptFileName = str_script;
      /* Create and set variables */
      CreateLuaState();
      SensorReadingsToLuaState();
      /* Execute script init function */
      if(!CLuaUtility::CallLuaFunction(m_ptLuaState, "init")) {
         m_bIsOK = false;
         return;
      }
      m_bIsOK = true;
      m_bScriptActive = true;
   }

   /****************************************/
   /****************************************/

   void CLuaController::CreateLuaState() {
      /* Register functions */
      CLuaUtility::RegisterLoggerWrapper(m_ptLuaState);
      /* Create a table that will contain the state of the robot */
      lua_newtable(m_ptLuaState);
      /* Set the id of the robot */
      lua_pushstring(m_ptLuaState, "id");
      lua_pushstring(m_ptLuaState, GetId().c_str());
      lua_settable(m_ptLuaState, -3);
      /* Register RNG */
      CLuaUtility::RegisterRNG(m_ptLuaState, m_pcRNG);
      /* Go through devices and add the necessary items to the table */
      for(CCI_Actuator::TMap::iterator it = m_mapActuators.begin();
          it != m_mapActuators.end();
          ++it) {
         it->second->CreateLuaState(m_ptLuaState);
      }
      for(CCI_Sensor::TMap::iterator it = m_mapSensors.begin();
          it != m_mapSensors.end();
          ++it) {
         it->second->CreateLuaState(m_ptLuaState);
      }
      /* Set the name of the table */
      lua_setglobal(m_ptLuaState, "robot");
   }

   /****************************************/
   /****************************************/

   void CLuaController::SensorReadingsToLuaState() {
      /* Put the robot state table on top */
      lua_getglobal(m_ptLuaState, "robot");
      /* Go through the sensors */
      for(CCI_Sensor::TMap::iterator it = m_mapSensors.begin();
          it != m_mapSensors.end();
          ++it) {
         it->second->ReadingsToLuaState(m_ptLuaState);
      }
      /* Pop the robot state table */
      lua_pop(m_ptLuaState, 1);
   }

   /****************************************/
   /****************************************/

   std::string CLuaController::GetErrorMessage() {
      if(m_bIsOK) {
         return "OK";
      }
      else {
         return lua_tostring(m_ptLuaState, -1);
      }
   }

   /****************************************/
   /****************************************/

   REGISTER_CONTROLLER(CLuaController, "lua_controller");

}
