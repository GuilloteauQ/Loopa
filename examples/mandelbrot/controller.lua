Controller = {}
-- Controller.__index = Controller

function Controller:init() 
  local ret = { u = 0, kp = 2000, ki = 1000, ref = 0.02, cumerror = 0.0 }
  setmetatable(ret, Controller)
  print("sensor, u")
  return ret
end

function Controller:recv_sensor(sensor)
  local error = self.ref - sensor
  self.cumerror = self.cumerror + error
  self.u = self.kp * error + self.ki * self.cumerror
  print(sensor .. "," .. self.u)
  if self.u < 0 then
    self.u = 0
  end
end

function Controller:send_actuator()
  return math.floor(self.u)
end
