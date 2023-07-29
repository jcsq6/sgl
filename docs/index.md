## Quick links
- [Window Guide](#window-management)
- [Input Guide](#key-class-handle)
    - [Key Codes](#key-codes)
    - [Mouse Codes](#mouse-codes)
- [Drawing guide](#drawing)

## **Window Management**
Windows are created and handled with the class `sgl::window`.  
An `sgl::window` handles window creation, destruction, and input.  
`sgl::window` inherits from `sgl::render_target`, which is detailed [here](#render_target).

#### **Creation Hints**
```
struct creation_hints {
    int window_flags
    int opengl_major
    int opengl_minor
}
```
- `creation_hints` is a class to be passed to the constructor of `sgl::window`
    - **window_flags** - bit mask of flags defining how the window is created. (see below for various flags) defaults to 0
    - **opengl_major** - major version of opengl. defaults to 4
    - **opengl_minor** - minor version of opengl. defaults to 1
- `creation_code` holds a set of flags that will be passed to `window_flags` through bitwise or
    - `creation_code::is_not_resizable` - set to make the window not resizable
    - `creation_code::is_invisible` - set to make the window invisible
    - `creation_code::is_plain` - set to make the window have no decorations
    - `creation_code::is_focused` - set to call focus on window creation
    - `creation_code::should_auto_minimize` - set to automatically minimize window upon lost focus
    - `creation_code::is_always_top` - set to make window remain above all other windows
    - `creation_code::is_maximized` - set to make the window maximized on creation
    - `creation_code::cursor_is_centered` - set to make the cursor centered on full screen windows only
    - `creation_code::framebuffer_is_transparent` - set to make a transparent frambebuffer (if supported)
    - `creation_code::should_scale_to_monitor` - set to make the window resized based on whatever monitor content scale it is placed on
    - `creation_code::defaults` - none of the above options

#### **Member functions**
- `window(int width, int height, const std::string &name, creation_hints hints = {})`
    - constructs window
    - **Parameters**
        - **width** - width of window (in screen coordinates)
        - **height** - height of window (in screen coordinates)
        - **name** - title of window
        - **hints** - hints for window creation
- `bool should_close() const`
    - returns true if the window has received a closing signal
- `void swap_buffers() cosnt`
    - swaps double buffers
- `void poll_events() const`
    - gets all types of input if available
- `void wait_events() const`
    - waits for any input
- `key *get_key(key_code code) const`
    - gets handle to key specified by code (see below for specification of `key` and `key_code`)
    - **Parameters**
        - **code** - code of key
- `key *get_mouse_button(mouse_code code) const`
    - gets handle to mouse button specified by code (see below for specification of `key` and `mouse_code`)
    - **Parameters**
        - **code** - code of mouse button
- `ivec2 get_window_size() const`
    - returns window size in screen coordinates
- `ivec2 get_framebuffer_size() const`
    - returns window size in pixels
- `ivec2 get_window_pos() const`
    - returns location of window in screen coordinates
- `dvec2 get_mouse_pos() const`
    - returns location of mouse in coordinates defined by `drawable_size()`
- `void set_window_title(const std::string &name)`
    - changes the title of the window
    - **Parameters**
        - **name** - new title of window
- `void set_window_size(int width, int height)`
    - changes the size of the window in screen coordinates
    - **Parameters**
        - **width** - width of window
        - **height** - height of window
- `void set_logical_size(int width, int height)`
    - sets a logical size of the window that may be preffered over the framebuffer size
    - **Parameters**
        - **width** - logical width of window
        - **height** - logical height of window
- `void set_window_size_limits(ivec2 min_size, ivec2 max_size)`
    - sets window minimum and maximum size in screen coordinates (to specify no limit on a component, set it to -1)
    - **Parameters**
        - **min_size** - minimum size limit for the window
        - **max_size** - maximum size limit for the window
- `void set_aspect_ratio(int numerator, int denominator)`
    - sets the aspect ratio of the window that will be maintained on resize
    - **Parameters**
        - **numerator** - numerator of aspect ratio
        - **denominator** - denominator of aspect ratio
- `void set_window_pos(ivec2 screen_pos)`
    - sets the position of the window in screen coordinates
    - **Parameters**
        - **screen_pos** - integer position
- `void set_should_close(bool should_close)`
    - sets whether or not the window should close
    - **Parameters**
        - **should_close** - true if the window should close and false if it should not close
- `void set_cursor_mode(state_code state)`
    - set state of cursor
    - **Parameters**
        - **state** - one of the following values
            - `state_code::enabled`
            - `state_code::hidden`
            - `state_code::disabled`
- `void set_raw_mouse_motion(bool state)`
    - set whether cursor acceleration is enabled or not
    - only available when cursor is disabled (See `set_cursor_mode`)
    - **Parameters**
        - **state** - true for raw mouse motion being on, false for off
- `void set_close_callback(std::function<void()> callback)`
    - set callback for when the user tries to close the window
    - **Parameters**
        - **callback** - function object returning void with no parameters
- `void set_windowsize_callback(std::function<void(int, int)> callback)`
    - set callback for when the size of the window changes
    - **Parameters**
        - **callback** - function object with the signature `void(width, height)`, `where` width and `height` represent the new size of the window (in screen coordinates)
- `void set_framebuffer_callback(std::function<void(int, int)> callback)`
    - set callback for when the framebuffer size changes
    - There is no need to change the viewport with this callback, that is handled by the window
    - **Parameters**
        - **callback** - function object with the signature `void(width, height)`, where `width` and `height` repressent the new size of the framebuffer
- `void set_contentscale_callback(std::function<void(float, float)> callback)`
    - set callback for when the content scale of the window changes
    - **Parameters**
        - **callback** - function object with the signature `void(xscale, yscale)`, where `xscale` and `yscale` represent the new scale the current DPI and the platforms default DPI
- `void set_windowpos_callback(std::function<void(int, int)> callback)`
    - set callback for when the screen position of the window changes
    - **Parameters**
        - **callback** - function object with the signature `void(x, y)`, where `x` and `y` represent the new position of the window
- `void set_windowminimize_callback(std::function<void(bool)> callback)`
    - set callback for when the window is minimized
    - **Parameters**
        - **callback** - function object with the signature `void(minimized)`, where `minimized` is true if the window is minimized, false otherwise
- `void set_windowmaximize_callback(std::function<void(bool)> callback)`
    - set callback for when the window is maximized
    - **Parameters**
        - **callback** - function object with the signature `void(maximized)`, where `maximized` is true if the window is maximized, false otherwise
- `void set_windowfocus_callback(std::function<void(bool)> callback)`
    - set callback for when the window gains/loses focus
    - **Parameters**
        - **callback** - function object with the signature `void(focused)`, where `focused` is true if the window is focused, false otherwise
- `void set_key_callback(std::function<void(key_code, int, action_code, int)> callback)`
    - set callback for when a key is pressed or released
    - **Parameters**
        - **callback** - function object with the signature `void(key, scancode, action, mods)`, where `key` is the `key_code`, `scancode` is the platform-dependant scancode, `action` is the state of the key, and `mods` is an int with relevant modifier bits set (defined in `modifier_code` enum)
- `void set_character_callback(std::function<void(unsigned int)> callback)`
    - set callback for text input
    - **Parameters**
        - **callback** - function object with the signature `void(codepoint)`, where `codepoint` is a character in UTF-32
- `void set_cursor_callback(std::function<void(double, double)> callback)`
    - set callback for cursor position
    - **Parameters**
        - **callback** - function object with the signature `void(x, y)`, where `x` and `y` are the positions of the cursor in coordinates defined by `drawable_size()`
- `void set_enterexit_callback(std::function<void(int)> callback)`
    - set callback for cursor entering/exiting
    - **Parameters**
        - **callback** - function object with the signature `void(entered)`, where `entered` is true if the mouse entered the window, and false otherwise
- `void set_mousebutton_callback(std::function<void(mouse_code, action_code, int)> callback)`
    - set callback for when a mouse button is pressed or released
    - **Parameters**
        - **callback** - function object with the signature `void(button, action, mods)`, where `button` is the `mouse_code`, `action` is the `action_code`, and `mods` is an int with relevant modifier bits set (defined in `modifier_code` enum)
- `void set_scroll_callback(std::function<void(double, double)> callback)`
    - set callback for when the user scrolls the mouse
    - **Parameters**
        - **callback** - function object with the signature `void(xoff, yoff)`, where `xoff` and `yoff` are the horizontal and vertical offsets from the wheel, respectively

- **The following functions remove their respective callbacks**
    - `void remove_windowsize_callback()`
    - `void remove_framebuffer_callback()`
    - `void remove_contentscale_callback()`
    - `void remove_windowpos_callback()`
    - `void remove_windowminimize_callback()`
    - `void remove_windowmaximize_callback()`
    - `void remove_windowfocus_callback()`
    - `void remove_key_callback()`
    - `void remove_character_callback()`
    - `void remove_cursor_callback()`
    - `void remove_enterexit_callback()`
    - `void remove_mousebutton_callback()`
    - `void remove_scroll_callback()`

- `static void set_swap_interval(int interval)`
    - sets the number of screen updates that the window will wait for between swapping buffers
    - **Parameters**
        - **interval** - number of screen updates
#### **Inherited methods**
- `void draw(const render_obj &obj, const render_settings &settings)`
    - draws object onto window. Further explained [here](#drawing).
    - **Parameters**
        - **obj** - object to be drawn
        - **settings** - settings for render operation
- `void draw(const render_obj &obj)`
    - draws object onto window using default settings. Further explained [here](#drawing).
    - **Parameters**
        - **obj** - object to be drawn
- `void clear(vec4 color, GLbitfield mask)`
    - Clears the specified buffers of the windows
    - **Parameters**
        - **color** - clear color
        - **mask** - bitmask with the specified buffer bits set (defaults to `GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT`)
- `ivec2 drawable_size() const`
    - returns either set logical size or framebuffer size to be used for drawing
- `ivec2 actual_size() const`
    - returns `get_framebuffer_size()`
### **Input**
---
There are two main ways of getting input in sgl, and both are handled through the `sgl::window` class.  
- By use of `sgl::key`  
- By using callbacks

#### `key`
- `sgl::key`'s are handled by `sgl::window`, and are accessed through member functions `sgl::window::get_key` and `sgl::window::get_mouse_button`, which both return pointers to the key/button requested  

#### **Member functions**
- `bool is_pressed() const`
    - returns true if the key/button is currently pressed
- `bool is_repeated() const`
    - returns true if the key/button has been pressed for more than one call of `sgl::window::poll_events` or `sgl::window::wait_events`
- `bool is_initial_press() const`
    - returns true if the key/button has been pressed for only one call of `sgl::window::poll_events` or `sgl::window::wait_events`
#### Key Codes
Key codes are found in enum `key_code` and to be used with `sgl::window::get_key`, and all of their values are self-explanatory
- `key_code::uknown`
- `key_code::space`
- `key_code::apostrophe`
- `key_code::comma`
- `key_code::minus`
- `key_code::period`
- `key_code::slash`
- `key_code::key_0`
- `key_code::key_1`
- `key_code::key_2`
- `key_code::key_3`
- `key_code::key_4`
- `key_code::key_5`
- `key_code::key_6`
- `key_code::key_7`
- `key_code::key_8`
- `key_code::key_9`
- `key_code::semicolon`
- `key_code::equal`
- `key_code::a`
- `key_code::b`
- `key_code::c`
- `key_code::d`
- `key_code::e`
- `key_code::f`
- `key_code::g`
- `key_code::h`
- `key_code::i`
- `key_code::j`
- `key_code::k`
- `key_code::l`
- `key_code::m`
- `key_code::n`
- `key_code::o`
- `key_code::p`
- `key_code::q`
- `key_code::r`
- `key_code::s`
- `key_code::t`
- `key_code::u`
- `key_code::v`
- `key_code::w`
- `key_code::x`
- `key_code::y`
- `key_code::z`
- `key_code::left_bracket`
- `key_code::backslash`
- `key_code::right_bracket`
- `key_code::grave_accent`
- `key_code::escape`
- `key_code::enter`
- `key_code::tab`
- `key_code::backspace`
- `key_code::insert`
- `key_code::key_delete`
- `key_code::right`
- `key_code::left`
- `key_code::down`
- `key_code::up`
- `key_code::page_up`
- `key_code::page_down`
- `key_code::home`
- `key_code::end`
- `key_code::caps_lock`
- `key_code::scroll_lock`
- `key_code::num_lock`
- `key_code::print_screen`
- `key_code::pause`
- `key_code::f1`
- `key_code::f2`
- `key_code::f3`
- `key_code::f4`
- `key_code::f5`
- `key_code::f6`
- `key_code::f7`
- `key_code::f8`
- `key_code::f9`
- `key_code::f10`
- `key_code::f11`
- `key_code::f12`
- `key_code::f13`
- `key_code::f14`
- `key_code::f15`
- `key_code::f16`
- `key_code::f17`
- `key_code::f18`
- `key_code::f19`
- `key_code::f20`
- `key_code::f21`
- `key_code::f22`
- `key_code::f23`
- `key_code::f24`
- `key_code::f25`
- `key_code::keypad_0`
- `key_code::keypad_1`
- `key_code::keypad_2`
- `key_code::keypad_3`
- `key_code::keypad_4`
- `key_code::keypad_5`
- `key_code::keypad_6`
- `key_code::keypad_7`
- `key_code::keypad_8`
- `key_code::keypad_9`
- `key_code::keypad_decimal`
- `key_code::keypad_divide`
- `key_code::keypad_multiply`
- `key_code::keypad_subtract`
- `key_code::keypad_add`
- `key_code::keypad_enter`
- `key_code::keypad_equal`
- `key_code::left_shift`
- `key_code::left_control`
- `key_code::left_alt`
- `key_code::left_super`
- `key_code::right_shift`
- `key_code::right_control`
- `key_code::right_alt`
- `key_code::right_super`
- `key_code::menu`
#### Mouse Codes
Mouse codes are found in enum `mouse_code` and are to be used with `sgl::window::get_mouse_button`, and their names are self-explanatory
- `mouse_code::button_1`
- `mouse_code::button_2`
- `mouse_code::button_3`
- `mouse_code::button_4`
- `mouse_code::button_5`
- `mouse_code::button_6`
- `mouse_code::button_7`
- `mouse_code::button_8`
- `mouse_code::left`
- `mouse_code::right`
- `mouse_code::middle`
#### Example Code
```
sgl::window window(500, 500, "example");
while (!window.should_close()) {
    window.wait_events();
    if (window.get_key(sgl::key_code::escape)->is_pressed())
        window.set_should_close(true);
}
```

#### Callbacks
Callbacks are handled by `sgl::window`, and are all listed and explained in the `sgl::window` documentation [here](#window-management).

## Drawing
Drawing operations are handled by the following classes
- `sgl::viewport`
- `sgl::render_target`
- `sgl::render_type`
- `sgl::rendervao_type`
- `sgl::render_settings`
- `sgl::render_obj`
- `sgl::transformable_obj`
- `sgl::shader`
- `sgl::render_shader`
- `sgl::lighting_engine`
### `viewport`
An `sgl::viewport` is a representation of a viewport for a `render_target`
#### **Public member functions**
- `void apply() const`
    - calls `glViewport` with members `pos` and `size`
#### **Public members**
- `ivec2 pos`
    - position of viewport
    - passed to members `x` and `y` of `glViewport(int x, int y, int width, int height)`
- `ivec2 size`
    - size of viewport
    - passed to members `width` and `height` of `glViewport(int x, int y, int width, int height)`

### `render_target`
An `sgl::render_target` is an abstract type representing a target for drawing operations.  
Two common classes derived form this class are `sgl::window` and `sgl::texture_target`

#### **Public member functions**
- `void draw(const render_obj &obj, const render_settings &settings)`
    - draws object onto target
    - **Parameters**
        - **obj** - object to be drawn
        - **settings** - settings for render operation
- `void draw(const render_obj &obj)`
    - draws object onto target using default settings
    - **Parameters**
        - **obj** - object to be drawn
- `void clear(vec4 color, GLbitfield mask)`
    - Clears the specified buffers of the target
    - **Parameters**
        - **color** - clear color
        - **mask** - bitmask with the specified buffer bits set (defaults to `GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT`)
- `ivec2 drawable_size() const`
    - pure virtual method used in drawing operations that returns the size of target that can be drawn onto
- `ivec2 actual_size() const`
    - pure virtual method used to set viewport dimensions
- `void set_viewport(viewport view)`
    - sets viewport of target to be used for drawing operations
    - **Parameters**
        - **view** - viewport to be used
- `void set_logical_viewport()`
    - sets viewport to a default settings. (uses `render_target::actual_size`)

#### **Protected member functions**
- `void bind_framebuffer()`
    - pure virtual method that is called before drawing operations. This function should bind the active framebuffer for the derived target
#### **Protected members**
- `viewport view`
    - variable that will be used to set the viewport of drawing operations
#### Derived classes of `render_target`
- [texture_target](#texture-target)
- [window](#window-management)

### `render_type`
An `sgl::render_type` is an abstract representation of a generic rendering type. It doesn't represent specific instances. Examples of this would be a cube, rectangle, or triangle.

#### **Public member functions**
- `void draw(render_target &target) const`
    - pure virtual method that is called by instances of a `render_type`, which draws a base instance of the type without shader setup
    - **Parameters**
        - **target** - `render_target` to be drawn onto

#### **Protected member functions**
- `void bind_target(render_target &target) const`
    - helper function that should be called before drawing operations. It applies the targets viewport and binds its framebuffer
    - **Parameters**
        - **target** - `render_target` that is being drawn onto

#### Derived classes of `render_type`
- [rendervao_type](#rendervao_type)

### `rendervao_type`
An abstract representation of a generic rendering type, which stores a vao. It doesn't represent specific instances. Examples would be a cube, rectangle, or triangle.

#### **Public member functions**
- `void draw(render_target &target) const`
    - pure virtual method that is called by instances of a `rendervao_type`, which draws a base instance of the type without shader setup
    - **Parameters**
        - **target** - `render_target` to be drawn onto

### `render_settings`
A type holding settings useful for rendering operations.

#### **Public member functions**
- `render_settings()`
    - constructs a `render_settings` with color black
- `render_settings(vec4 col)`
    - constructs a `render_settings` with color `col`
    - **Parameters**
        - **col** - color for rendering operation
- `render_settings(render_shader *shader_program)`
    - constructs a `render_settings` with color black and shader `shader_program`
    - **Parameters**
        - **shader_program** - a pointer to the [render_shader](#render_shader) object that will be used for drawing operation
- `render_settings(const lighting_engine *light_engine)`
    - constructs a `render_settings` with color black and lighting engine `light_engine`
    - **Parameters**
        - **light_engine** - a pointer to the [lighting_engine](#lighting_engine) object that will be used for drawing
- `render_settings(const abstract_material *obj_material)`
    - constructs a `render_settings` with color black and material `obj_material`
    - **Parameters**
        - **obj_material** - a pointer to the [abstract_material](#abstract_material) object that will be used for drawing
- `render_settings(vec4 col, render_shader *shader_program, const lighting_engine *light_engine, const abstract_material *obj_material)`
    - constructs a `render_settings` with the provided settings
    - **Parameters**
        - **col** - color for rendering operation
        - **shader_program** - a pointer to the [render_shader](#render_shader) object that will be used for drawing operation
        - **light_engine** - a pointer to the [lighting_engine](#lighting_engine) object that will be used for drawing
        - **obj_material** - a pointer to the [abstract_material](#abstract_material) object that will be used for drawing

### `render_obj`
An `sgl::render_obj` represents an instance of a render_type.

#### **Public member functions**
- `void draw(render_target &target, const render_settings &settings) const`
    - pure virtual method that draws an instance onto `target` with settings `settings`
    - **Parameters**
        - **target** - `render_target` to be drawn onto
        - **settings** - `render_settings` that dictate settings for the draw operation
- `void draw(render_target &target) const`
    - virtual method that draws an instance onto `target` with default settings
    - **Parameters**
        - **target** - `render_target` to be drawn onto

#### **Protected member functions**
- `render_obj(const render_type &rtype)`
    - constructs a `render_obj` of type `rtype`
    - **Parameters**
        - **rtype** - `render_type` representing the type of this instance
- `render_obj()`
    - construct a `render_obj` that has no type
- `void bind_target(render_target &target) const`
    - binds framebuffer of target to be used for drawing
    - must be called before drawing operations
    - **Parameters**
        - **target** - `render_target` to be drawn onto
- `const vao *get_vao() const`
    - returns the `vao` owned by the type this `render_obj` stores, if it stores on, otherwise, `nullptr`

#### Derived classes of `render_obj`
- [transformable_obj](#transformable_obj)
- [base_transformable_obj](#base_transformable_obj)
- [movable_obj](#movable_obj)
- [scalable_obj](#scalable_obj)
- [rotatable_obj](#rotatable_obj)
