model Blower
  parameter Real strength = 8;
  parameter Real time_fac = 1;
  Real g;
equation
  g = strength * sin(time_fac*time);
end Blower;
