/**
 * @file <argos3/core/simulator/physics_engine/physics_engine.cpp>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#include <cstdlib>
#include "physics_engine.h"
#include <argos3/core/utility/logging/argos_log.h>
#include <argos3/core/utility/string_utilities.h>
#include <argos3/core/simulator/entity/entity.h>

namespace argos {

   /* The default value of the simulation clock tick */
   Real CPhysicsEngine::m_fSimulationClockTick = 0.1f;
   Real CPhysicsEngine::m_fInverseSimulationClockTick = 1.0f / CPhysicsEngine::m_fSimulationClockTick;

   /****************************************/
   /****************************************/

   CPhysicsEngine::CPhysicsEngine() :
      m_unIterations(10),
      m_fPhysicsClockTick(m_fSimulationClockTick) {}

   /****************************************/
   /****************************************/

   void CPhysicsEngine::Init(TConfigurationNode& t_tree) {
      try {
         /* Get id from the XML */
         GetNodeAttribute(t_tree, "id", m_strId);
         /* Get iterations per time step */
         GetNodeAttributeOrDefault(t_tree, "iterations", m_unIterations, m_unIterations);
         m_fPhysicsClockTick = GetSimulationClockTick() / static_cast<Real>(m_unIterations);
         LOG << "[INFO] The physics engine \""
             << GetId()
             << "\" will perform "
             << m_unIterations
             << " iterations per tick (dt = "
             << GetPhysicsClockTick() << " sec)"
             << std::endl;
      }
      catch(CARGoSException& ex) {
         THROW_ARGOSEXCEPTION("Error initializing a physics engine");
      }
   }

   /****************************************/
   /****************************************/

   Real CPhysicsEngine::GetSimulationClockTick() {
      return m_fSimulationClockTick;
   }
   
   /****************************************/
   /****************************************/

   Real CPhysicsEngine::GetInverseSimulationClockTick() {
      return m_fInverseSimulationClockTick;
   }
   
   /****************************************/
   /****************************************/

   void CPhysicsEngine::SetSimulationClockTick(Real f_simulation_clock_tick) {
      LOG << "[INFO] Using simulation clock tick = " << f_simulation_clock_tick << std::endl;
      m_fSimulationClockTick = f_simulation_clock_tick;         
      m_fInverseSimulationClockTick = 1.0f / f_simulation_clock_tick;         
   }

   /****************************************/
   /****************************************/

}
