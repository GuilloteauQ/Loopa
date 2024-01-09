# Loopa

Header-only library to easily implement feedback loops with Lua in C

## Usage

1. Download locally `loopa.h`

2. Include `loopa.h`

```c
#define LOOPA_IMPLEMENTATION
// optional
// #define LOOPA_ACTUATOR_TYPE uint64_t
// #define LOOPA_SENSOR_TYPE double
#include "loopa.h"
```
3. Initialize the `Lua_State`

```c
lua_State* L = loopa_init("your_lua_file.lua");
// ...
function(L);
// ...
loopa_close(L);
```

4. Define your controller in Lua

The Lua class is expected to be named `Controller`, but this can be overloaded with the `LOOPA_CLASS_NAME` macro.

```c
#define LOOPA_CLASS_NAME "MyControllerClass"
//...
#include "loopa.h"
```

The class **must implement** at least the following methods:

- `Controller:init()`: initialize the controller

- `Controller:recv_sensor(sensor)`: receive the new sensor value and update the inner state of the controller

- `Controller:send_actuator()`: return the new value of the actuator

Here is an example for a PI Controller:

```lua
-- your_lua_file.lua
Controller = {}

function Controller:init() 
  local ret = { u = 0, kp = 2000, ki = 1000, ref = 0.02, cumerror = 0.0 }
  setmetatable(ret, Controller)
  return ret
end

function Controller:recv_sensor(sensor)
  local error = self.ref - sensor
  self.cumerror = self.cumerror + error
  self.u = self.kp * error + self.ki * self.cumerror
  if self.u < 0 then
    self.u = 0
  end
end

function Controller:send_actuator()
  return math.floor(self.u)
end
```

5. Call the controller from C

For example:
```c
// function(L)
// ...
cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
loopa_send_sensor(L, cpu_time_used);
iterations = loopa_recv_actuator(L);
// ...
```

## Examples

### `examples/mandelbrot`

This example compute the Mandelbrot set.
The maximum depth of convergence is controlled with respect to the iteration speed.
The control loop is executed at the end of each iteration.

### `examples/monte_carlo_pi`

This example computes an estimation of PI via a Monte Carlo simulation.
The number of concurrent threads is controlled with respect to the `loadavg` of the host machine.
The control loop is running in another thread.
