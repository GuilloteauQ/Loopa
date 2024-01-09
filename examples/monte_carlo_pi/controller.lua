Controller = {}
-- Controller.__index = Controller

function Controller:init() 
  local ret = { u = 0, kp = 0.1, ki = 0.5, ref = 4, cumerror = 0.0 }
  setmetatable(ret, Controller)
  print("sensor, u")
  return ret
end

function Controller:recv_sensor(sensor)
  local error = self.ref - sensor
  self.cumerror = self.cumerror + error
  self.u = self.kp * error + self.ki * self.cumerror
  print(sensor .. "," .. self.u)
  if self.u < 1 then
    self.u = 1
  end
  if self.u > 100 then
    self.u = 100
  end
end

function Controller:send_actuator()
  return math.floor(self.u)
end
