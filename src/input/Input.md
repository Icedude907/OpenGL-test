# Input
```c++
Context game;
Context menu;
Context save;

auto handle = input.register(
    ()->{ return game & menu & !save; }, // Input is active when game & menu are active and while save isnt.
    std::bind(on_camera_move),
    {
        {key1, PRESS|RELEASE},
        {key2, PRESS|RELEASE},
    }
);
auto debugtrigger = input.register(
    ()->{ return true; } // Input detection is always active
);

void on_camera_move(const InputAction& is){
    // is.contexts
    // is.key
    // is.action
}

```