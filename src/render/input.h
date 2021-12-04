#pragma once

#include "../util/int.hpp"
#include <functional>
#include <vector>
#include <set>
#include <map>
#include <variant>

#include <vkfw/vkfw.hpp>

#include "../util/util.h"
#include "../util/registry.h"

/*
    This could be made faster with sorting & hash searching
*/
namespace input{
    struct Key{
        std::variant<vkfw::Key, vkfw::MouseButton> key;
        constexpr inline friend bool operator==(const Key& lhs, const Key& rhs){
            return lhs.key == rhs.key;
        }
        constexpr inline friend bool operator<(const Key& lhs, const Key& rhs){
            return lhs.key < rhs.key;
        }
    };
    
    using Action = u8;
    namespace Actions{
        constexpr Action PRESS   = 0b00000001;
        constexpr Action RELEASE = 0b00000010;
        constexpr inline bool pressed(Action action){
            return action & PRESS;
        }
    }
    struct MonitoredActions{
        bool press : 1;
        bool release : 1;

        inline MonitoredActions(bool press, bool release): press(press), release(release){}
        inline MonitoredActions(Action mask): press(mask & Actions::PRESS), release(mask & Actions::RELEASE){}
        inline bool defined(Action action){
            // theres a better way to do this?
            return (action & Actions::PRESS && press == true)
            || (action & Actions::RELEASE && release == true);
        }
        // MonitoredActions(u8 field): press(u8 in & press),

    };
    enum SourceDevice{
        Keyboard,
        Mouse,
        Joystick,  
    };
    struct InputEvent{
        Util::AnyPtr analogAction = nullptr; // Ptr to analog data if needed
        Key key;
        Action action;
        SourceDevice device;

        inline InputEvent(Key key, Action action): key(key), action(action){}

        // Returns true if functionally equvilent
        constexpr inline friend bool operator==(const InputEvent& lhs, const InputEvent& rhs){
            return lhs.key == rhs.key && lhs.action == rhs.action;
        }
        // Can i do this implicitly?
        constexpr inline bool matches(Key k){
            return key == k;
        }
        constexpr inline bool matches(Key k, Action a){
            return key == k && action == a;
        }
        constexpr inline bool pressed(){
            return Actions::pressed(action);
        }
    };
    struct InputEventMontior{
        Key key;
        MonitoredActions actions;

        // Returns true if the given event satisfies the conditions of the monitor
        inline bool satisfied(InputEvent event){
            return (key == event.key) && actions.defined(event.action);
        }
    };
    struct InputHandler{
        std::vector<InputEventMontior> monitoredEvents; 
        std::function<void(InputEvent)> onEvent;
    };
    /** The key to the input system
     * Holds a list of registered handlers for specific input events
     * If multiple handlers are bound to the same key, they will all trigger
     * When a handler's condition is satisfied, it will immediately execute on the input thread
     * - If you want something that runs every frame, add it to the correct registry and then remove it when appropriate
    */
    struct Inputs{
        using handlerID = size_t;
        Util::Registry<InputHandler, std::map, handlerID> registeredHandlers;
        std::set<Key> pressedKeys;

        inline void press(Key k){
            pressedKeys.insert(k);
            executeHandlers(InputEvent(k, Actions::PRESS));
        }
        inline void release(Key k){
            pressedKeys.erase(k);
            executeHandlers(InputEvent(k, Actions::RELEASE));
        }
        // Could be made faster by splitting out press and release into separate functions
        void executeHandlers(InputEvent event){
            for(auto& [id, handler]: registeredHandlers.store){
                for(auto& eventMonitor: handler.monitoredEvents){
                    if(eventMonitor.satisfied(event)){
                        handler.onEvent(event);
                        break; // Only breaks this inner for loop
                    }
                }
            }
        }
        inline std::optional<handlerID> addHandler(const InputHandler& handler){
            return registeredHandlers.insert(handler);
        }
        inline bool removeHandler(const handlerID id){
            return registeredHandlers.erase(id);
        }
        // bool removeHandler
        
    };
    static Inputs inputSystem;

    namespace callbacks{
        void keyboardhandler(const vkfw::Window& window, vkfw::Key key, int32_t scancode, vkfw::KeyAction action, vkfw::ModifierKeyFlags modifiers){
            if(action == vkfw::KeyAction::Press){
                inputSystem.press(Key{key});
            }else if(action == vkfw::KeyAction::Release){
                inputSystem.release(Key{key});
            }
        }
        void mousebuttonhandler(const vkfw::Window& window, vkfw::MouseButton button, vkfw::MouseButtonAction action, vkfw::ModifierKeyFlags modifiers){
            if(action == vkfw::MouseButtonAction::Press){
                inputSystem.press(Key{button});
            }else if(action == vkfw::MouseButtonAction::Release){
                inputSystem.release(Key{button});
            }
        }
    }

    void vkfwSetupCallbacksForWindow(const vkfw::Window& window){
        using namespace std::placeholders;
        auto cb = window.callbacks();
        cb->on_key = callbacks::keyboardhandler;
        if(vkfw::rawMouseMotionSupported()){
            window.set<vkfw::InputMode::RawMouseMotion>(true);
        }
        cb->on_mouse_button = callbacks::mousebuttonhandler;
    }
}