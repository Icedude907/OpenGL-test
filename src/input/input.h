#pragma once

/* A nice, medium sized, input system
    Provides a way to hook into and filter for specific input events in an easy to use fashion
    Triggers are activated as soon as the input is recieved, not every frame or something
    - For every frame hooks, consider adding an event to a RunnableRegistry.
*/
#include <vector>
#include <set>
#include <map>
#include <variant>

#include <vkfw/vkfw.hpp>

#include "../util/int.hpp"
#include "../util/registry.h"


namespace Input{
    
    // A handle that dictates if an input context is active
    // Specific contexts can be looked up from a string at compile time
    struct InputContext{
        size_t id;
    };
    namespace Actions{
        using Button_t = u8;
        enum Button: Button_t{
            PRESS   = 1,
            RELEASE = 1 << 1,
        };
        // using MousePos = u8;
        // constexpr MousePos MOVED = 1;
        // using Analog = u8;
        // constexpr Analog CHANGED = 1;
    }
    using ButtonKey = std::variant<vkfw::Key, vkfw::MouseButton>;
    namespace Action{
        struct Button{
            ButtonKey key;
            Actions::Button action;

            constexpr inline bool matches(ButtonKey k){
                return key == k;
            }
            constexpr inline bool matches(ButtonKey k, Actions::Button a){
                return key == k && action == a;
            }
            constexpr inline bool pressed(){
                return action == Actions::PRESS;
            }
        };
        struct MousePos{
            double x, y;
        };
        struct Analog{
            
        };
    }
    namespace Trigger{
        struct Button{
            ButtonKey key;
            Actions::Button_t actions;
            
            // Returns true if the given action is being detected by the trigger
            inline bool satisfies(Action::Button action){ 
                return key == action.key && actions & action.action; 
            }
        };
        struct MousePos{
            double x, y;
        };
        struct Analog{
            
        };
    }
    // Defines a list of potential input triggers to a callback
    // Includes a function which should check if the input can be run in the current contexts. (TODO: Reevaluated only when the context changes.)
    using ActiveContexts = std::set<InputContext>;
    template <typename trigger_t, typename event_t>
    struct InputHandler{
        using event_f = std::function<void(event_t)>;
        using trigger_f = std::function<bool(std::set<InputContext>&)>;
        using triggers_t = std::vector<trigger_t>;
        event_f onEvent;
        trigger_f shouldTrigger;
        triggers_t triggers;

        inline InputHandler(event_f onEvent, trigger_f shouldTrigger, triggers_t triggers)
        : onEvent(onEvent), shouldTrigger(shouldTrigger), triggers(triggers){}
        inline InputHandler(event_f onEvent, triggers_t triggers)
        : onEvent(onEvent), shouldTrigger(globalTrigger), triggers(triggers){}

        static bool globalTrigger(ActiveContexts&){ return true; }
    };
    // Defines which contexts are currently active.
    // Inputs bound to specific contexts are only executed if that context is active.
    using ButtonInputHandler = InputHandler<Trigger::Button, Action::Button>;
    using MousePosInputHandler = InputHandler<Trigger::MousePos, Action::MousePos>;
    using AnalogInputHandler = InputHandler<Trigger::Analog, Action::Analog>;
    struct InputInstance{
        template <typename Value> using MapRegistry = Util::Registry<Value, std::map>;

        ActiveContexts activeContexts;
        MapRegistry<ButtonInputHandler> buttonHandlers;
        MapRegistry<MousePosInputHandler> mouseHandlers;
        MapRegistry<AnalogInputHandler> analogHandlers;

        void buttonPress(ButtonKey key){
            executeButtonHandlers({key, Actions::PRESS});
        }
        void buttonRelease(ButtonKey key){
            executeButtonHandlers({key, Actions::RELEASE});
        }
        // Executes mouse handlers. x and y are relative to top left of screen, and may have subpixel values depending on the environment (hence double).
        void mouseMove(double x, double y){
            executeMouseHandlers({x, y});
        }
        void executeButtonHandlers(Action::Button event){
            for(auto& [id, handler]: buttonHandlers.store){
                if(handler.shouldTrigger(activeContexts)){
                    // Find a trigger that satisfies should run
                    for(auto& trigger: handler.triggers){
                        if(trigger.satisfies(event)){
                            handler.onEvent(event);
                            break; // Only breaks this inner for loop
                        }
                    }
                }
            }
        }
        void executeMouseHandlers(Action::MousePos event){
            for(auto& [id, handler]: mouseHandlers.store){
                if(handler.shouldTrigger(activeContexts)){
                    handler.onEvent(event);
                }
            }
        }
        inline std::optional<size_t> addButtonHandler(const ButtonInputHandler& handler){
            return buttonHandlers.insert(handler);
        }
        inline bool removeButtonHandler(const size_t id){
            return buttonHandlers.erase(id);
        }
    };
    static InputInstance inputSystem;
}